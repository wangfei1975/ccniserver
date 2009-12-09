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

    _pstate = st_sending;
    bd.destroy();
    return dosend();
}

bool CClient::dosend()
{
    CCNIMsgPacker::state_t st = _resmsg.send(_tcpfd);
    if (st == CCNIMsgPacker::st_sderror)
    {
        LOGW("send error.\n");
        return false;
    }
    if (st == CCNIMsgPacker::st_sdok)
    {
        LOGV("send ok.\n");
        _pstate = st_reading;
        _resmsg.free();
        _curmsg.free();
        return true;
    }
    LOGW("send eagain.\n");
    return true;
}
bool CClient::run()
{
    bool ret;
    if (_pstate == st_reading)
    {
        ret = doread();
    }
    else
    {
        ret = dosend();
    }
    if (ret)
    {
        LOGV("_psate = %s\n", _pstate == st_reading ? "reading":"sending");
        // LOGV("re assign ok.\n");
    }
    
    return ret;
}


//
// wo don't consider of multiple process of CClient::run() 
// since each time we just read one ccni message from client.
// after this message has been processed, then read next.
//
bool CClientTask::run()
{
    CClientPtr c = _cli.lock();
    if (c == NULL)
    {
        delete this;
        return false;
    }

    if (c->run())
    {
        int s = (c->pstate() == CClient::st_reading) ? 1 : 0;
        CEngine::instance().usrListener().assign(this, s);
        LOGV("run ok.\n")
        return true;
    }
    else
    {
       
        //tbd: broad cast user NotifyUserLogoff...
        CEngine::instance().dataMgr().delClient(c->uname());
        LOGI("client cnt %d\n",  CEngine::instance().dataMgr().userCount());
        LOGI("msg count %d times.\n", CEngine::instance().counter().msgCount());
        LOGI("thread pool status %d\n", CEngine::instance().threadsPool().getBusyCount());
        delete this;
        return false;
    }

    return false;
}
