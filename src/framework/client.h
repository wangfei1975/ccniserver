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
/*             client.h                                                                */
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

#ifndef CLINET_H_
#define CLINET_H_
#include <tr1/memory>
#include "log.h"
#include "ccni_msg.h"
#include "dbi.h"
#include "threads_pool.h"

/*
 *      
 * Offline 离线
 * Online 在线
 * Idle 在线，已进入房间
 * Sessional 在线，已进入房间，加入对局会话，空闲（等待对手或协商规则）
 * Ready 在线，已进入房间，已加入对局会话，已准备好
 * Watching 在线，已进入房间，已加入对局会话，观战
 * Moving 在线，已进入房间，已加入对局会话，已开始棋局，走棋
 * Pondering 在线，已进入房间，已加入对局会话，已开始棋局，等待对方走棋
 * */

class CUdpSockData;
class CBroadCaster;
class CClientTask;
class CClient;
typedef std::tr1::shared_ptr<CClient> CClientPtr;

class CClient
{
public:
    enum state_t
    {
        Offline = 0,
        Online = 0x01,
        Idle = 0x02,
        Sessional = 0x04,
        Ready = 0x08,
        Watching = 0x10,
        Moving = 0x20,
        Pondering = 0x40,
    };
    enum proc_state_t
    {
        st_idle,
        st_reading,
        st_sending,
        st_notifing,
    };
private:

    state_t _state;
    int _tcpfd;
    CUdpSockData * _udp;
    secret_key_t _k1, _k2;
    struct sockaddr_in _udpaddr;
    CDataBase::CRecord _usrinfo;

private:
    proc_state_t _pstate;
    CCNIMsgParser _curmsg;
    CCNIMsgPacker _resmsg;
    list<CCNINotification *> _notimsgs;
private:
    CClientTask * _ctsk;
    int _epfd;
private:
    bool doread();
    bool dosend();
    bool donotify();
public:
    CClient(int tcpfd, CUdpSockData * udp, const secret_key_t & k1, const secret_key_t & k2,
            const struct sockaddr_in & udpaddr, const CDataBase::CRecord & usr) :
        _state(Online), _tcpfd(tcpfd), _udp(udp), _k1(k1), _k2(k2), _udpaddr(udpaddr),
                _usrinfo(usr), _pstate(st_idle), _ctsk(NULL), _epfd(-1)
    {

    }
    virtual ~CClient()
    {
        close(_tcpfd);
        _curmsg.free();
        _resmsg.free();
        LOGI("client %s destroyed\n", uname());
    }

public:
    int epfd()
    {
        return _epfd;
    }

    bool addToEpoll(int epfd)
    {
        struct epoll_event ev;
        ev.events = epollflag();// | EPOLLET;
        ev.data.ptr = _ctsk;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, _tcpfd, &ev) < 0)
        {

            LOGW("epoll ctrl add fd error: %s\n", strerror(errno));
            return false;
        }
        _epfd = epfd;
        return true;
    }
    bool rmFromEpoll()
    {
        struct epoll_event ev;
        if (epoll_ctl(_epfd, EPOLL_CTL_DEL, _tcpfd, &ev) < 0)
        {
            LOGW("epoll ctrl del fd error: %s\n", strerror(errno));
            return false;
        }
        _epfd = -1;
        return true;
    }
    void setClientTask(CClientTask * t)
    {
        _ctsk = t;
    }
    uint32_t epollflag()
    {
        if (_pstate == st_idle || _pstate == st_reading)
        {
            return EPOLLIN | EPOLLPRI;
        }
        else
        {
            return EPOLLOUT;
        }
    }
    proc_state_t pstate()
    {
        return _pstate;
    }
    int tcpfd()
    {
        return _tcpfd;
    }
    const char * uname()
    {
        return _usrinfo.name.c_str();
    }
    CUdpSockData * udpsock()
    {
        return _udp;
    }
    struct sockaddr_in & udpaddr()
    {
        return _udpaddr;
    }
    const char * strpstate()
    {
        const char * spstate[] =
        { "idle", "reading", "sending", "notifing" };
        return spstate[_pstate];
    }
    const char * strstate()
    {
        static const struct
        {
            state_t msk;
            const char * name;
        } tab[]=
        {
        { Offline, "Offline" },
        { Online, "Online" },
        { Idle, "Idle" },
        { Sessional, "Sessional" },
        { Ready, "Ready" },
        { Watching, "Watching" },
        { Moving, "Moving" },
        { Pondering, "Pondering" },
        { Offline, "Unknow state" } };
        int i;
        for (i = 0; i < (int)(sizeof(tab)/sizeof(tab[0]))-1; i++)
        {
            if (_state == tab[i].msk)
                break;
        }
        return tab[i].name;
    }

public:
#include "ccni_msghnds.h"
private:
    // 
    // for sync doincoming  message and send notification.
    CMutex _lk;
public:
    bool doIncomingMsg();
    bool queueNotification(CNotifyMsgBufPtr msg);
};
class CClientTask : public CJob
{
private:
    CClientPtr _cli;

public:
    ~CClientTask()
    {
        LOGI("client task %s destroyed.\n", _cli->uname());
    }
    CClientTask(CClientPtr cli) :
        _cli(cli)
    {
        cli->setClientTask(this);
    }
    //
    // wo don't consider of multiple process of CClient::run() 
    // since each time we just read one ccni message from client.
    // after this message has been processed, then read next.
    //
    virtual bool run()
    {
        return _cli->doIncomingMsg();
    }
    CClientPtr client()
    {
        return _cli;
    }

};

#endif /*CLINET_H_*/

