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

bool CClient::doread()
{
    _pstate = iostReading;
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
        _pstate = iostReading;
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
        LOGI("total message count:%d\n", c);
    }

    LOGV("got a ccni msg:\n%s\n", _curmsg.data());
    _resmsg.create();

    procMsgs(_curmsg);
    return dosend();
}

bool CClient::dosend()
{
    _pstate = iostSending;
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
        LOGV("empty notify message queue.\n");
        _pstate = iostIdle;
        //
        //we don't return false,since we want next reading.
        //
        return true;
    }
    _pstate = iostNotifing;
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
            _pstate = iostIdle;
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
    CAutoMutex du(_iolk);
    switch (_pstate)
    {
    case iostIdle:
    case iostReading:
        ret = doread();
        break;
    case iostSending:
        ret = dosend();
        break;
    case iostNotifing:
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
        //io error, we treat it as break line.
        //hold a strong reference to this.
        cli = _ctsk->client();
         
        if (_sessionid > 0)
        {
            genBdsAndNotis();
            leaveSession();
            consumeBdsAndNotis();
        }
        
        if (_roomid > 0)
        {
            genBdsAndNotis();
            leaveRoom();
            consumeBdsAndNotis();
        }
        CDataMgr & dmgr = CEngine::instance().dataMgr();
        dmgr.delClient(uname());
        LOGV("delete from dmgr ok.\n")
        _state = stOffline;
        delete _ctsk;
        LOGV("delete _ctsk ok.\n")
        _ctsk = NULL;

        LOGI("client cnt %d\n", dmgr.userCount());
        LOGV("msg count %d times.\n", CEngine::instance().counter().msgCount());
        //  LOGI("thread pool status %d\n", CEngine::instance().threadsPool().getBusyCount());
    }

    
    LOGV("_psate = %s\n", striostate());

    return ret;
}

bool CClient::queueNotification(CFlatMsgBufPtr msg)
{
    CAutoMutex du(_iolk);

    _notimsgs.push_back(new CCNINotification(msg));
    //if we are in epoll waiting list, then remove it and  post a send request 
    if (_pstate == iostIdle)
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

