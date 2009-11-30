/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             thread.h                                                                */
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
/*             2008-11-25     initial draft                                            */
/***************************************************************************************/

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
class CThread
{
private:
    pthread_t _thid;

public:
    
    virtual void doWork() = 0;
public:
    CThread() :
        _thid(0)
    {
    }
    ~CThread()
    {
        destroy();
    }
    void join()
    {
        pthread_join(_thid, NULL);
    }
    bool create()
    {
        if (pthread_create(&_thid, NULL, (void *(*)(void*))_fun, this) < 0)
        {
            _thid = 0;
            return false;
        }
        return true;
    }
    void destroy()
    {
        if (_thid)
        {
            pthread_cancel(_thid);
            _thid = 0;
        }
    }

private:
    static void * _fun(CThread * w)
    {
        w->doWork();
        return NULL;
    }
};

#endif /*THREAD_H_*/
