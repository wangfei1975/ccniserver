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

bool CClient::doread()
{
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
    CBroadCaster bd;
    _resmsg.create();
    bd.create();
    procMsgs(_curmsg, _resmsg, bd);

    bd.destroy();
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
bool CClient::doIncomingMsg()
{
    bool ret = false;
    CAutoMutex du(_lk);
    switch (_pstate)
    {
    case st_idle:
    case st_reading:
        ret = doread();
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
        //tbd: broad cast user NotifyUserLogoff...
        CEngine::instance().dataMgr().delClient(uname());
        LOGI("client cnt %d\n", CEngine::instance().dataMgr().userCount());
        LOGI("msg count %d times.\n", CEngine::instance().counter().msgCount());
        LOGI("thread pool status %d\n", CEngine::instance().threadsPool().getBusyCount());
        delete _ctsk;
    }
    LOGV("_psate = %s\n", strpstate());
    return ret;
}

bool CClient::queueNotification(CNotifyMsgBufPtr msg)
{
    CAutoMutex du(_lk);

    _notimsgs.push_back(new CCNINotification(msg));
    //if we are in epoll waiting list, then remove it and then post a send request 
    if (_pstate == st_idle)
    {
        _pstate = st_notifing;
        rmFromEpoll();
        //post a new task.
        CEngine::instance().usrListener().assign(_ctsk);
    }
    return true;
}

