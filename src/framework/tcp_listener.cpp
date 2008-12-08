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
#include "engine.h"

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
        if (listen(fd.fd, 1000) < 0)
        {
            LOGE("call listen error %s\n", strerror(errno));
            return false;
        }
        fd.islisenter = true;
        _fds.push_back(fd);
    }

    if ((_epfd = epoll_create(1000)) < 0)
    {
        LOGE("create epoll fd error: %s\n", strerror(errno));
        return false;
    }

    //add all tcp sockets to epoll events.
    for (unsigned int i = 0; i < _fds.size(); i++)
    {
        if (!_epollAddSock(&_fds[i]))
        {
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
        if ((nfds = epoll_wait(_epfd, events, 10, _cfg.login_timeout*1000)) < 0)
        {
            LOGW("epoll wait error: %s\n", strerror(errno));
            pthread_testcancel();
            continue;
        }

       //  LOGV("tcp epoll wait events: %d\n", nfds);
        for (int i = 0; i < nfds; i++)
        {
             if (!((events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI)))
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

        //clear timeouted sockets.

        pthread_testcancel();
    }
}

void CTcpListener::_doaccept(CTcpSockData * sk)
{
    struct sockaddr_in addr;
    socklen_t len = (socklen_t)sizeof(addr);
    int client = accept(sk->fd, (struct sockaddr *) &addr, &len);
    static int acccnt = 0;
   
    if (client < 0)
    {
        LOGE("accept error: %s\n", strerror(errno));
        return;
    }
    LOGW("account cnt = %d\n", ++acccnt);
    LOGD("got connection from %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    if (set_nonblock(client) < 0)
    {
        LOGE("set non block error: %s\n", strerror(errno));
        return;
    }
    CTcpSockData * cli = new CTcpSockData(false, client, addr.sin_addr.s_addr);
    if (!_epollAddSock(cli))
    {
        delete cli;
    }
}

void CTcpListener::_doread(CTcpSockData *sk)
{

    CCNIMsgParser::state_t st = sk->parser.read(sk->fd);
    //read error, remote disconnected.
    if (st == CCNIMsgParser::st_rderror)
    {
        LOGW("remote disconnected. %s\n", strip(sk->peerip));
        _epollDelSock(sk);
        close(sk->fd);
        delete sk;
    }
    else if (st == CCNIMsgParser::st_hderror)
    {
        LOGW("read a invalid ccni header from %s,force close it.\n", strip(sk->peerip));
        //TBD: add this ip to black list
        _epollDelSock(sk);
        close(sk->fd);
        delete sk;
    }
    else if (st == CCNIMsgParser::st_rdok)
    {
        LOGD("read ccni message ok.\n%s\n", sk->parser.data());
        _epollDelSock(sk);
        _pool.assign(new CTcpJob(sk, this));
    }
    else
    {
        //nothing to do, just waiting for next epoll triger.
    }
}
bool CTcpListener::CTcpJob::doLogin(CUdpSockData * udp, const struct sockaddr_in & udpaddr)
{
    CXmlNode msglogin = _sk->parser.getmsg(xmlTagLogin);
    if (msglogin.isEmpty())
    {
        LOGW("first msg is not login msg.\n");
        return false;
    }
  
    LOGD("msg is:\n%s\n", _sk->parser.data());
    //verify username and password
    string username, password;
    msglogin.findChild(xmlTagUserName).getContent(username);
    msglogin.findChild(xmlTagPassword).getContent(password);

    LOGD("uname: %s, password: %s\n", username.c_str(), "****");

    CDataBase::CRecord rec;
    CCNIMsgPacker msg;
    CXmlMsg lgmsg;
    msg.create();
    lgmsg.create(xmlTagLoginRes);
    const CCNI_HEADER & hd = _sk->parser.header();

    if (CEngine::instance().dataMgr().findClient(username.c_str()) != NULL)
    {
        LOGW("user %s alread login.\n", username.c_str());
        lgmsg.addParameter(xmlTagReturnCode, -2);
        lgmsg.addParameter(xmlTagReturnInfo, "User already login.");

        msg.appendmsg(lgmsg);
        msg.pack(hd.seq, hd.udata, hd.secret1, hd.secret2);
        msg.block_send(_sk->fd);

        return false;
    }

    if (CEngine::instance().db().verifyUser(username.c_str(), password.c_str(), rec) < 0)
    {
        LOGW("user name or password error.\n");
        //send response to ...
        lgmsg.addParameter(xmlTagReturnCode, -1);
        lgmsg.addParameter(xmlTagReturnInfo, "Error user name or password.");

        msg.appendmsg(lgmsg);
        msg.pack(hd.seq, hd.udata, hd.secret1, hd.secret2);
        msg.block_send(_sk->fd);

        return false;
    }
    LOGI("user %s login success.\n", username.c_str());
     
    lgmsg.addParameter(xmlTagReturnCode, 0);
    lgmsg.addParameter(xmlTagReturnInfo, "Login success.");
    msg.appendmsg(lgmsg);
    msg.pack(hd.seq, hd.udata, hd.secret1, hd.secret2);
    
    msg.block_send(_sk->fd);

    CClient * cli = new CClient(_sk->fd, udp, hd.secret1, hd.secret2, udpaddr, rec);
    CEngine::instance().dataMgr().addClient(cli);
    CEngine::instance().usrListener().assign(cli);
    static int ascnt = 0;
    LOGW("loging succ count:%d\n", ++ascnt);
    delete _sk;
    return true;
}

bool CTcpListener::CTcpJob::run()
{
    CTcpListener * lster = (CTcpListener *)_arg;

    LOGD("client secret key:\n");
    DUMPBIN(&_sk->parser.header(), sizeof(CCNI_HEADER));

    struct sockaddr_in udpaddr;
    CUdpSockData * udp;
    //verify secret key.
    if (!lster->_smap.verifykey(_sk->parser.header().secret1, _sk->parser.header().secret2, _sk->peerip, udp, udpaddr))
    {
        LOGW("verify secret key and ip error:%s.\n", strip(_sk->peerip));
        goto err_end;
    }
    //parse xml mesages.
    if (!_sk->parser.parse())
    {
        LOGW("parse xml error.\n");
        goto err_end;
    }

    if (!doLogin(udp, udpaddr))
    {
        LOGW("login error!\n");
        goto err_end;
    }

    delete this;
    return true;

    err_end: close(_sk->fd);
    delete _sk;
    delete this;
    return false;
}

