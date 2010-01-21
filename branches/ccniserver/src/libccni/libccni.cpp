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
/*             libccni.cpp                                                             */
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
/*             2010-01-13     initial draft                                            */
/***************************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "libccni.h"

#include "log.h"
#include "ccni.h"
#include "ccni_msg.h"
/*
 const char * copyright = "\n"
 "CCNI client library 1.0.\n"
 "Copyright (C) bjwf 2008-2016. All rights reserved.\n"
 "bjwf2000@gmail.com  11-20-2008\n"
 "http://ccniserver.googlecode.com\n\n";
 */
bool CCNIConnection::connect(const char * serverip, int tport, int uport)
{
    CCNI_HEADER hd;
    struct sockaddr_in uad;

    memset(&uad, 0, sizeof(uad));
    uad.sin_family = AF_INET;
    uad.sin_port = htons(uport);
    uad.sin_addr.s_addr = inet_addr(serverip);
    LOGV("server ip:port is %s:%d\n", inet_ntoa(uad.sin_addr), ntohs(uad.sin_port));

    _udpsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sendto(_udpsock, &hd, sizeof(hd), 0, (struct sockaddr *)&uad, sizeof(uad)) != sizeof(hd))
    {
        _lasterr = "send error:" + string(strerror(errno));
        LOGE("%s\n", _lasterr.c_str());
        disconnect();
        return false;
    }
    socklen_t rlen = (socklen_t)sizeof(uad);
    if (recvfrom(_udpsock, &hd, sizeof(hd), 0, (struct sockaddr *)&uad, &rlen) != sizeof(hd))
    {
        _lasterr = "receive udp error";
        LOGE("%s\n", _lasterr.c_str());
        disconnect();
        return false;
    }

    _k1 = hd.secret1;
    _k2 = hd.secret2;
    _tcpsock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in tcpraddr;
    memset(&tcpraddr, 0, sizeof(tcpraddr));
    tcpraddr.sin_family = AF_INET;
    tcpraddr.sin_addr.s_addr = inet_addr(serverip);
    tcpraddr.sin_port = htons(tport);

    if (::connect(_tcpsock, (struct sockaddr *)&tcpraddr, sizeof(tcpraddr)) < 0)
    {
        _lasterr = "TCP connect error:" + string(strerror(errno));
        LOGE("%s\n", _lasterr.c_str());
        disconnect();
        return false;
    }

    if (!create())
    {
        _lasterr ="create listener thread error.";
        LOGE("%s\n", _lasterr.c_str());
        disconnect();
        return false;
    }
    _connected = true;
    return true;
}

void CCNIConnection::disconnect()
{
    if (_connected)
    {
        LOGD("disconnect.\n")
        if (_tcpsock >= 0)
        {
            close(_tcpsock);
            _tcpsock = -1;
        }
        if (_udpsock >= 0)
        {
            close(_udpsock);
            _udpsock = -1;
        }

        event_lst_t::iterator it;
        for (it = _evts.begin(); it != _evts.end(); ++it)
        {
            delete (it->second);
        }

        _evts.clear();
        _connected = false;
    }
}
#define PrepareCCNIMsg(tag)  uint32_t seq = (uint32_t)pthread_self(); \
        CCNIMsgPacker msg; msg.create(); \
        CCMsgQueue * que = getMsgQue(seq); \
        CXmlMsg lgmsg; \
        lgmsg.create(tag); msg.appendmsg(lgmsg);

#define SendMsgAndWaitRes(tag) msg.pack(seq, 0, _k1, _k2); \
       if (msg.block_send(_tcpsock) < 0) \
       { \
         _lasterr = "send error";LOGE("%s\n", _lasterr.c_str());\
         return -1;\
       }  \
       CCNIMsgParser * rmsg = (CCNIMsgParser *)que->receive(_timeout);\
       if (rmsg == NULL)\
       {\
         _lasterr = "receive error";LOGE("%s\n", _lasterr.c_str());\
         return -1;\
       } \
       CXmlMsg rgmsg; rgmsg.attach(rmsg->getmsg(tag)); \
       if (rgmsg.isEmpty()) \
       {\
           _lasterr = "received error msg"; LOGE("%s\n", _lasterr.c_str());\
           delete rmsg; \
           return -2; \
       }\
       int rcode = rgmsg.findChild(xmlTagReturnCode).getIntContent(); \
       if (rcode != 0) \
       { \
           string v; \
           rgmsg.findChild(xmlTagDescription).getContent(v); \
           LOGE("%s error: %s\n", tag, v.c_str()); \
           _lasterr = v; \
           delete rmsg; \
           return rcode - 2; \
       }

