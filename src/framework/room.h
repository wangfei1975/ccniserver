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
class CSessionConfig
{
public:
    string des;
    string fen;
    int red;
    int innings;
    int rule;
    int totalTime;
    int stepTime;
    int bonusPerStep;
    int requiredScore;
    int ante;
    int watcherCount;

    CSessionConfig() :
        des("session"), fen("hello"), red(0), innings(0), rule(0), totalTime(10*60), stepTime(30),
                bonusPerStep(3), requiredScore(0), ante(0), watcherCount(200)
    {

    }
};
class CSession
{
public:

private:
    int _id;
    CSessionConfig _cfg;
    CClientPtr _owner, _opponent;

    set <CClientPtr> _watchers;
    CMutex _lk;

    static int genid()
    {
        static int idseed = 0;
        idseed = (idseed + 1)% 20000;
        return (idseed+1);
    }
public:
    CSession(CClientPtr ow, const CSessionConfig & cfg) :
        _cfg(cfg), _owner(ow)
    {
        _id = genid();
    }

    uint32_t id()
    {
        return _id;
    }
    //   < 0 error
    //   >=0 clients number in session 
    int leave(CClientPtr c, CNotifier & notier);
    int enter(CClientPtr c, CNotifier & notier);
    
    // -3 session full
    int watch(CClientPtr c, CNotifier & notier);
    CXmlNode toXml()
    {
        CXmlMsg nd;
        nd.create(xmlTagSession);
        nd.addParameter(xmlTagId, id());
        nd.addParameter(xmlTagDescription, _cfg.des.c_str());
        nd.addParameter(xmlTagFen, _cfg.fen.c_str());
        nd.addParameter(xmlTagRed, _cfg.red);
        nd.addParameter(xmlTagInnings, _cfg.innings);
        nd.addParameter(xmlTagRule, _cfg.rule);
        nd.addParameter(xmlTagTotalTime, _cfg.totalTime);
        nd.addParameter(xmlTagStepTime, _cfg.stepTime);
        nd.addParameter(xmlTagBonusPerStep, _cfg.bonusPerStep);
        nd.addParameter(xmlTagRequiredScore, _cfg.requiredScore);
        nd.addParameter(xmlTagAnte, _cfg.ante);
        nd.addParameter(xmlTagMaxWatcherCount, _cfg.watcherCount);
        CAutoMutex du(_lk);
        if (_owner != NULL)
        {
            nd.addParameter(xmlTagOwner, _owner->uname());
        }
        if (_opponent != NULL)
        {
            nd.addParameter(xmlTagOpponent, _opponent->uname());
        }
        CXmlMsg wt;
        wt.create(xmlTagWatchers);
        set <CClientPtr>::iterator it;
        for (it = _watchers.begin(); it != _watchers.end(); ++it)
        {
            wt.addParameter(xmlTagUserName, (*it)->uname());
        }
        nd.addChild(wt);
        return nd;
    }
};
typedef std::tr1::shared_ptr<CSession> CSessionPtr;

class CRoom
{
public:
    typedef set<CClientPtr> usr_list_t;
    typedef map<uint32_t, CSessionPtr> session_list_t;
private:
    const CRoomConfig & _cfg;
    CMutex _lk;
    usr_list_t _usrlist;

    CMutex _slk;
    session_list_t _sessions;
public:
    CRoom(const CRoomConfig & cfg) :
        _cfg(cfg)
    {

    }
    CSessionPtr createSession(CClientPtr c, const CSessionConfig & cfg, CBroadCaster & bd);
    bool leaveSession(int sid, CClientPtr c, CNotifier & noti, CBroadCaster & bd);
    //
    // ret -2 error session id
    //     -3 session full
    int enterSession(uint32_t sid, CClientPtr c, CNotifier & noti);
    
    //
    // ret -2 error session id
    //     -3 session full
    int watchSession(uint32_t sid, CClientPtr c, CNotifier & noti);
    
    
    CXmlNode xmlSessionList()
    {
        CXmlNode nd;
        nd.create(xmlTagSessions);
        CAutoMutex du(_slk);
        session_list_t::iterator it;
        for (it = _sessions.begin(); it != _sessions.end(); ++it)
        {
            nd.addChild(it->second->toXml());
        }
        return nd;
    }
    CXmlNode toXml()
    {
        CXmlMsg nd;
        nd.create(xmlTagRoom);
        nd.addParameter(xmlTagCapacity, capacity());
        nd.addParameter(xmlTagDescription, description());
        nd.addParameter(xmlTagId, id());
        CAutoMutex dumy(_lk);
        nd.addParameter(xmlTagUserCount, _usrlist.size());
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

    bool inroom(CClientPtr c)
    {
        CAutoMutex dumy(_lk);
        return (_usrlist.find(c) != _usrlist.end());
    }

    bool enter(CClientPtr c, CBroadCaster & bd);

    bool leave(CClientPtr c, CBroadCaster & bd);

};
typedef std::tr1::shared_ptr<CRoom> CRoomPtr;

#endif /*ROOM_H_*/
