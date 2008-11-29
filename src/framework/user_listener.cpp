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
            //if (!((events[i].events & EPOLLIN) ||(events[i].events & EPOLLPRI)))
            {
            //    continue;
            }
            LOGI("event is 0x%x\n", events[i].events);
            if (events[i].events & EPOLLHUP)
            {
                LOGI("epoll hang up...\n");
               // _epollDel((CClient *)events[i].data.ptr);
            }
            else if (events[i].events & EPOLLRDHUP)
            {
                LOGI("epoll rd hup.\n");
            }
            if (events[i].data.ptr == NULL)
            {
               
                CClient * cli;
                int isnew;
                read(_pipfd[0], &cli, sizeof(cli));
                read(_pipfd[0], &isnew, sizeof(isnew));
              //  LOGI("addr user %d\n", isnew);
                _epollAdd(cli, isnew);
            }
            else
            {
                 _epollDel((CClient *)events[i].data.ptr);
                 CEngine::instance().threadsPool().assign((CClient *)events[i].data.ptr);
            }
        }

        pthread_testcancel();
    }
}
