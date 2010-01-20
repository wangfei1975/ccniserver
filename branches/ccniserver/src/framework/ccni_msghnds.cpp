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
/*             ccni_msghnds.cpp                                                        */
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

#include "client.h"
#include "room.h"
#include "engine.h"
#include "broadcaster.h"
#include "notifier.h"

#define IMPL_MSG_HANDLE(name) int CClient::name(CXmlNode msg)

CClient::hndtable_t CClient::msghnds[] =
{
{ xmlTagCCNI, stConnected, &CClient::doCCNI },
{ xmlTagMyState, stConnected, &CClient::doMyState },
{ xmlTagLogout, stOnline, &CClient::doLogout },
{ xmlTagEnterRoom, stOnline, &CClient::doEnterRoom },
{ xmlTagLeaveRoom, stIdle, &CClient::doLeaveRoom },
{ xmlTagNewSession, stIdle, &CClient::doNewSession },
{ xmlTagEnterSession, stIdle, &CClient::doEnterSession },
{ xmlTagLeaveSession, stInSession, &CClient::doLeaveSession },
{ xmlTagWatchSession, stIdle, &CClient::doWatchSession },
{ xmlTagReady, stSessional, &CClient::doReady },
{ xmlTagMove, stMoving, &CClient::doMove },
{ xmlTagDraw, stPlaying, &CClient::doDraw },
{ xmlTagGiveUp, stPlaying, &CClient::doGiveUp },
{ xmlTagListRooms, stOnline, &CClient::doListRooms },
{ xmlTagListSessions, stIdle, &CClient::doListSessions },
{ xmlTagSendMessage, stConnected, &CClient::doSendMessages },
{ NULL, stConnected, &CClient::doUnknow } };

bool CClient::checkstate(CXmlNode msg, state_t vstate)
{
    if ((_state & vstate) == 0)
    {
        string tag(msg.name());
        tag += "Res";
        CXmlMsg lgmsg;
        lgmsg.create(tag.c_str());
        lgmsg.addParameter(xmlTagReturnCode, -1);
        lgmsg.addParameter(xmlTagDescription, "user state error");
        _resmsg.appendmsg(lgmsg);
        return false;
    }
    return true;
}
void CClient::procMsgs(CCNIMsgParser & pmsg)
{

    int i;
    for (CXmlNode msg = pmsg.getfirst(); !msg.isEmpty(); msg = msg.next())
    {
        if (msg.type() != XML_ELEMENT_NODE)
        {
            continue;
        }
        for (i = 0; msghnds[i].label != NULL; i++)
        {
            if (strcmp(msghnds[i].label, msg.name()) == 0)
            {
                break;
            }
        }
        //
        // check state
        //
        if (!checkstate(msg, msghnds[i].vstate))
        {
            LOGW("user state error. req:%s cur state:%s\n", msg.name(), strstate());
            break;
        }

        if (_bder == NULL)
        {
            _bder = new CBroadCaster;
            _bder->create();
        }
        if (_notifier == NULL)
        {
            _notifier = new CNotifier;
            _notifier->create();
        }

        if ((this->*msghnds[i].fun)(msg) < 0)
        {
            break;
        }

        // in case of this processing generate some broadcast messages.
        // if broadcast msg is not empty, we assign it to threads poll.
        // if is empty, we don't need delete it, we can use it in next loop or next ccni message
        if (!_bder->empty())
        {
            LOGV("broadcaster is not emtpy\n")
            CEngine::instance().threadsPool().assign(_bder);
            _bder = NULL;
        }

        // in case of this processing generate some notification to other clients.
        if (!_notifier->empty())
        {
            LOGV("notifier is not emtpy\n")
            CEngine::instance().threadsPool().assign(_notifier);
            _notifier = NULL;
        }

    }

    //
    // in case of multiple ccni message in one xml request, we pack all response in one
    // ccni message. 
    //
    const CCNI_HEADER & hd(pmsg.header());

    if (!_resmsg.pack(hd.seq, hd.udata, hd.secret1, hd.secret2))
    {
        LOGE("pack error..\n");
    }

}

/*
 * 
 *   Message handles
 * 
 * 
 * */
#define doreturn(tag, rcode, des) {  CXmlMsg lgmsg; lgmsg.create(tag);\
                                     lgmsg.addParameter(xmlTagReturnCode, rcode); \
                                     lgmsg.addParameter(xmlTagDescription, des);\
                                     _resmsg.appendmsg(lgmsg);  \
                                     LOGV("out:ret code=%d,des=%s\n", rcode, des);  \
                                     return rcode;}