#define SuccessReturn(tag) {LOGD("%s success\n", tag); delete rmsg; return 0;}

#define SendMsgAndWaitResAndReturn(tag) {SendMsgAndWaitRes(tag) SuccessReturn(tag)}

#define DoSimpleRequest(tag, tagres) {PrepareCCNIMsg(tag) SendMsgAndWaitResAndReturn(tagres)}

int CCNIConnection::login(const char * uname, const char * pwd)
{
    PrepareCCNIMsg(xmlTagLogin)
    lgmsg.addParameter(xmlTagUserName, uname);
    lgmsg.addParameter(xmlTagPassword, pwd);
    SendMsgAndWaitResAndReturn(xmlTagLoginRes)
}
int CCNIConnection::logout()
{
    PrepareCCNIMsg(xmlTagLogout)

    lgmsg.addParameter(xmlTagUserName, _uname.c_str());
    lgmsg.addParameter(xmlTagPassword, _passwd.c_str());

    SendMsgAndWaitResAndReturn(xmlTagLogoutRes)

}
int CCNIConnection::ccni(string & ccnistr)
{
    PrepareCCNIMsg(xmlTagCCNI)
    SendMsgAndWaitRes(xmlTagCCNIRes)
    rgmsg.toString(ccnistr);
    LOGD("ccni:\n%s\n", ccnistr.c_str());
    SuccessReturn(xmlTagCCNIRes)
}

int CCNIConnection::state()
{
    PrepareCCNIMsg(xmlTagMyState)
    SendMsgAndWaitRes(xmlTagMyStateRes)

    string v;
    LOGD("return msg:\n%s\n", rgmsg.toString(v).c_str());
    rgmsg.findChild(xmlTagState).getContent(v);

    static const struct
    {
        state_t msk;
        const char * name;
    } tab[]=
    {
    { stOffline, "Offline" },
    { stOnline, "Online" },
    { stIdle, "Idle" },
    { stSessional, "Sessional" },
    { stReady, "Ready" },
    { stWatching, "Watching" },
    { stMoving, "Moving" },
    { stPondering, "Pondering" },
    { stOffline, "Unknow state" } };
    int i;
    for (i = 0; i < (int)(sizeof(tab)/sizeof(tab[0]))-1; i++)
    {
        if (v == tab[i].name)
        {
            LOGD("current state: %s\n", tab[i].name);
            break;
        }
    }

    delete rmsg;
    return (_state = tab[i].msk);

}
int CCNIConnection::listrooms(room_lst_t & lst)
{
    PrepareCCNIMsg(xmlTagListRooms)
    SendMsgAndWaitRes(xmlTagListRoomsRes)
    lst.clear();
    CXmlNode rooms = rgmsg.findChild(xmlTagRooms);

    for (CXmlNode nd = rooms.child() ; !nd.isEmpty(); nd = nd.next())
    {
        if (nd.type() != XML_ELEMENT_NODE)
        {
            continue;
        }
        if (strcmp(nd.name(), xmlTagRoom) == 0)
        {
            CRoom r;
            r.id = nd.findChild(xmlTagId).getIntContent();
            r.capacity = nd.findChild(xmlTagCapacity).getIntContent();
            r.usrcount = nd.findChild(xmlTagUserCount).getIntContent();
            nd.findChild(xmlTagDescription).getContent(r.des);
            LOGV("%s\n", nd.toString().c_str());
            LOGD("Room: %d Capacity=%d UserCount=%d %s\n", r.id, r.capacity, r.usrcount, r.des.c_str());
            lst.push_back(r);
        }
    }
    delete rmsg;
    return lst.size();
}

int CCNIConnection::enterroom(int roomid)
{
    PrepareCCNIMsg(xmlTagEnterRoom)
    lgmsg.addParameter(xmlTagId, roomid);
    SendMsgAndWaitResAndReturn(xmlTagEnterRoomRes)
}
int CCNIConnection::leaveroom()
{
    DoSimpleRequest(xmlTagLeaveRoom, xmlTagLeaveRoomRes)
}

