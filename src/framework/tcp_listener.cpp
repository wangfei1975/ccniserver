/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             tcp_listener.cpp                                                        */
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
#include "tcp_listener.h"
#include "utils.h"

bool CTcpListener::create()
{
    CTcpSockData fd;
    CConfig::addrlist_t::const_iterator it;
    for (it = _cfg.tcplst.begin(); it != _cfg.tcplst.end(); ++it)
    {
        if ((fd.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
        if (listen(fd.fd, 10) < 0)
        {
            LOGE("call listen error %s\n", strerror(errno));
            return false;
        }
        fd.islisenter = true;
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
        LOGE("create thread error!\n");
        return false;
    }
    return true;
}
void CTcpListener::destroy()
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

void CTcpListener::doWork()
{
    struct epoll_event events[10];
    int nfds;
    CTcpSockData * sk;
    while (1)
    {
        if ((nfds = epoll_wait(_epfd, events, 10, -1)) < 0)
        {
            LOGW("epoll wait error: %s\n", strerror(errno));
            pthread_testcancel();
            continue;
        }
        LOGD("epoll wait events: %d\n", nfds);
        for (int i = 0; i < nfds; i++)
        {
            if (!(events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI))
            {
                continue;
            }
            sk = (CTcpSockData *)events[i].data.ptr;
            if (sk->islisenter)
            {
                _doaccept(sk);
            }
            else
            {
                _doread(sk);
            }
        }
        pthread_testcancel();
    }
}

void CTcpListener::_doaccept(CTcpSockData * sk)
{
    struct sockaddr_in addr;
    socklen_t len = (socklen_t)sizeof(addr);
    int client = accept(sk->fd, (struct sockaddr *) &addr, &len);
    if (client < 0)
    {
        LOGE("accept error: %s\n", strerror(errno));
        return;
    }
    LOGD("got connection from %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    if (set_nonblock(client) < 0)
    {
        LOGE("set non block error: %s\n", strerror(errno));
        return;
    }
    CTcpSockData * cli = new CTcpSockData();
    cli->fd = client;
    cli->islisenter = false;
    cli->peerip = addr.sin_addr.s_addr;

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI | EPOLLET;
    ev.data.ptr = cli;
    if (epoll_ctl(_epfd, EPOLL_CTL_ADD, cli->fd, &ev) < 0)
    {

        LOGE("epoll ctrl add fd error: %s\n", strerror(errno));
        close(client);
        delete cli;
    }
}

void CTcpListener::_doread(CTcpSockData *sk)
{
    struct epoll_event ev;
    if (sk->parser == NULL)
    {
        sk->parser = new CCNIMsgParser();
    }
    CCNIMsgParser::parse_state_t st = sk->parser->read(sk->fd);
    if (st == CCNIMsgParser::st_bdok)
    {
        LOGD("read ccni message ok.\n%s\n", sk->parser->data());
        if (epoll_ctl(_epfd, EPOLL_CTL_DEL, sk->fd, &ev) < 0)
        {
            LOGE("epoll ctrl del fd error: %s\n", strerror(errno));
        }
        _pool.assign(new CTcpJob(sk, this));
    }
    else if (st == CCNIMsgParser::st_rderror || st == CCNIMsgParser::st_hderror)
    {
        if (epoll_ctl(_epfd, EPOLL_CTL_DEL, sk->fd, &ev) < 0)
        {
            LOGE("epoll ctrl del fd error: %s\n", strerror(errno));
        }

        close(sk->fd);
        delete sk->parser;
        delete sk;
    }
}

bool CTcpListener::CTcpJob::run()
{
    CTcpListener * lster = (CTcpListener *)_arg;
    if (_sk == NULL || _sk->parser == NULL || _sk->parser->state() != CCNIMsgParser::st_bdok)
    {
        LOGE("***oops***, bugs here.\n");
        goto err_end;
    }
    LOGD("client secret key:\n");
    DUMPBIN(&_sk->parser->header(), sizeof(CCNI_HEADER));
    //verify secret key.
    if (!lster->_smap.verifykey(_sk->parser->header().secret1, _sk->parser->header().secret2, _sk->peerip))
    {
        LOGW("verify secret key and ip error:%s.\n", strip(_sk->peerip));
        goto err_end;
    }
    //parse xml mesages.
    if (!_sk->parser->parse())
    {
        LOGW("parse xml error.\n");
        goto err_end;
    }
    {
        CXmlNode msglogin = _sk->parser->getmsg(xmlTagLogin);
        if (msglogin.isEmpty())
        {
            LOGW("first msg is not login msg.\n");
            goto err_end;
        }
        string strxml;
        LOGD("msg is:\n%s\n", msglogin.toString(strxml).c_str());
        //verify username and password
        string username, password;
        msglogin.findChild(xmlTagUserName).getContent(username);
        msglogin.findChild(xmlTagPassword).getContent(password);
       
        LOGD("uname: %s, password: %s\n", username.c_str(), "****");

        if (username.empty() || password.empty())
        {
            LOGW("empty username or password: %s:%s", username.c_str(), "****");
            goto err_end;
        }
        LOGI("user %s login success.\n", username.c_str());
    }
    delete this;
    return true;
err_end: 
    close(_sk->fd);
    delete _sk->parser;
    delete _sk;
    delete this;
    return false;
}

