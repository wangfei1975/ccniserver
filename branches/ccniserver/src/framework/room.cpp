/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             room.cpp                                                                */
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
#include "room.h"

bool CRoom::enter(CClient * c, CBroadCaster & bd)
{
    usr_list_t::iterator it;
    CAutoMutex du(_lk);
    for (it = _usrlist.begin(); it != _usrlist.end(); ++it)
    {
        bd.insertaddr((*it)->udpsock(), (*it)->udpaddr());
    }
    return _usrlist.insert(c).second;
}

bool CRoom::leave(CClient * c, CBroadCaster & bd)
{
    usr_list_t::iterator it;
    CAutoMutex du(_lk);
    it = _usrlist.find(c);
    if (it != _usrlist.end())
    {
        _usrlist.erase(it);
        for (it = _usrlist.begin(); it != _usrlist.end(); ++it)
        {
            bd.insertaddr((*it)->udpsock(), (*it)->udpaddr());
        }
        return true;
    }
    return false;
}



