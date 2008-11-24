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
    typedef map<secret_key_t, struct sockaddr_in> secret_key_map_t;
private:
    secret_key_map_t _map;
    CMutex _lk;
public:
    bool insert(const secret_key_t & k, const struct sockaddr_in & v)
    {
        CAutoMutex dumy(_lk);
        if (_map.find(k) != _map.end())
        {
            return false;
        }
        _map[k] = v;
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
};
#endif /*SECKEY_MAP_H_*/
