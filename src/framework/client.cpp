/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             client.cpp                                                              */
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

bool CClient::run()
{
    LOGI("user job run.\n");
/*
    CCNIMsgParser::parse_state_t st = _curmsg.read(_tcpfd);
    if (st == CCNIMsgParser::st_rderror)
    {
        LOGW("remote disconnected. %s\n", inet_ntoa(_udpaddr.sin_addr));
        CEngine::instance().dataMgr().delClient(this);
        //tbd: broad cast user NotifyUserLogoff...
        close(_tcpfd);
        delete this;
        return false;
    }
    else if (st == CCNIMsgParser::st_hderror)
    {
        LOGW("read a invalid ccni header from %s,force close it.\n", inet_ntoa(_udpaddr.sin_addr));
        CEngine::instance().dataMgr().delClient(this);
        close(_tcpfd);
        delete this;
        return false;
    }
    else if (st == CCNIMsgParser::st_bdok)
    {
        if (!_curmsg.parse())
        {
            LOGW("read a invalid ccni msg from %s,force close it.\n", inet_ntoa(_udpaddr.sin_addr));
            CEngine::instance().dataMgr().delClient(this);
            close(_tcpfd);
            delete this;
            return false;
        }
        LOGI("got a ccni msg:\n%s\n", _curmsg.data());
        _curmsg.free();
    }
 */
   // CEngine::instance().usrListener().assign(this, 0);
    return true;
}

