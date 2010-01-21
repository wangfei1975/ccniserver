/*
 Copyright (C) 2009  Wang Fei (bjwf2000@gmail.com)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Generl Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
#include "notifier.h"
int CSession::leave(CClientPtr c, CNotifier & notier)
{
    int ret = -1;
    set <CClientPtr>::iterator it;
    CAutoMutex dumy(_lk);
    if (_owner == c)
    {
        _owner.reset();
        ret = 0;
    }
    else if (_opponent == c)
    {
        _opponent.reset();
        ret = 0;
    }
    else
    {
        for (it = _watchers.begin(); it != _watchers.end(); ++it)
        {
            if ((*it) == c)
            {
                _watchers.erase(it);
                ret = 0;
                break;
            }
        }
    }
    if (ret < 0)
    {
        return ret;
    }
    if (_owner != NULL)
    {
        notier.addNotifiee(_owner);
        ret ++;
    }
    if (_opponent != NULL)
    {
        notier.addNotifiee(_opponent);
        ret ++;
    }
    for (it = _watchers.begin(); it != _watchers.end(); ++it)
    {
        notier.addNotifiee(*it);
        ret ++;
    }
    LOGD("leave ret = %d\n", ret);
    return ret;
}
int CSession::watch(CClientPtr c, CNotifier & notier)
{
    set <CClientPtr>::iterator it;
    CAutoMutex dumy(_lk);
    if ((int)_watchers.size() >= _cfg.watcherCount)
    {
        return -3;
    }
    if (_owner != NULL)
    {
        notier.addNotifiee(_owner);
    }
    if (_opponent != NULL)
    {
        notier.addNotifiee(_opponent);
    }
    for (it = _watchers.begin(); it != _watchers.end(); ++it)
    {
        notier.addNotifiee(*it);
    }
    _watchers.insert(c);
    return 0;
}
int CSession::enter(CClientPtr c, CNotifier & notier)
{
    set <CClientPtr>::iterator it;
    CAutoMutex dumy(_lk);
    if (_owner != NULL && _opponent != NULL)
    {
        return -3;
    }
    if (_owner != NULL)
    {
        notier.addNotifiee(_owner);
        _opponent = c;
    }
    if (_opponent != NULL)
    {
        notier.addNotifiee(_opponent);
        _owner = c;
    }
    for (it = _watchers.begin(); it != _watchers.end(); ++it)
    {
        notier.addNotifiee(*it);
    }
    return 0;
}
bool CRoom::enter(CClientPtr c, CBroadCaster & bd)
{
    usr_list_t::iterator it;
    CAutoMutex du(_lk);
    if (_usrlist.size() >= capacity())
    {
        return false;
    }
    if (_usrlist.insert(c).second)
    {
        for (it = _usrlist.begin(); it != _usrlist.end(); ++it)
        {
            if ((*it) != c)
            {
                bd.insertaddr((*it)->udpsock(), (*it)->udpaddr());
            }
        }
        return true;
    }
    return false;
}

bool CRoom::leave(CClientPtr c, CBroadCaster & bd)
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

CSessionPtr CRoom::createSession(CClientPtr c, const CSessionConfig & cfg, CBroadCaster & bd)
{
    {
        CAutoMutex dumy(_lk);
        usr_list_t::iterator it = _usrlist.find(c);
        ASSERT (it != _usrlist.end())
        for (it = _usrlist.begin(); it != _usrlist.end(); ++it)
        {
            if (*it != c)
            {
                bd.insertaddr((*it)->udpsock(), (*it)->udpaddr());
            }
        }
    } // end scope for  CAutoMutex _lk.


    CSessionPtr s(new CSession(c, cfg));
    CAutoMutex ddumy(_slk);
    _sessions[s->id()] = s;
    return s;
}
bool CRoom::leaveSession(int sid, CClientPtr c, CNotifier & noti, CBroadCaster & bd)
{
    session_list_t::iterator it;
    CAutoMutex ddumy(_slk);
    it = _sessions.find(sid);
    ASSERT(it != _sessions.end());

    int ret = it->second->leave(c, noti);

    ASSERT(ret >= 0);

    if (ret == 0)
    {
        LOGD("session empty, delete it\n");
        _sessions.erase(it);
        usr_list_t::iterator uit;
        for (uit = _usrlist.begin(); uit != _usrlist.end(); ++uit)
        {
            // if (*uit != c)
            {
                bd.insertaddr((*uit)->udpsock(), (*uit)->udpaddr());
            }
        }
    }
    return true;
}
int CRoom::enterSession(uint32_t sid, CClientPtr c, CNotifier & noti)
{
    session_list_t::iterator it;
    CAutoMutex du(_slk);
    it = _sessions.find(sid);

    if (it == _sessions.end())
    {
        return -2;
    }

    return it->second->enter(c, noti);
}
int CRoom::watchSession(uint32_t sid, CClientPtr c, CNotifier & noti)
{
    session_list_t::iterator it;
    CAutoMutex du(_slk);
    it = _sessions.find(sid);
    if (it == _sessions.end())
    {
        return -2;
    }
    return it->second->watch(c, noti);
}
