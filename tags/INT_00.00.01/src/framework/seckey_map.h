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
class CUdpSockData;
class CSecKeyMap
{
public:

    static const unsigned int CLEAN_THRESHOLD = 100;
    class CItem
    {
public:
        secret_key_t   key;
        CUdpSockData * udp;
        struct sockaddr_in addr;

        time_t tm;
        CItem()
        {
        }
        CItem(const secret_key_t & k, CUdpSockData * u, const struct sockaddr_in & ad, const time_t &t) :
            key(k), udp(u), addr(ad), tm(t)
        {

        }
    };
    typedef map<secret_key_t, CItem> secret_key_map_t;
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

        it = _map.find(k1);
        if (it != _map.end() && ((time_t)(it->second.tm + _cfg.secret_timeout) >= now))
        {
            return false;
        }

        _map[k1] = CItem(k2, u, v, now);
        return true;
    }
    secret_key_map_t::const_iterator find(const secret_key_t & k)
    {
        CAutoMutex dumy(_lk);
        return _map.find(k);
    }

    bool verifykey(const secret_key_t & k1, const secret_key_t & k2, in_addr_t ip, CUdpSockData * & udp,
            struct sockaddr_in & udpaddr)
    {
        CAutoMutex dumy(_lk);
        secret_key_map_t::iterator it = _map.find(k1);
        if (it == _map.end())
        {
            return false;
        }
        if (it->second.key != k2 || it->second.addr.sin_addr.s_addr != ip)
        {
            return false;
        }
        memcpy(&udpaddr, &(it->second.addr), sizeof(udpaddr));
        udp = it->second.udp;
        _map.erase(it);
        return true;
    }
    void remove(const secret_key_t & k)
    {
        CAutoMutex dumy(_lk);
        _map.erase(_map.find(k));
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
