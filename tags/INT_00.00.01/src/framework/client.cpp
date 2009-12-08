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
        LOGW("msg count %d times.\n", CEngine::instance().counter().msgCount());
        LOGW("thread pool status %d\n", CEngine::instance().threadsPool().getBusyCount());

        //tbd: broad cast user NotifyUserLogoff...

        return false;
    }

    if (st == CCNIMsgParser::st_hderror)
    {
        LOGW("read a invalid ccni header from %s,force close it.\n", inet_ntoa(_udpaddr.sin_addr));

        //  LOGW("read error cnt:%d\n", ++errcnt);

        return false;
    }
    if (st == CCNIMsgParser::st_rdbd || CCNIMsgParser::st_rdhd)
    {
        //just waiting for next read.
        return true;
    }

    // if (st == CCNIMsgParser::st_bdok) //got a complete message
    if (!_curmsg.parse())
    {
        LOGW("read a invalid ccni msg from %s,force close it.\n", inet_ntoa(_udpaddr.sin_addr));
        return false;
    }
    uint32_t c = CEngine::instance().counter().incMsgCnt();
    if ((c %1000) == 0)
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
    /*
     _resmsg.block_send(_tcpfd);
     _resmsg.free();
     _curmsg.free();
     _pstate = st_reading;
     return true;
     */
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
    //static volatile int cnt = 0;

    //if (++cnt == 1)
    {
        // LOGV("user job run %d times.\n", ++cnt);
    }
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
    else
    {
        _resmsg.free();
        _curmsg.free();
        close(_tcpfd);
    }
    return ret;
}
bool CClientTask::run()
{
    CClientPtr c = _cli.lock();
    if (c == NULL)
    {
        return false;
    }

    if (c->run())
    {
        int s = (c->pstate() == CClient::st_reading) ? 1 : 0;
        CEngine::instance().usrListener().assign(this, s);
        LOGI("run ok.\n")
        return true;
    }
    else
    {
        CEngine::instance().dataMgr().delClient(c->uname());
        LOGI("client cnt %d\n",  CEngine::instance().dataMgr().userCount());
        delete this;
        return false;
    }

    return false;
}
