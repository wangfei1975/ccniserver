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
        CTcpSockData():islisenter(false),fd(-1){}
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
        int _fd;

public:
        CTcpJob(int fd, void * arg) :
            CJob(arg), _fd(fd)
        {
        }
        virtual bool run();
    };
};
#endif /*TCP_LISTENER_H_*/
