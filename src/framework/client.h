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
#include "log.h"
#include "ccni_msg.h"
#include "dbi.h"
#include "threads_pool.h"

/*
 *      Offline 离线
 Online 在线
 Idle 在线，已进入房间
 Sessional 在线，已进入房间，加入对局会话，空闲（等待对手或协商规则）
 Ready 在线，已进入房间，已加入对局会话，已准备好
 Watching 在线，已进入房间，已加入对局会话，观战
 Moving 在线，已进入房间，已加入对局会话，已开始棋局，走棋
 Pondering 在线，已进入房间，已加入对局会话，已开始棋局，等待对方走棋
 * */

class CUdpSockData;
class CClient:public CJob
{
public:
    enum state_t
    {
        Offline,
        Online,
        Idle,
        Sessional,
        Ready,
        Watching,
        Moving,
        Pondering,
    };
private:
    state_t _state;
    int _tcpfd;
    CUdpSockData * _udp;
    secret_key_t _k1, _k2;
    struct sockaddr_in _udpaddr;
    CDataBase::CRecord _usrinfo;
    
private:
    CCNIMsgParser  _curmsg;
    
public:
    CClient(int tcpfd, CUdpSockData * udp, const secret_key_t & k1, const secret_key_t & k2, const struct sockaddr_in & udpaddr,
            const CDataBase::CRecord & usr) :
        _state(Online), _tcpfd(tcpfd), _udp(udp), _k1(k1), _k2(k2), _udpaddr(udpaddr), _usrinfo(usr)
    {

    }
    
public:
    int tcpfd()
    {
        return _tcpfd;
    }

    
public:
  
public:
    virtual bool run();
   
};
#endif /*CLINET_H_*/
