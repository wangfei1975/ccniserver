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
            CEngine::instance().threadsPool().assign(_bder);
            _bder = NULL;
        }

        // in case of this processing generate some notification to other clients.
        if (!_notifier->empty())
        {
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
                                     LOGV(" out %d\n", rcode);  \
                                     return rcode;}

void CClient::leaveRoom()
{
    CDataMgr & dmgr = CEngine::instance().dataMgr();
    if (_roomid <= 0)
    {
        return;
    }
    CRoomPtr rm = dmgr.findRoom(_roomid);

    if (rm != NULL)
    {
        if (!rm->leave(_ctsk->client(), *_bder))
        {
            LOGE("leave room error. bugs here.\n")
        }
    }
    else
    {
        LOGE("error, leave room bugs here.\n")
    }
    _state = stOnline;
    _roomid = -1;

    CXmlMsg bdmsg;
    if (!_bder->empty())
    {
        bdmsg.create(xmlTagBroadcastEnterRoom);
        bdmsg.addParameter(xmlTagUserName, uname());
        _bder->append(bdmsg);
    }

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
        CXmlMsg bdmsg;
        bdmsg.create(xmlTagBroadcastEnterRoom);
        bdmsg.addParameter(xmlTagUserName, uname());
        _bder->append(bdmsg);
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

    if (_roomid < 0)
    {
        doreturn(xmlTagLeaveRoomRes, -1, "user state error");
    }

    if (_roomid> 0)
    {
        leaveRoom();
    }

    doreturn(xmlTagLeaveRoomRes, 0, "leave room success");
}

IMPL_MSG_HANDLE(doNewSession)
{
    return 0;
}

IMPL_MSG_HANDLE(doEnterSession)
{
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
