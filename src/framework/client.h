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
class CClient;
typedef std::tr1::shared_ptr<CClient> CClientPtr;
typedef std::tr1::weak_ptr<CClient> CClientWeakPtr;

class CClient : public CJob
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
        st_reading,
        st_sending,
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

private:
    bool doread();
    bool dosend();

public:
    CClient(int tcpfd, CUdpSockData * udp, const secret_key_t & k1, const secret_key_t & k2,
            const struct sockaddr_in & udpaddr, const CDataBase::CRecord & usr) :
        _state(Online), _tcpfd(tcpfd), _udp(udp), _k1(k1), _k2(k2), _udpaddr(udpaddr),
                _usrinfo(usr), _pstate(st_reading)
    {

    }
    virtual ~CClient()
    {
        close(_tcpfd);
        _curmsg.free();
        _resmsg.free();
    }

public:
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
public:
    virtual bool run();

};
class CClientTask : public CJob
{
private:
    CClientWeakPtr _cli;
public:
    CClientPtr lock()
    {
        return _cli.lock();
    }
    CClientTask(CClientWeakPtr cli) :
        _cli(cli)
    {

    }
    virtual bool run();
};

#endif /*CLINET_H_*/

