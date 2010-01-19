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
#include "config.h"
#include "client.h"
#include "log.h"
#include "broadcaster.h"

class CRoom
{
public:
    typedef set <CClientPtr> usr_list_t;
private:
    const CRoomConfig & _cfg;
    CMutex _lk;
    usr_list_t _usrlist;
public:
    CRoom(const CRoomConfig & cfg) :
        _cfg(cfg)
    {

    }
    CXmlNode toXml()
    {
        CXmlMsg nd;
        nd.create(xmlTagRoom);
        nd.addParameter(xmlTagCapacity, capacity());
        nd.addParameter(xmlTagUserCount, _usrlist.size());
        nd.addParameter(xmlTagDescription, description());
        nd.addParameter(xmlTagId, id());
        return nd;
    }
    unsigned int capacity()
    {
        return _cfg.capacity;
    }
    unsigned int id()
    {
        return _cfg.id;
    }
    const char * description()
    {
        return _cfg.description.c_str();
    }

    bool enter(CClientPtr c, CBroadCaster & bd);

    bool leave(CClientPtr c, CBroadCaster & bd);

};
typedef std::tr1::shared_ptr<CRoom> CRoomPtr;

#endif /*ROOM_H_*/