int CCNIConnection::listsessions()
{
    DoSimpleRequest(xmlTagListSessions, xmlTagListSessionsRes)
}
int CCNIConnection::newsession()
{
    DoSimpleRequest(xmlTagNewSession, xmlTagNewSessionRes)
}
int CCNIConnection::entersession(int sessionid)
{
    PrepareCCNIMsg(xmlTagEnterSession)
    lgmsg.addParameter(xmlTagId, sessionid);
    SendMsgAndWaitResAndReturn(xmlTagEnterSessionRes)

}
int CCNIConnection::leavesession()
{
    DoSimpleRequest(xmlTagLeaveSession, xmlTagLeaveSessionRes)
}
int CCNIConnection::watchsession(int sessionid)
{
    PrepareCCNIMsg(xmlTagWatchSession)
    lgmsg.addParameter(xmlTagId, sessionid);
    SendMsgAndWaitResAndReturn(xmlTagWatchSessionRes)
}

CCMsgQueue * CCNIConnection::getMsgQue(uint32_t seq)
{
    _evtlk.lock();
    event_lst_t::iterator it = _evts.find(seq);
    if (it == _evts.end())
    {
        _evts[seq] = new CCMsgQueue;
    }
    _evtlk.unlock();
    return _evts[seq];

}
bool CCNIConnection::sendRes(uint32_t seq, CCNIMsgParser * res)
{
    _evtlk.lock();
    event_lst_t::iterator it = _evts.find(seq);
    if (it == _evts.end())
    {
        _evtlk.unlock();
        return false;
    }
    _evtlk.unlock();
    (it->second)->send(res);

    return true;
}
int CCNIConnection::doServerMessage(int sock)
{
    // LOGV("read server mesage\n");
    CCNIMsgParser * pmsg = new CCNIMsgParser;
    CCNIMsgParser::state_t st;

    st = pmsg->read(sock);

    while (st == CCNIMsgParser::st_rdhd || st == CCNIMsgParser::st_rdbd)
    {
        LOGD("read eagain\n");
        st = pmsg->read(sock);
    }
    if (st != CCNIMsgParser::st_rdok)
    {
        LOGE("read error discard %d.\n", st);
        delete pmsg;
        return -1;
    }
    if (!pmsg->parse())
    {
        LOGE("got a error message. discard.\n");
        delete pmsg;
        return -1;
    }
    LOGD("seq = 0x%X\n", pmsg->header().seq);
    LOGV("got a server msg:\n%s\n", pmsg->data());
    if (pmsg->header().seq != 0)
    {
        if (!sendRes(pmsg->header().seq, pmsg))
        {
            LOGE("msg queue error.\n");
            delete pmsg;
        }
    }
    else
    {
        delete pmsg;
    }
    return 0;
}

void CCNIConnection::doWork()
{
    fd_set rfds;
    struct timeval tv;
    int ret = 0;
    LOGV("select udp:%d, tcp:%d\n", _udpsock, _tcpsock);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    while (ret >= 0)
    {
        FD_ZERO(&rfds);
        FD_SET(_udpsock, &rfds);
        FD_SET(_tcpsock, &rfds);
        int n = _udpsock> _tcpsock ? _udpsock : _tcpsock;
        //LOGV("select udp:%d, tcp:%d\n", _udpsock, _tcpsock);
        if (select(n+1, &rfds, NULL, NULL, &tv) == -1)
        {
            if (errno == EBADF)
            {
                LOGD("socket closed, exit.\n");
                ret = -1;
            }
            LOGE("select error.\n");
        }
        else if (FD_ISSET(_udpsock, &rfds))
        {
            LOGV("udp sock has something.\n");
            ret = doServerMessage(_udpsock);
        }
        else if (FD_ISSET(_tcpsock, &rfds))
        {
            LOGV("tcp sock has something.\n");
            if ((ret = doServerMessage(_tcpsock)) < 0)
            {
                LOGW("connection closed.");
            }
        }
        else
        {
            // LOGV("select time out, continue.\n");
            ret = 0;
        }
    }
    disconnect();
}
