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
    _startok = true;
    CJob * j;
    while (!_exit)
    {
        _event.lock();
        if (_job == NULL)
        {
            _event.wait();
        }
        j = _job;
        _job = NULL;
        _event.unlock();
        j->run();
        _boss->moveToIdleList(this);

       // pthread_testcancel();
    }
}

void CWorkThread::run(CJob * job)
{
    CAutoMutex dumy(_event.mutex());
    _job = job;
    _event.signal();
}
void CThreadsPool::moveToIdleList(CWorkThread * w)
{
    list<CWorkThread *>::iterator it;

    CAutoMutex dumy(_event.mutex());
    
    for (it = _busyList.begin(); it != _busyList.end(); ++it)
    {
        if (*it == w)
            break;
    }
    if (it == _busyList.end())
    {
        LOGE("error.\n")
      
        return;
    }
    _busyList.erase(it);
    _idleList.push_back(w);
     
    //_busyList.erase(w);
    //_idleList.insert(w);
    _event.signal();
}
void CThreadsPool::doWork()
{
    CWorkThread * w;
    while (1)
    {

        CJob * job = (CJob *)_queue.receive();

        if (job)
        {
            //find a idle thread from idle list and move to busy list.
            _event.lock();
            if (_idleList.empty())
            {
                _event.wait();
            }

            w = *(_idleList.begin());
            _idleList.erase(_idleList.begin());
            _busyList.push_back(w);
            _event.unlock();
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

    _event.lock();
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
    _event.unlock();

    _queue.destroy();
}

