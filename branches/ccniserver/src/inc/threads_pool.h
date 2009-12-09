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
/*             threads_pool.h                                                          */
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
#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_
#include <list>
#include <set>
using namespace std;

#include "event.h"
#include "mutex.h"
#include "msgqueue.h"
#include "cmsgqueue.h"
#include "thread.h"

/*
 * 
 * main idea of thread pool
 *   .  CThreadsPool contains some workthreads and one msgqueue and one management threads.
 *   .  CThreadsPool once initialized(function create), it will create above things.
 *   .  CWorkThread once created, it will block at waitevent
 *   .  CThreadPool's management thread receive job(msg) from msgqueue and find a idle working thread, 
 *        then weak up the working thread, and let the working thread do the job.
 *   .  CJob is a interface for user.
 * 
 * code example :
 *      
 *     //extends CJob and overwrite run    
 *     class CMyJob : public CJob
 *     {
 *         bool run()
 *         {
 *             printf("hello, this is myjob\n");
 *             usleep(10);
 *             //delete self to avoid memory leak.
 *             delete(this);
 *         }
 *     }
 * 
 *     //
 *     CThreadsPool pool;
 *     pool.create(4);          //create 4 work threads.
 *     //Note: it's depends on user to delete the CMyJob instance in run()  
 *     pool.assign(new CMyJob());
 * 
 * */

class CJob
{
protected:
    void * _arg;
public:
    virtual bool run() = 0;
    CJob(void * arg) :
        _arg(arg)
    {
    }
    CJob() :
        _arg(NULL)
    {
    }
    virtual ~CJob()
    {
    }
};

class CThreadsPool;

class CWorkThread : public CThread
{
protected:
  
    CThreadsPool * _boss;
 
    bool volatile _exit;
    CEvent _event;
    CCMsgQueue _queue;
    CJob * volatile  _job;
    void * _arg;
    bool volatile _startok;
public:
    CWorkThread(CThreadsPool * bo) :
        _boss(bo),_exit(false), _job(NULL), _arg(NULL), _startok(false)
    {
    }
    ~CWorkThread()
    {
    }
public:

    void run(CJob * job);
    bool create()
    {
        if (!CThread::create())
        {
            return false;
        }
        //make sure the worker thread already startup..
        while (!_startok)
        {
            usleep(10);
        }
        return true;
    }
    
public:
    virtual void doWork();
};

class CThreadsPool:public CThread
{
    friend class CWorkThread;
protected:
    
    CEvent _event;
    int _threadCount;
    CCMsgQueue _queue;
    list<CWorkThread *> _idleList;
    list<CWorkThread *> _busyList;

    
    void moveToIdleList(CWorkThread * w);
public:
    CThreadsPool()
    {
    }
    ~CThreadsPool()
    {
        destroy();
    }

public:
    virtual void doWork();
    
    bool create(int cnt = 0);
    void destroy();

    bool assign(CJob * jb)
    {
        return _queue.send(jb);
    }
    int getQueuedJobs()
    {
        return _queue.getMsgCnt();
    }
    int getBusyCount()
    {
        CAutoMutex dumy(_event.mutex());
        return (int)_busyList.size();
    }
 
   
};

#endif /*THREAD_POOL_H_*/

