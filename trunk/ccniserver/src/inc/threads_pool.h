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
using namespace std;

#include "event.h"
#include "mutex.h"
#include "msgqueue.h"

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

class CWorkThread
{
protected:
	CMutex _lk;

	CThreadsPool * _boss;
	pthread_t _thid;
	bool _exit;
	CEvent _event;
	CJob * _job;
	void * _arg;
    bool   _startok;
public:
	CWorkThread(CThreadsPool * bo) :
		_boss(bo), _thid(0), _exit(false), _job(NULL), _arg(NULL),_startok(false)
	{
	}
	~CWorkThread()
	{
		deInit();
	}
public:

	void run(CJob * job);
	bool init()
	{
		if (pthread_create(&_thid, NULL, (void *(*)(void*))_fun, this) < 0)
		{
			_thid = 0;
			return false;
		}
		//make sure the worker thread already startup..
		while(!_startok)
		{
			usleep(10);
		}
		return true;
	}
	void deInit()
	{
		if (_thid)
		{
			pthread_cancel(_thid);
			_thid = 0;
		}
	}

	void * doWork();

private:
	static void * _fun(CWorkThread * w)
	{
		return w->doWork();
	}

};
class CThreadsPool
{
	friend class CWorkThread;
protected:
	pthread_t _thid;
	CEvent _event;
	int _threadCount;
	CMsgQueue _queue;
	CMutex _lk;

	list<CWorkThread *> _idleList;
	list<CWorkThread *> _busyList;

	void * doManagement();
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
	bool create(int cnt = 0);
	void destroy();
	

	bool assign(CJob * jb)
	{
		return _queue.send(jb);
	}
	int getQueuedJobs()
	{
		return _queue.getQueueMsgCnt();
	}
	int getBusyCount()
	{
		int cnt;
		_lk.lock();
		cnt = (int)_busyList.size();
		_lk.unlock();
		return cnt;
	}

private:
	static void * _fun(CThreadsPool * mgr)
	{
		return mgr->doManagement();
	}
};

#endif /*THREAD_POOL_H_*/

