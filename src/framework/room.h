/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             room.h                                                                  */
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
/*             2008-12-09     initial draft                                            */
/***************************************************************************************/
#ifndef ROOM_H_
#define ROOM_H_
#include <string>
#include <set>
#include "client.h"
#include "log.h"
#include "broadcaster.h"

class CRoom
{
public:
    typedef set <CClient *> usr_list_t;
private:
    int _capacity;
    int _id;
    CMutex _lk;
    usr_list_t _usrlist;
public:
    bool enter(CClinet * c, CBroadCaster & bd)
    {
        usr_list_t::iterator it;
        CAutoMutex du(_lk);
        for (it = _usrlist.begin(); it != _usrlist.end(); ++it)
        {
            bd.insertaddr((*it)->udpsock(), (*it)->udpaddr);
        }
        return _usrlist.insert(c).second;
    }
    bool leave(CClient * c, CBroadCaster & bd)
    {
        usr_list_t::iterator it;
        CAutoMutex du(_lk);
        it = _usrlist.find(c);
        if (it != _usrlist.end())
        {
            _usrlist.erase(it);
            for (it = _usrlist.begin(); it != _usrlist.end(); ++it)
            {
                bd.insertaddr((*it)->udpsock(), (*it)->udpaddr);
            }
            return true;
        }
        return false;
    }
};

#endif /*ROOM_H_*/