void CClient::leaveRoom()
{
    CDataMgr & dmgr = CEngine::instance().dataMgr();
    CRoomPtr rm = dmgr.findRoom(_roomid);
    ASSERT(rm != NULL);

    LOGV("%s leave room %d start\n", uname(), _roomid);

    if (!rm->leave(_ctsk->client(), *_bder))
    {
        ASSERT(false);
    }

    LOGV("%s leave room %d ok\n", uname(), _roomid);
    _state = stOnline;
    _roomid = -1;

    CXmlMsg bdmsg;
    if (!_bder->empty())
    {
        LOGV("leave room broadcast.\n");
        bdmsg.create(xmlTagBroadcastLeaveRoom);
        bdmsg.addParameter(xmlTagUserName, uname());
        _bder->append(bdmsg);
    }
    LOGV("%s leave room %d ok ok\n", uname(), _roomid);
}
int CClient::enterRoom(int rid)
{
    if (_state != stOnline || _ctsk == NULL)
    {
        return -1;
    }
    CDataMgr & dmgr = CEngine::instance().dataMgr();
    CRoomPtr room = dmgr.findRoom(rid);
    if (room == NULL)
    {
        return -2;
    }

    if (!room->enter(_ctsk->client(), *_bder))
    {
        return -3;
    }
    _state = stIdle;
    _roomid = rid;
    if (!_bder->empty())
    {
        LOGV("enter room broadcast.\n");
        CXmlMsg bdmsg;
        bdmsg.create(xmlTagBroadcastEnterRoom);
        bdmsg.addParameter(xmlTagUserName, uname());
        _bder->append(bdmsg);
    }
    return 0;
}
int CClient::createSession(const CSessionConfig & cfg)
{
    ASSERT (_state == stIdle && _ctsk != NULL && _roomid> 0);

    CDataMgr & dmgr = CEngine::instance().dataMgr();
    CRoomPtr room = dmgr.findRoom(_roomid);
    ASSERT(room != NULL);

    CSessionPtr s = room->createSession(_ctsk->client(), cfg, *_bder);
    if (s == NULL)
    {
        LOGE("create session error.\n")
        return -2;
    }
    _state = stSessional;
    _sessionid = s->id();
    if (!_bder->empty())
    {
        LOGV("enter room broadcast.\n");
        CXmlMsg bdmsg;
        bdmsg.create(xmlTagBroadcastNewSession);
        bdmsg.addChild(s->toXml());
        _bder->append(bdmsg);
    }
    return 0;
}
int CClient::leaveSession()
{
    ASSERT ((_ctsk != NULL && _roomid> 0 && _sessionid> 0));
    CDataMgr & dmgr = CEngine::instance().dataMgr();
    CRoomPtr room = dmgr.findRoom(_roomid);
    ASSERT(room != NULL);
    room->leaveSession(_sessionid, _ctsk->client(), *_notifier, *_bder);
    int sid = _sessionid;
    _state = stIdle;
    _sessionid = -1;

    if (!_bder->empty())
    {
        LOGV("broadcast.\n");
        CXmlMsg bdmsg;
        bdmsg.create(xmlTagBroadcastDelSession);
        bdmsg.addParameter(xmlTagId, sid);
        _bder->append(bdmsg);
    }
    if (!_notifier->empty())
    {
        LOGV("notification.\n");

        CXmlMsg bdmsg;
        bdmsg.create(xmlTagNotifyLeaveSession);
        bdmsg.addParameter(xmlTagUserName, uname());
        _notifier->append(bdmsg);
    }
    return 0;
}
int CClient::enterSession(int sid)
{
    ASSERT ((_ctsk != NULL && _roomid> 0 && _sessionid < 0));
    CDataMgr & dmgr = CEngine::instance().dataMgr();
    CRoomPtr room = dmgr.findRoom(_roomid);
    ASSERT(room != NULL);
    int ret = room->enterSession(sid, _ctsk->client(), *_notifier);
    if (ret < 0)
    {
        LOGE("enter session error.\n");
        return ret;
    }
    _state = stSessional;
    _sessionid = sid;
    if (!_notifier->empty())
    {
        LOGV("notification.\n");
        CXmlMsg bdmsg;
        bdmsg.create(xmlTagNotifyEnterSession);
        bdmsg.addParameter(xmlTagUserName, uname());
        _notifier->append(bdmsg);
    }
    return 0;
}
IMPL_MSG_HANDLE(doEnterRoom)
{
    LOGV("in\n");

    int roomid = msg.findChild(xmlTagId).getIntContent();

    int ret = enterRoom(roomid);
    if (ret == -1)
    {
        doreturn(xmlTagEnterRoomRes, -1, "user state error");
    }
    else if (ret == -2)
    {
        doreturn(xmlTagEnterRoomRes, -2, "unknow room id");
    }
    else if (ret == -3)
    {
        doreturn(xmlTagEnterRoomRes, -3, "room full");
    }

    doreturn(xmlTagEnterRoomRes, 0, "enter room success");

}

