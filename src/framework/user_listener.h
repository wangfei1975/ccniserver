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
            ev.events = EPOLLIN | EPOLLPRI | EPOLLET;
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
        void assign(CClient * usr)
        {
            write(_pipfd[1], &usr, sizeof(usr));
        }
        virtual void doWork()
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
                LOGD("epoll wait events: %d\n", nfds);
                for (int i = 0; i < nfds; i++)
                {
                    if (!(events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI))
                    {
                        continue;
                    }
                    if (events[i].data.ptr == NULL)
                    {
                        //add user...
                        CClient * cli;
                        read(_pipfd[0], &cli, sizeof(cli));
                        _epollAdd(cli);
                    }
                }

                pthread_testcancel();
            }
        }
private:
        bool _epollAdd(CClient * cli)
        {
            struct epoll_event ev;
            
            /*
             * we use oneshot flag for epoll for client socket.
             * 
             * this will ensure 
             * */
            ev.events = EPOLLIN | EPOLLPRI | EPOLLONESHOT;
            ev.data.ptr = cli;
            if (epoll_ctl(_epfd, EPOLL_CTL_ADD, cli->sock(), &ev) < 0)
            {

                LOGE("epoll ctrl add fd error: %s\n", strerror(errno));
                return false;
            }
            return true;

        }
        void _epollDel(CClient * cli)
        {
            struct epoll_event ev;
            if (epoll_ctl(_epfd, EPOLL_CTL_DEL, cli->sock(), &ev) < 0)
            {
                LOGE("epoll ctrl del fd error: %s\n", strerror(errno));
            }
        }
    };
private:
    list <CListenThread *> _ths;
    CMutex _lk;
public:
    void assign(CClient * usr)
    {
        list <CListenThread *>::iterator it;
        CAutoMutex dumy(_lk);
        it = _ths.begin();
        (*it)->assign(usr);
        CListenThread * th = *it;
        _ths.erase(it);
        _ths.push_back(th);
    }
    bool create(const CConfig & cfg)
    {
        CListenThread * th;
        for (int i = 0; i < 4; i++)
        {
            th = new CListenThread();
            if (!th->create())
            {
                return false;
            }
            _ths.push_back(th);
        }
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
