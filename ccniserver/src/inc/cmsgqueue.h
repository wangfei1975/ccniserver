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
    list <void *> _data;

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
    void * receive(int millsecond)
    {
        CAutoMutex dumy(_evt.mutex());
        if (_data.empty())
        {
            _evt.wait(millsecond);
        }
        if (_data.empty())
        {
            return NULL;
        }
        void * p = (*_data.begin());
        _data.erase(_data.begin());
        return p;
    }
    void * receive()
    {
        CAutoMutex dumy(_evt.mutex());
        while (_data.empty())
        {
            _evt.wait();
        }
        void * p = (*_data.begin());
        _data.erase(_data.begin());
        //_data.pop_front();
        return p;
    }
};

#endif /*CMSGQUEUE_H_*/