IMPL_MSG_HANDLE(doLeaveRoom)
{
    LOGV("in\n");

    ASSERT(_roomid> 0);
    leaveRoom();

    doreturn(xmlTagLeaveRoomRes, 0, "leave room success");
}

IMPL_MSG_HANDLE(doNewSession)
{
    LOGV("in\n");

    CSessionConfig cfg;
    int ret = createSession(cfg);
    if (ret == -1)
    {
        doreturn(xmlTagNewSessionRes, -1, "user state error");
    }
    else if (ret == -2)
    {
        doreturn(xmlTagNewSessionRes, -2, "error session parameter");
    }

    doreturn(xmlTagNewSessionRes, 0, "create session success");
    return 0;
}

IMPL_MSG_HANDLE(doEnterSession)
{
    LOGV("in\n");
    int sessionid = msg.findChild(xmlTagId).getIntContent();
    int ret = enterSession(sessionid);
    if (ret == -2)
    {
        doreturn(xmlTagEnterSessionRes, -2, "error session id");
    }
    else if (ret == -3)
    {
        doreturn(xmlTagEnterSessionRes, -3, "session full");
    }
    doreturn(xmlTagEnterSessionRes, 0, "enter session success");
    return 0;
}

IMPL_MSG_HANDLE(doLeaveSession)
{
    leaveSession();
    doreturn(xmlTagLeaveSessionRes, 0, "leave session success");
    return 0;
}

IMPL_MSG_HANDLE(doWatchSession)
{
    return 0;
}

IMPL_MSG_HANDLE(doReady)
{
    return 0;
}

IMPL_MSG_HANDLE(doMove)
{
    return 0;
}

IMPL_MSG_HANDLE(doDraw)
{
    return 0;
}

IMPL_MSG_HANDLE(doGiveUp)
{
    return 0;
}
IMPL_MSG_HANDLE(doListRooms)
{
    CDataMgr & dmgr = CEngine::instance().dataMgr();

    CXmlMsg lgmsg; lgmsg.create(xmlTagListRoomsRes);

    lgmsg.addParameter(xmlTagReturnCode, 0);
    lgmsg.addChild(dmgr.xmlRoomList());

    _resmsg.appendmsg(lgmsg);

    return 0;
}

IMPL_MSG_HANDLE(doListSessions)
{
    return 0;
}

IMPL_MSG_HANDLE(doSendMessages)
{
    return 0;
}

IMPL_MSG_HANDLE(doUnknow)
{
    CXmlMsg lgmsg;
    lgmsg.create(xmlTagUnknowMessage);
    _resmsg.appendmsg(lgmsg);
    return 0;
}

IMPL_MSG_HANDLE(doCCNI)
{
    // LOGV("enter\n");
    CXmlMsg lgmsg, svrinfo;
    lgmsg.create(xmlTagCCNIRes);
    svrinfo.create(xmlTagSeriverInformation);

    svrinfo.addParameter(xmlTagServerType, "CCNIServer");
    svrinfo.addParameter(xmlTagServerVersion, "1.0");
    svrinfo.addParameter(xmlTagCCNIVersion, "1.0");
    svrinfo.addParameter(xmlTagDescription, "CCNI Chinese Chess server 1.0 for Linux");
    lgmsg.addChild(svrinfo);
    _resmsg.appendmsg(lgmsg);
    // LOGV("out\n");
    return 0;
}
IMPL_MSG_HANDLE(doMyState)
{
    CXmlMsg lgmsg;
    lgmsg.create(xmlTagMyStateRes);
    lgmsg.addParameter(xmlTagState, strstate());
    _resmsg.appendmsg(lgmsg);
    return 0;
}
IMPL_MSG_HANDLE(doLogout)
{
    //tbd:
    doreturn(xmlTagLogoutRes, 0, "logout success");
    return 0;
}
