/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             tcp_listener.h                                                          */
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

#ifndef TCP_LISTENER_H_
#define TCP_LISTENER_H_

#include <sys/types.h>
#include <sys/socket.h>

#include "log.h"
#include "config.h"
#include "thread.h"
#include "ccni.h"
#include "ccni_msg.h"
#include "threads_pool.h"
#include "seckey_map.h"

class CUdpSockData;
class CTcpListener : public CThread
{
public:
    class CTcpSockData
    {
public:
        bool islisenter;
        int fd;
        in_addr_t peerip;
        time_t stamp;
        CCNIMsgParser parser;
        CTcpSockData(bool is=true, int f=-1, in_addr_t ip=0) :
            islisenter(is), fd(f), peerip(ip), stamp(time(NULL))
        {
        }
    };
private:
    int _epfd;
    const CConfig & _cfg;
    CSecKeyMap & _smap;
    CThreadsPool & _pool;

    vector <CTcpSockData> _fds;
public:
    CTcpListener(const CConfig &cfg, CSecKeyMap & smap, CThreadsPool & pool) :
        _epfd(-1), _cfg(cfg), _smap(smap), _pool(pool)
    {

    }
    ~CTcpListener()
    {
        destroy();
    }

public:
    virtual void doWork();
    bool create();
    void destroy();
private:
    void _doaccept(CTcpSockData * sk);
    void _doread(CTcpSockData *sk);

    bool _epollAddSock(CTcpSockData *sk)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLPRI;
        ev.data.ptr = sk;
        if (epoll_ctl(_epfd, EPOLL_CTL_ADD, sk->fd, &ev) < 0)
        {

            LOGE("epoll ctrl add fd error: %s\n", strerror(errno));
            close(sk->fd);
            return false;
        }
        return true;
    }
    void _epollDelSock(CTcpSockData * sk)
    {
        struct epoll_event ev;
        if (epoll_ctl(_epfd, EPOLL_CTL_DEL, sk->fd, &ev) < 0)
        {
            LOGE("epoll ctrl del fd error: %s\n", strerror(errno));
        }
    }

private:
    class CTcpJob : public CJob
    {
private:
        friend class CTcpListener;
        CTcpSockData * _sk;
private:
      bool doLogin(CUdpSockData * udp, const struct sockaddr_in & udpaddr);
public:
        CTcpJob(CTcpSockData * sk, void * arg) :
            CJob(arg), _sk(sk)
        {
        }
        virtual bool run();
    };
};
#endif /*TCP_LISTENER_H_*/
