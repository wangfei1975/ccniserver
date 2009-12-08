/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             seckey_map.h                                                            */
/*                                                                                     */
/*  version                                                                            */
/*             1.0                                                                     */
/*                                                                                     */
/*  description                                                                        */
/*                                                                                     */
/*                                                                                     */
/*  component                                                                          */
/*                                                                                     */
/*  author                                                                             */
/*             bjwf       bjwf2000@gmail.com                                           */
/*                                                                                     */
/*  histroy                                                                            */
/*             2008-11-23     initial draft                                            */
/***************************************************************************************/

#ifndef SECKEY_MAP_H_
#define SECKEY_MAP_H_
#include "log.h"
#include "ccni.h"
#include "config.h" 
#include "utils.h"
class CUdpSockData;
class CSecKeyMap
{
public:

    static const unsigned int CLEAN_THRESHOLD = 100;
    class CItem
    {
public:
        CUdpSockData * udp;
        struct sockaddr_in addr;
        time_t tm;
        
        CItem()
        {
        }
        CItem(CUdpSockData * u, const struct sockaddr_in & ad, const time_t &t) :
           udp(u), addr(ad), tm(t)
        {

        }
    };
    class CSecretKey
    {
    public:
         secret_key_t k1,k2;
         CSecretKey(const secret_key_t & h, const secret_key_t & l):k1(h),k2(l){}
         bool operator <(const CSecretKey & k) const
         {
             if (k1 != k.k1) return k1 < k.k1;
             return k2 < k.k2;
         }
    };
    typedef map<CSecretKey, CItem> secret_key_map_t;
private:
    const CConfig & _cfg;
    secret_key_map_t _map;
    CMutex _lk;
public:
    CSecKeyMap(const CConfig & cfg) :
        _cfg(cfg)
    {

    }

    int size()
    {
        CAutoMutex dumy(_lk);
        return _map.size();
    }
    bool insert(const secret_key_t & k1, const secret_key_t & k2, CUdpSockData * u, const struct sockaddr_in & v)
    {
        time_t now = time(NULL);
        secret_key_map_t::iterator it;
        CAutoMutex dumy(_lk);
        if (_map.size() > CLEAN_THRESHOLD)
        {
            clear_timeouted(now);
        }
        CSecretKey k(k1,k2);
        it = _map.find(k);
        if (it != _map.end() && ((time_t)(it->second.tm + _cfg.secret_timeout) >= now))
        {
            return false;
        }

        _map[k] = CItem(u, v, now);
        return true;
    }
    secret_key_map_t::const_iterator find(const secret_key_t & k1, const secret_key_t & k2)
    {
        CAutoMutex dumy(_lk);
        return _map.find(CSecretKey(k1, k2));
    }

    bool verifykey(const secret_key_t & k1, const secret_key_t & k2, in_addr_t ip, CUdpSockData * & udp,
            struct sockaddr_in & udpaddr)
    {
        CAutoMutex dumy(_lk);
        secret_key_map_t::iterator it = _map.find(CSecretKey(k1, k2));
        if (it == _map.end())
        {
            return false;
        }
        if (it->second.addr.sin_addr.s_addr != ip)
        {
            return false;
        }
        memcpy(&udpaddr, &(it->second.addr), sizeof(udpaddr));
        udp = it->second.udp;
        _map.erase(it);
        return true;
    }
    void remove(const secret_key_t & k1, const secret_key_t & k2)
    {
        CAutoMutex dumy(_lk);
        _map.erase(_map.find(CSecretKey(k1, k2)));
    }
    //generate secret key,
    //
    static void genSecret(uint8_t * outbuf, const struct sockaddr_in & addr, const char * sec)
    {
        static CMutex mu;
        static uint32_t serno = 0;
        int seclen = strlen(sec);
        uint8_t buf[512];
        mu.lock();
        memcpy(buf, &(++serno), sizeof(serno));
        mu.unlock();
        memcpy(buf + sizeof(serno), &addr, sizeof(addr));
        memcpy(buf + sizeof(serno) + sizeof(addr), sec, seclen);
        md5_calc(outbuf, buf, sizeof(serno) + sizeof(addr) + seclen);
    }
private:
    void clear_timeouted(time_t now)
    {
        secret_key_map_t::iterator it = _map.begin();
        while (it != _map.end())
        {
            if ((time_t)(it->second.tm + _cfg.secret_timeout) < now)
            {
                secret_key_map_t::iterator tit = it;
                ++it;
                _map.erase(tit);
            }
            else
            {
                ++it;
            }
        }
    }
};
#endif /*SECKEY_MAP_H_*/
