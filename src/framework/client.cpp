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

 

bool CClient::run()
{
    static int cnt = 0, disccnt = 0, errcnt = 0;
    static int msgcnt = 0;
    if (++cnt == 1)
    {
        LOGW("user job run %d times.\n",  cnt);
    }
  
    CCNIMsgParser::parse_state_t st = _curmsg.read(_tcpfd);
    LOGV("read state: %d\n", st);
    if (st == CCNIMsgParser::st_rderror)
    {
        
        LOGW("remote disconnected. %s %d\n", inet_ntoa(_udpaddr.sin_addr), ++disccnt);
        LOGW("user job run %d times.\n",  cnt);
        LOGW("msg count %d times.\n",  msgcnt);
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
        LOGW("read error cnt:%d\n", ++errcnt);
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
        ++msgcnt;
        LOGV("got a ccni msg:\n%s\n", _curmsg.data());
       // procMsgs(_curmsg);
        _curmsg.free();
    }
 
    CEngine::instance().usrListener().assign(this, 0);
    return true;
}

