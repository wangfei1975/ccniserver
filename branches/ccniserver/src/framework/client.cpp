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
/*             client.cpp                                                              */
/*                                                                                     */
/*  version                                                                            */
/*             1.0                                   h                                    */
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

void CClient::destroy()
{
    close(_tcpfd);
    _curmsg.free();
    _resmsg.free();
    list<CCNINotification *>::iterator it;
    for (it = _notimsgs.begin(); it != _notimsgs.end(); ++it)
    {
        delete (*it);
    }
    _notimsgs.clear();
    if (_bder)
    {
        delete _bder;
        _bder = NULL;
    }
    if (_notifier)
    {
        delete _notifier;
        _notifier = NULL;
    }
    LOGI("client %s destroyed\n", uname());
}
bool CClient::doread(CNotifier & nt, CBroadCaster & bd)
{
    _pstate = st_reading;
    CCNIMsgParser::state_t st = _curmsg.read(_tcpfd);
    LOGV("read state: %d\n", st);
    if (st == CCNIMsgParser::st_rderror)
    {
        LOGW("remote disconnected. %s\n", inet_ntoa(_udpaddr.sin_addr));
        return false;
    }

    if (st == CCNIMsgParser::st_hderror)
    {
        LOGW("read a invalid ccni header from %s,force close it.\n", inet_ntoa(_udpaddr.sin_addr));
        return false;
    }
    if (st == CCNIMsgParser::st_rdbd || CCNIMsgParser::st_rdhd)
    {
        //just waiting for next read.
        _pstate = st_reading;
        return true;
    }

    if (!_curmsg.parse())
    {
        LOGW("read a invalid ccni msg from %s,force close it.\n", inet_ntoa(_udpaddr.sin_addr));
        return false;
    }
    uint32_t c = CEngine::instance().counter().incMsgCnt();
    if ((c %5000) == 0)
    {
        LOGW("total message count:%d\n", c);
    }

    LOGV("got a ccni msg:\n%s\n", _curmsg.data());
    _resmsg.create();

    procMsgs(_curmsg);
    return dosend();
}

bool CClient::dosend()
{
    _pstate = st_sending;
    CCNIMsgPacker::state_t st = _resmsg.send(_tcpfd);
    if (st == CCNIMsgPacker::st_sderror)
    {
        LOGW("send error.\n");
        return false;
    }
    if (st == CCNIMsgPacker::st_sdok)
    {
        LOGV("send ok.\n");
        _resmsg.free();
        _curmsg.free();
        return donotify();
    }
    LOGW("send eagain.\n");
    return true;
}
bool CClient::donotify()
{
    if (_notimsgs.empty())
    {
        LOGV("empty notify message queue.");
        _pstate = st_idle;
        //
        //we don't return false,since we want next reading.
        //
        return true;
    }
    _pstate = st_notifing;
    CCNINotification * inmsg = *_notimsgs.begin();
    int st = inmsg->send(_tcpfd);
    if (st < 0)
    {
        LOGW("notification send error.\n");
        delete inmsg;
        _notimsgs.erase(_notimsgs.begin());
        return false;
    }
    if (st == 0)
    {
        _notimsgs.erase(_notimsgs.begin());
        delete inmsg;
        if (_notimsgs.empty())
        {
            _pstate = st_idle;
        }
        // else send next notification.
        return true;
    }
    //send notify eagain.
    return true;
}

// doWork was trigered by epoll event.
//
bool CClient::doWork()
{
    bool ret = false;
    CClientPtr cli;
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

    CAutoMutex du(_iolk);
    switch (_pstate)
    {
    case st_idle:
    case st_reading:
        ret = doread(*_notifier, *_bder);
        break;
    case st_sending:
        ret = dosend();
        break;
    case st_notifing:
        ret = donotify();
        break;
    default:LOGW("this is impossibile. : )\n")
        ;
        break;
    }

    if (ret)
    {
        CEngine::instance().usrListener().assign(_ctsk);
    }
    else
    {
        //io error, we take it as break line case.
        //hold a strong reference to this.
        cli = _ctsk->client();
        CDataMgr & dmgr = CEngine::instance().dataMgr();
        leaveRoom();

        dmgr.delClient(uname());
        _state = Offline;
        LOGI("client cnt %d\n", dmgr.userCount());
        LOGV("msg count %d times.\n", CEngine::instance().counter().msgCount());
        //  LOGI("thread pool status %d\n", CEngine::instance().threadsPool().getBusyCount());
        delete _ctsk;
        _ctsk = NULL;
    }

    LOGV("_psate = %s\n", strpstate());
    // in case of this processing generate some broad cast message.
    // we if broad cast msg is not empty, we assign it to threads poll.
    // if is empty, we don't need delete it, we can use it in next ccni msg.
    if (!_bder->empty())
    {
        CEngine::instance().threadsPool().assign(_bder);
        _bder = NULL;
    }

    // in case of this processing generate some notification to other client.
    if (!_notifier->empty())
    {
        CEngine::instance().threadsPool().assign(_notifier);
        _notifier = NULL;
    }

    return ret;
}

bool CClient::queueNotification(CNotifyMsgBufPtr msg)
{
    CAutoMutex du(_iolk);

    _notimsgs.push_back(new CCNINotification(msg));
    //if we are in epoll waiting list, then remove it and  post a send request 
    if (_pstate == st_idle)
    {
        rmFromEpoll();
        donotify();
        //
        // we don't care donotify's return value.
        // even donotify returns a error, we still assign to epoll list to 
        // catch the break line event. 
        //
        CEngine::instance().usrListener().assign(_ctsk);
    }
    return true;
}

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
    _state = Online;
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
    if (_state != Online || _ctsk == NULL)
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
    _state = Idle;
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
