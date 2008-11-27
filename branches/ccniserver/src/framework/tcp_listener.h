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

class CTcpListener : public CThread
{
public:
    class CTcpSockData
    {
public:
        bool islisenter;
        int fd;
        CCNIMsgParser * parser;
        in_addr_t       peerip;
        CTcpSockData():islisenter(false),fd(-1),parser(NULL){}
    };
private:
    int _epfd;
    const CConfig & _cfg;
    CSecKeyMap & _smap;
    CThreadsPool & _pool;

    //we may have more than one listen socket port and ip.
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

private:
    class CTcpJob : public CJob
    {
private:
        friend class CTcpListener;
        CTcpSockData * _sk;

public:
        CTcpJob(CTcpSockData * sk, void * arg) :
            CJob(arg), _sk(sk)
        {
        }
        virtual bool run();
    };
};
#endif /*TCP_LISTENER_H_*/
