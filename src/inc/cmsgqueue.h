/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             cmsgqueue.h                                                             */
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
/*             2008-06-05     initial draft                                            */
/***************************************************************************************/
#ifndef CMSGQUEUE_H_
#define CMSGQUEUE_H_
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <deque>
#include <list>
#include "mutex.h"
#include "event.h"

using namespace std;

class CCMsgQueue
{
private:

    CEvent _evt;
    deque <void *> _data;

public:

    CCMsgQueue()
    {
        //create();
    }
    ~CCMsgQueue()
    {
        // destroy();
    }

    int getMsgCnt()
    {
        CAutoMutex dumy(_evt.mutex());
        return _data.size();
    }

    bool create()
    {
        return true;
    }

    void destroy()
    {

    }

    bool send(void * buf)
    {
        CAutoMutex dumy(_evt.mutex());
        _data.push_back(buf);
        _evt.signal();
    
        return true;
    }

    void * receive()
    {
        CAutoMutex dumy(_evt.mutex());
        while (_data.empty())
        {
            _evt.wait();
        }
        void * p = (*_data.begin());
        _data.pop_front();
        return p;
    }
};

#endif /*CMSGQUEUE_H_*/
