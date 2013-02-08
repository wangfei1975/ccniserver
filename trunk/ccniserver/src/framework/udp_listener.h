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
/*             udp_listener.h                                                          */
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
#ifndef UDP_LISTENER_H_
#define UDP_LISTENER_H_

#include <sys/types.h>
#include <sys/socket.h>

#include "log.h"
#include "config.h"
#include "thread.h"
#include "ccni.h"
#include "threads_pool.h"
#include "seckey_map.h"
class CUdpSockData
{
public:
    int fd;
    CMutex lk;

    bool sendto(const void * buf, size_t len, const struct sockaddr_in * addr, socklen_t addrlen)
    {
        CAutoMutex dumy(lk);

        if (::sendto(fd, buf, len, 0, (struct sockaddr *)addr, addrlen) != (ssize_t)len)
        {
            return false;
        }
        return true;
    }
};
class CUdpListener : public CThread
{
public:

private:
    friend class CUdpJob;
    //we may have multiple udp listen sockets on different interfaces or ports.
    vector <CUdpSockData> _fds;
    int _epfd;

    //reference of some required informations.
    const CConfig & _cfg;
    CSecKeyMap & _smap;
    CThreadsPool & _pool;
 
public:
    CUdpListener(const CConfig &cfg, CSecKeyMap & smap, CThreadsPool & pool) :
        _epfd(-1), _cfg(cfg), _smap(smap), _pool(pool)
    {
    }

    ~CUdpListener()
    {
        destroy();
    }

public:
    
    bool create();
    void destroy();
    virtual void doWork();

private:
    void _doread(CUdpSockData * sk);

private:
    class CUdpJob : public CJob
    {
        friend class CUdpListener;
private:
        CUdpSockData * _sock;
        CCNI_HEADER _hd;
        struct sockaddr_in _addr;

public:
        CUdpJob(CUdpSockData * sk, const CCNI_HEADER & hd, const struct sockaddr_in & addr,
                void * arg) :
            CJob(arg), _sock(sk), _hd(hd), _addr(addr)
        {
        }
        virtual bool run();
    };
};

#endif /*UDP_LISTENER_H_*/
