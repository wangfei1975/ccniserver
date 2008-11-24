/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             threads_pool.cpp                                                        */
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
#include "threads_pool.h"
#include "log.h"

void CWorkThread::doWork()
{
    while (!_exit)
    {
        _startok = true;
        if (_job)
        {
            _job->run();
            _job = NULL;
            _boss->moveToIdleList(this);
            _lk.unlock();
        }
        else
        {
            _event.wait();
        }
        pthread_testcancel();
    }
}

void CWorkThread::run(CJob * job)
{
    _lk.lock();
    _job = job;
    _event.signal(); //SetEvent(_event);
}
void CThreadsPool::moveToIdleList(CWorkThread * w)
{
    list<CWorkThread *>::iterator it;
    _lk.lock();
    for (it = _busyList.begin(); it != _busyList.end(); ++it)
    {
        if (*it == w)
            break;
    }
    if (it == _busyList.end())
    {
        _lk.unlock();
        return;
    }
    _busyList.erase(it);
    _idleList.push_back(w);
    bool sig = (_idleList.size() == 1);
    _lk.unlock();

    if (sig)
    {
        _event.signal();
    }
}
void   CThreadsPool::doWork()
{
    CWorkThread * w;
    while (1)
    {
        CJob * job = (CJob *)_queue.receive();
        if (job)
        {
            //find a idle thread from idle list and move to busy list.
            _lk.lock();
            while (_idleList.empty())
            {
                _lk.unlock();
                _event.wait();
                _lk.lock();
            }
            w = *(_idleList.begin());
            _idleList.erase(_idleList.begin());
            _busyList.push_front(w);
            _lk.unlock();
            w->run(job);
        }
        pthread_testcancel();
    }
}

bool CThreadsPool::create(int cnt)
{
    //TRACEL("init thread pool %d...", cnt);

    if (!_queue.create())
    {
        return false;
    }
    if (!CThread::create())
    {
        return false;
    }
    _threadCount = cnt;
    for (int i = 0; i < cnt; i++)
    {
        CWorkThread * w = new CWorkThread(this);
        _idleList.push_back(w);
        if (!w->create())
        {
            return false;
        }
    }
    //TRACEL("init thread pool success.%d working threads created.", _threadCount);
    return true;
}

void CThreadsPool::destroy()
{
    CThread::destroy();
    
    _lk.lock();
    for (list<CWorkThread *>::iterator it = _busyList.begin(); it != _busyList.end(); ++it)
    {
        (*it)->destroy();
        delete (*it);
    }
    _busyList.clear();
    for (list<CWorkThread *>::iterator it = _idleList.begin(); it != _idleList.end(); ++it)
    {
        (*it)->destroy();
        delete (*it);
    }
    _idleList.clear();
    _lk.unlock();

    _queue.destroy();
}

