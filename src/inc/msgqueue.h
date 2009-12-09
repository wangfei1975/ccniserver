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
/*             msgqueue.h                                                              */
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
#ifndef MSGQUEUE_H_
#define MSGQUEUE_H_
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "mutex.h"
extern int errno;

#pragma pack(push, 4)
struct MSGITEM
{
    long mtype;
    void * msg;
    MSGITEM() :
        mtype(1), msg(NULL)
    {
    }
};

class CMsgQueue
{
public:

    int _queueid;

public:

    CMsgQueue(const CMsgQueue & m) :
        _queueid(m._queueid)
    {
    }
    CMsgQueue() :
        _queueid(-1)
    {
        //create();
    }
    ~CMsgQueue()
    {
        // destroy();
    }
    int getQueue()
    {
        return _queueid;
    }
    int getMsgCnt()
    {
        struct msqid_ds ds;
        int ret;
        if ((ret = msgctl(_queueid, IPC_STAT, &ds)) < 0)
        {
            return ret;
        }
        return ds.msg_qnum;
    }
    bool create(key_t k)
    {
        if ((_queueid = msgget(k, IPC_CREAT|0660)) < 0)
        {
            printf("msg queue create error! %s\n", strerror(errno));
            return false;
        }
        // printf("msg queue create success! queue id:%d\n",_queueid);
        return true;
    }
    bool create()
    {
        if ((_queueid = msgget(IPC_PRIVATE, IPC_CREAT|0660)) < 0)
        {
            printf("msg queue create error! %s\n", strerror(errno));
            return false;
        }
        // printf("msg queue create success! queue id:%d\n",_queueid);
        return true;
    }

    void destroy()
    {
        if (_queueid > 0)
        {
            msgctl(_queueid, IPC_RMID, NULL);
            //  printf("queue %d destroied!\n", _queueid);
            _queueid = -1;
        }
    }

    bool send(void * buf)
    {
        MSGITEM msg;
        msg.msg = buf;
        if (msgsnd(_queueid, &msg, sizeof(MSGITEM)-sizeof(long), 0) < 0)
        {
            printf("send msg queue %d error:%s\n", _queueid, strerror(errno));
            return false;
        }
        return true;
    }

    bool send(void * buf, int len)
    {
        if (msgsnd(_queueid, buf, len, 0) < 0)
        {
            printf("send msg queue %d error:%s\n", _queueid, strerror(errno));
            return false;
        }
        return true;
    }

    int receive(void * buf, int len, long type)
    {
        int rlen;
        if ((rlen = msgrcv(_queueid, buf, len, type, 0)) < 0)
        {
            printf("msg queue %d receive error:%s\n", _queueid, strerror(errno));
            return -1;
        }
        return rlen;
    }

    int tm_receive(void * buf, int len, long type, int millsec)
    {
        //printf("in");
        int rlen = -1;
        for (int i = 0; i <millsec; i++)
        {
            if ((rlen = msgrcv(_queueid, buf, len, type, IPC_NOWAIT)) >0)
            {
                break;
            }
            usleep(1000);
        }
        // printf("out");
        return rlen;
    }

    int receive(void * buf, int len)
    {
        int rlen;
        if ((rlen = msgrcv(_queueid, buf, len, 0, 0)) < 0)
        {
            printf("msg queue %d receive error:%s\n", _queueid, strerror(errno));
            return -1;
        }
        return rlen;
    }

    void * receive()
    {
        MSGITEM msg;
        if (msgrcv(_queueid, &msg, sizeof(MSGITEM)-sizeof(long), 0, 0) != sizeof(MSGITEM)-sizeof(long))
        {
            printf("receive error:%s\n", strerror(errno));
            return NULL;
        }
        return msg.msg;
    }
};
#pragma pack(pop)
#endif /*MSGQUEUE_H_*/
