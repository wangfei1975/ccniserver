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
class CNotifier;
class CClient;

typedef std::tr1::shared_ptr<CClient> CClientPtr;

/*
 *  Client socket IO state transition design idea: 
 *   . After creating, io state set to st_idle. and tcp socket added to Epoll as read triger.
 *   . once triged by incoming ccni msg, it transite to st_reading state, the state kept until a complete
 *     ccni msg received and finished the message processing. 
 *     this may include multiple epoll triger event.
 *   . When client start send response, the state transite to st_sending 
 *   . when send response complete, client check whether there are some notifications need to process,
 *     if so process it, else transite to st_idle.
 *   . transite to st_notifing until all notifications are processed.
 *     
 * 
 * 
 * */

class CClient
{
public:
    class CClientTask;
    enum state_t
    {
        stOffline = 0,
        stOnline = 0x01,
        stIdle = 0x02,
        stSessional = 0x04,
        stReady = 0x08,
        stWatching = 0x10,
        stMoving = 0x20,
        stPondering = 0x40,
        
        //combined state
        stConnected = stOnline|stIdle|stSessional|stReady|stWatching|stMoving|stPondering,
        stInRoom    = stIdle|stSessional|stReady|stWatching|stMoving|stPondering,
        stInSession = stSessional|stReady|stWatching|stMoving|stPondering,
        stPlaying   = stMoving|stPondering,
        
    };
    enum io_state_t
    {
        iostIdle,
        iostReading,
        iostSending,
        iostNotifing,
    };
private:
    state_t _state;
    int _tcpfd;
    CUdpSockData * _udp;
    secret_key_t _k1, _k2;
    struct sockaddr_in _udpaddr;
    CDataBase::CRecord _usrinfo;

private:
    //current room id, if in room.
    int _roomid;
    int enterRoom(int rid);
    void leaveRoom();
    

private:
    io_state_t _pstate;
    // for sync io process.
    CMutex _iolk;

    CCNIMsgParser _curmsg; //current client request
    CCNIMsgPacker _resmsg; //response for current request 
    CBroadCaster * _bder; //broadcast msgs which caused by current client request
    CNotifier * _notifier; //notifications which caused by current client request

    list<CCNINotification *> _notimsgs; //notifications to this client.

private:
    bool doread();
    bool dosend();
    bool donotify();
    
    
public:
    CClient(int tcpfd, CUdpSockData * udp, const secret_key_t & k1, const secret_key_t & k2,
            const struct sockaddr_in & udpaddr, const CDataBase::CRecord & usr) :
        _state(stOnline), _tcpfd(tcpfd), _udp(udp), _k1(k1), _k2(k2), _udpaddr(udpaddr),
                _usrinfo(usr), _roomid(-1), _pstate(iostIdle), _bder(NULL), _notifier(NULL), _ctsk(NULL),
                _epfd(-1)
    {

    }
    virtual ~CClient()
    {
        destroy();
    }

public:
    void destroy();
    bool addToEpoll(int epfd)
    {
        struct epoll_event ev;
        ev.data.ptr = _ctsk;
        if (_pstate == iostIdle || _pstate == iostReading)
        {
            ev.events = EPOLLIN | EPOLLPRI;
        }
        else
        {
            ev.events = EPOLLOUT;
        }

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

    io_state_t pstate()
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
    const char * striostate()
    {
        const char * iostate[] =
        { "idle", "reading", "sending", "notifing" };
        return iostate[_pstate];
    }
    const char * strstate()
    {
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
            if (_state == tab[i].msk)
                break;
        }
        return tab[i].name;
    }

public:
#include "ccni_msghnds.h"
private:

public:
    bool doWork();
    bool queueNotification(CFlatMsgBufPtr msg);

public:

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
            cli->_ctsk = this;
        }
        //
        // wo don't consider of multiple process of CClient::run() 
        // since each time we just read one ccni message from client.
        // after this message has been processed, then read next.
        //
        virtual bool run()
        {
            return _cli->doWork();
        }
        CClientPtr client()
        {
            return _cli;
        }

    };

private:
    friend class CClientTask;
    CClientTask * _ctsk;
    int _epfd;
};

#endif /*CLINET_H_*/

