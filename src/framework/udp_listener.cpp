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
            LOGE("bind socket to ip port error %s:%d!\n",
                    inet_ntoa((*it).sin_addr), ntohs((*it).sin_port));
            return false;
        }
        _fds.push_back(fd);
    }
    if ((_epfd = epoll_create(10)) < 0)
    {
        LOGE("create epoll fd error: %s\n", strerror(errno));
        return false;
    }

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
    if ((len = recvfrom(sk->fd, &hd, sizeof(hd), 0, (struct sockaddr *)&(addr), &flen)) != sizeof(hd))
    {
        LOGW("receive ccni header len error, discard!\n");
        return;
    }

    if (hd.hdlen != sizeof(hd) || hd.ver_major != 1 || hd.ver_minor != 0 || hd.type != 0)
    {
        LOGW("not valid ccni connect req header, discard.\n");
        return;
    }
    _pool.assign(new CUdpJob(sk, hd, addr, this));
    
}

bool CUdpListener::CUdpJob::run()
{
    const char * sec = ((CUdpListener *)_arg)->_cfg.secret.c_str();
    int seclen = strlen(sec);
    unsigned char buf[512];
    memcpy(buf, &_addr, sizeof(_addr));
    memcpy(buf + sizeof(_addr), sec, seclen);

    //generate secret key.
    md5_calc((uint8_t *)&(_hd.secret), buf, sizeof(_addr)+seclen);

    //insert {key, addr} to hash map.
    CSecKeyMap & map(((CUdpListener *)_arg)->_smap);

    if (!map.insert(_hd.secret, _addr))
    {
        LOGE("pair {secret key, addr} alread in map.\n");
        delete this;
        return false;
    }

    //send back key.
    if (!_sock->sendto(&_hd, sizeof(_hd), &_addr))
    {
        LOGE("send connect response error: %s\n", strerror(errno));
        map.remove(_hd.secret);
        delete this;
        return false;

    }

    delete this;
    return true;
}
