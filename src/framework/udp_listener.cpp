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
/*             udp_listener.cpp                                                        */
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
#include "udp_listener.h"
#include "utils.h"
#include "engine.h"

bool CUdpListener::create()
{

    CUdpSockData fd;
    _fds.clear();

    CConfig::addrlist_t::const_iterator it;
    for (it = _cfg.udplst.begin(); it != _cfg.udplst.end(); ++it)
    {
        if ((fd.fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            LOGE("create socket error %s\n", strerror(errno));
            return false;
        }
        int opt = 1;
        if (setsockopt(fd.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            LOGE("set sockopt error %s\n", strerror(errno));
            return false;
        }
        if (bind(fd.fd, (struct sockaddr *) &(*it), sizeof(*it)) < 0)
        {
            LOGE("bind socket to ip port error %s:%d!\n", inet_ntoa((*it).sin_addr), ntohs((*it).sin_port));
            return false;
        }
        _fds.push_back(fd);
    }
    LOGD("%d udp socket fds created.\n", _fds.size());
    if ((_epfd = epoll_create(10)) < 0)
    {
        LOGE("create epoll fd error: %s\n", strerror(errno));
        return false;
    }
    LOGD("create create epfd success.\n");
    //add all udp sockets to epoll events.
    struct epoll_event ev;
    for (unsigned int i = 0; i < _fds.size(); i++)
    {
        ev.events = EPOLLIN | EPOLLPRI;
        ev.data.ptr = &_fds[i];
        if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _fds[i].fd, &ev) < 0)
        {
            LOGE("epoll ctrl add fd error: %s\n", strerror(errno));
            return false;
        }

    }

    if (!CThread::create())
    {
        LOGE("create thread error.\n");
        return false;
    }

    return true;
}
void CUdpListener::destroy()
{
    CThread::destroy();

    close(_epfd);
    _epfd = -1;

    for (unsigned int i = 0; i < _fds.size(); i++)
    {
        close(_fds[i].fd);
    }
    _fds.clear();
}

void CUdpListener::doWork()
{
    struct epoll_event events[10];
    int nfds;
    LOGD("udp listener thread start waiting...\n");
    while (1)
    {
        if ((nfds = epoll_wait(_epfd, events, 10, -1)) < 0)
        {
            LOGW("epoll wait error: %s\n", strerror(errno));
            pthread_testcancel();
            continue;
        }
        LOGD("epoll wait events %d\n", nfds);
        for (int i = 0; i < nfds; i++)
        {
            if ((events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI))
            {
                _doread((CUdpSockData *)events[i].data.ptr);
            }
        }
        pthread_testcancel();
    }
}

void CUdpListener::_doread(CUdpSockData * sk)
{
    CCNI_HEADER hd;
    struct sockaddr_in addr;
    socklen_t flen = (socklen_t)sizeof(struct sockaddr);
    ssize_t len;
    if ((len = recvfrom(sk->fd, &hd, sizeof(CCNI_HEADER), 0, (struct sockaddr *)&(addr), &flen))
            != sizeof(hd))
    {
        LOGW("receive ccni header error, discard!\n");
        return;
    }

    if (!hd.verify())
    {
        LOGW("not valid ccni connect req header, discard.\n");
        return;
    }
    static int udpacct = 0;
    LOGW("udp account %d\n", ++udpacct);
    _pool.assign(new CUdpJob(sk, hd, addr, this));

}

bool CUdpListener::CUdpJob::run()
{
    LOGD("cudp job run.\n");
    LOGV("received ccni header from %s:%d is :\n", inet_ntoa(_addr.sin_addr), ntohs(_addr.sin_port));
    DUMPBIN(&_hd, sizeof(_hd));

    const char * sec = ((CUdpListener *)_arg)->_cfg.secret.c_str();
    //generate secret key.
    CSecKeyMap::genSecret((uint8_t *)&(_hd.secret1), _addr, sec);
    LOGD("secret1:%x\n", _hd.secret1);

    CSecKeyMap & map(((CUdpListener *)_arg)->_smap);

    if (!map.insert(_hd.secret1, _hd.secret2, _sock, _addr))
    {
        LOGW("pair {secret key, addr} already in map.\n");
        delete this;
        return false;
    }
    LOGV("key map size:%d\n", map.size());
    LOGV("response ccni header:\n");
    DUMPBIN(&_hd, sizeof(_hd));
    //send back key.
    if (!_sock->sendto(&_hd, sizeof(_hd), &_addr, sizeof(_addr)))
    {
        LOGW("send connect response error: %s\n", strerror(errno));
        map.remove(_hd.secret1, _hd.secret2);
        delete this;
        return false;
    }
    
    uint32_t udpcnt = CEngine::instance().counter().incUdpCount();
    LOGW("udp send back count %d\n", udpcnt);
    delete this;
    return true;
}

