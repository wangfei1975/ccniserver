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
       // int _pipfd[2];
        int _epfd;

public:
        CListenThread() :
            _epfd(-1)
        {
           // _pipfd[0] = _pipfd[1] = -1;
        }
        ~CListenThread()
        {
            destroy();
        }
        bool create()
        {
            /*
            if (pipe(_pipfd) < 0)
            {
                LOGE("create pipe error:%s.\n", strerror(errno));
                return false;
            }
            */ 
            if ((_epfd = epoll_create(10)) < 0)
            {
                LOGE("create epoll fd error: %s\n", strerror(errno));
                return false;
            }
            /*
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLPRI;
            ev.data.ptr = NULL;
            if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _pipfd[0], &ev) < 0)
            {

                LOGE("epoll ctrl add fd error: %s\n", strerror(errno));
                return false;
            }
            */
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
                _epfd = -1;
               // close(_pipfd[0]);
               // close(_pipfd[1]);
               // _pipfd[0]=_pipfd[1] = -1;
                
            }
        }
        bool assign(CClientTask * ctsk)
        {
            CClientPtr cli = ctsk->client();
            return cli->addToEpoll(_epfd);
        }
        virtual void doWork();
private:
       
    };
private:
    list <CListenThread *> _ths;
    CMutex _lk;
public:
    void assign(CClientTask * usr)
    {
      
        CAutoMutex dumy(_lk);
        
        list <CListenThread *>::iterator it;
      
        it = _ths.begin();
        (*it)->assign(usr);
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
