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
/*             data_mgr.h                                                              */
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
#ifndef DATA_MGR_H_
#define DATA_MGR_H_
#include <map>
#include <vector>
#include <string>
#include "log.h"
#include "mutex.h"
#include "client.h"
#include "ccni_msg.h"
#include "room.h"

class CDataMgr
{
public:
    typedef map <int, CRoom> room_list_t;
    typedef map<string, CClientPtr> client_list_t;
private:
    client_list_t _list;
    CMutex _listlk;

    room_list_t _rooms;

public:

    int userCount()
    {
        CAutoMutex du(_listlk);
        return _list.size();
    }
    //room related operations
    CRoom * findRoom(int roomid)
    {
        room_list_t::iterator it = _rooms.find(roomid);
        if (it == _rooms.end())
        {
            return NULL;
        }
        return &(it->second);
    }

    CClientPtr findClient(const char * uname)
    {
        CAutoMutex du(_listlk);
        client_list_t::iterator it = _list.find(uname);
        if (it == _list.end())
        {
            return CClientPtr();
        }
        return (it->second);
    }
    bool addClient(CClientPtr c)
    {
        CAutoMutex du(_listlk);
        client_list_t::iterator it = _list.find(c->uname());
        if (it != _list.end())
        {
            return false;
        }
        _list[c->uname()] = c;
        return true;
    }
    void delClient(const char * uname)
    {
        CAutoMutex du(_listlk);
        client_list_t::iterator it = _list.find(uname);
        if (it == _list.end())
        {
            LOGW("not find user %s.\n", uname);
            return;
        }
        _list.erase(it);
    }
    bool create()
    {
        return true;
    }
    void destroy()
    {
        client_list_t::iterator it;
        CAutoMutex du(_listlk);
        // we are using referenced counted ptr point to each client, 
        // so, we don't need delete each one any more.
        //for (it = _list.begin(); it != _list.end(); ++it)
        //{
        //  delete (it->second);
        //}
        _list.clear();
    }
};
#endif /*DATA_MGR_H_*/
