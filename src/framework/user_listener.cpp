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

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             user_listener.cpp                                                       */
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


#include "user_listener.h"
#include "engine.h"
void CUserListener::CListenThread::doWork()
{
    struct epoll_event events[20];
    int nfds;
    while (1)
    {
        if ((nfds = epoll_wait(_epfd, events, 20, -1)) < 0)
        {
            LOGE("epoll wait error: %s\n", strerror(errno));
            pthread_testcancel();
            continue;
        }
       // LOGV("epoll wait events: %d\n", nfds);
        for (int i = 0; i < nfds; i++)
        {
            //if (!((events[i].events & EPOLLIN) ||(events[i].events & EPOLLPRI)))
            {
            //    continue;
            }
          //  LOGI("event is 0x%x\n", events[i].events);
            static int hupcnt = 0;
            if (events[i].events & EPOLLHUP)
            {
                LOGW("epoll hang up... %d\n", ++hupcnt);
               // _epollDel((CClient *)events[i].data.ptr);
            }
            else if (events[i].events & EPOLLRDHUP)
            {
                LOGW("epoll rd hup. %d\n", ++hupcnt);
            }
           
            if (events[i].data.ptr == NULL)
            {
                LOGW("error. epoll data ptr = NULL\n");
               /*
                CClientTask * cli;
                int isread;
                if (read(_pipfd[0], &cli, sizeof(cli)) < 0)
                {
                    LOGE("read pipe error.\n")
                }
                if (read(_pipfd[0], &isread, sizeof(isread)) < 0)
                {
                    LOGE("read pipe 1 error.\n");
                }
                 LOGV("add user %d\n", isread);
                _epollAdd(cli, isread);
                */
            }
            else
            {
                CClientTask * cli = (CClientTask *)events[i].data.ptr;
                 cli->client()->rmFromEpoll();
                 CEngine::instance().threadsPool().assign(cli);
           }
        }

        pthread_testcancel();
    }
}
