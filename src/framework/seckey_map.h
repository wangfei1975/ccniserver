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
class CSecKeyMap
{
public:
    static const unsigned int TIME_OUT = 10; //10 seconds
    static const unsigned int CLEAN_THRESHOLD = 100;
    class CItem
    {
public:
        secret_key_t key;
        struct sockaddr_in addr;
        time_t tm;
        CItem()
        {
        }
        CItem(const secret_key_t & k, const struct sockaddr_in & ad, const time_t &t) :
            key(k), addr(ad), tm(t)
        {

        }
    };
    typedef map<secret_key_t, CItem> secret_key_map_t;
private:
    secret_key_map_t _map;
    CMutex _lk;
public:
    int size()
    {
        CAutoMutex dumy(_lk);
        return _map.size();
    }
    bool insert(const secret_key_t & k1, const secret_key_t & k2, const struct sockaddr_in & v)
    {
        time_t now = time(NULL);
        secret_key_map_t::iterator it;
        CAutoMutex dumy(_lk);
        if (_map.size() > CLEAN_THRESHOLD)
        {
            clear_timeouted(now);
        }

        it = _map.find(k1);
        if (it != _map.end() && ((time_t)(it->second.tm + TIME_OUT) >= now))
        {
            return false;
        }

        _map[k1] = CItem(k2, v, now);
        return true;
    }
    secret_key_map_t::iterator find(const secret_key_t & k)
    {
        CAutoMutex dumy(_lk);
        return _map.find(k);
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
            if ((time_t)(it->second.tm + TIME_OUT) < now)
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