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

bool CClient::doread()
{
    static volatile int disccnt = 0, errcnt = 0;
    static volatile int msgcnt = 0;
    CCNIMsgParser::state_t st = _curmsg.read(_tcpfd);
    LOGV("read state: %d\n", st);
    if (st == CCNIMsgParser::st_rderror)
    {

        LOGW("remote disconnected. %s %d\n", inet_ntoa(_udpaddr.sin_addr), ++disccnt);
        LOGW("msg count %d times.\n", msgcnt);

        //tbd: broad cast user NotifyUserLogoff...

        return false;
    }

    if (st == CCNIMsgParser::st_hderror)
    {
        LOGW("read a invalid ccni header from %s,force close it.\n", inet_ntoa(_udpaddr.sin_addr));

        LOGW("read error cnt:%d\n", ++errcnt);

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
    ++msgcnt;
    _pstate = st_sending;
    LOGV("got a ccni msg:\n%s\n", _curmsg.data());
    CCNIMsgPacker bd;

    procMsgs(_curmsg, _resmsg, bd);

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
    return true;
}
bool CClient::run()
{
    static volatile int cnt = 0;
    if (++cnt == 1)
    {
        LOGW("user job run %d times.\n", cnt);
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
        CEngine::instance().usrListener().assign(this, (_pstate == st_reading) ? 1 : 0);
    }
    else
    {
        CEngine::instance().dataMgr().delClient(this);
        _resmsg.free();
        _curmsg.free();
        close(_tcpfd);
        delete this;
    }
    return ret;
}

