/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             user_listener.h                                                         */
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

#ifndef USER_LISTENER_H_
#define USER_LISTENER_H_
#include <vector>

#include <list>
#include "log.h"
#include "config.h"
#include "thread.h"
#include "client.h"

class CUserListener
{
private:
    class CListenThread : public CThread
    {
private:
        int _pipfd[2];
        int _epfd;

public:
        CListenThread() :
            _epfd(-1)
        {
            _pipfd[0] = _pipfd[1] = -1;
        }
        ~CListenThread()
        {
            destroy();
        }
        bool create()
        {
            if (pipe(_pipfd) < 0)
            {
                LOGE("create pipe error:%s.\n", strerror(errno));
                return false;
            }

            if ((_epfd = epoll_create(10)) < 0)
            {
                LOGE("create epoll fd error: %s\n", strerror(errno));
                return false;
            }

            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLPRI;
            ev.data.ptr = NULL;
            if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _pipfd[0], &ev) < 0)
            {

                LOGE("epoll ctrl add fd error: %s\n", strerror(errno));
                return false;
            }

            if (!CThread::create())
            {
                return false;
            }
            return true;
        }
        void destroy()
        {
            CThread::destroy();
            if (_epfd != -1)
            {
                close(_epfd);
                close(_pipfd[0]);
                close(_pipfd[1]);
                _pipfd[0]=_pipfd[1] = _epfd = -1;
            }
        }
        void assign(CClientTask * usr, int isread)
        {
            write(_pipfd[1], &usr, sizeof(usr));
            write(_pipfd[1], &isread, sizeof(isread));
        }
        virtual void doWork();
private:
        bool _epollAdd(CClientTask * wcli, int isread)
        {
            struct epoll_event ev;

            /*
             * we use oneshot flag for epoll for client socket.
             * 
             * this will ensure 
             * */
            if (isread == 1)
            {
               ev.events = EPOLLIN | EPOLLPRI ;// | EPOLLET;
            }
            else
            {
               ev.events = EPOLLOUT;
            }
            ev.data.ptr = wcli;
            CClientPtr cli = wcli->lock();
            if (cli == NULL)
            {
                return false;
            }
            if (epoll_ctl(_epfd, EPOLL_CTL_ADD, cli->tcpfd(), &ev) < 0)
            {

                LOGE("epoll ctrl add fd error: %s\n", strerror(errno));
                return false;
            }

            //            else
            //            {
            //                if (epoll_ctl(_epfd, EPOLL_CTL_MOD, cli->tcpfd(), &ev) < 0)
            //                {
            //                    LOGE("epoll ctrl modify fd error: %d %d %s\n", cli->tcpfd(), errno, strerror(errno));
            //                    return false;
            //                }
            //            }
            return true;

        }
        void _epollDel(CClientTask * wcli)
        {
            struct epoll_event ev;
            CClientPtr cli = wcli->lock();
            if (cli == NULL)
            {
               return ;
            }
            if (epoll_ctl(_epfd, EPOLL_CTL_DEL, cli->tcpfd(), &ev) < 0)
            {
                LOGE("epoll ctrl del fd error: %s\n", strerror(errno));
            }
        }
    };
private:
    list <CListenThread *> _ths;
    CMutex _lk;
public:
    void assign(CClientTask * usr, int isread=1)
    {
      
        CAutoMutex dumy(_lk);
        
        list <CListenThread *>::iterator it;
      
        it = _ths.begin();
        (*it)->assign(usr, isread);
        CListenThread * th = *it;
        _ths.erase(it);
        _ths.push_back(th);
    }
    bool create(const CConfig & cfg)
    {
        CListenThread * th;
        for (unsigned int i = 0; i < cfg.usr_listen_threads; i++)
        {
            th = new CListenThread();
            if (!th->create())
            {
                return false;
            }
            _ths.push_back(th);
        }
        LOGD("create %d user listen threads success.\n", cfg.usr_listen_threads);
        return true;
    }

    void destroy()
    {
        list <CListenThread *>::iterator it;
        for (it = _ths.begin(); it != _ths.end(); ++it)
        {
            delete (*it);
        }
        _ths.clear();
    }
};

#endif /*USER_LISTENER_H_*/
