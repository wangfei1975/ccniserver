/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             mutex.h                                                                 */
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
/*             2008-11-16     add CAutoMutex                                           */
/***************************************************************************************/

#ifndef MUTEX_H_
#define MUTEX_H_
#include <pthread.h>

class CMutex
{
public:
    pthread_mutex_t  _mutex;
 
public:
    /*
    operator pthread_mutex_t & ()
    {
        return _mutex;
    }
    */
    CMutex()
    {
        pthread_mutex_init(&_mutex, NULL);
    }
    ~CMutex()
    {
        pthread_mutex_destroy(&_mutex);
    }
    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

};
//
//a mutex helper, to avoid forget unlock, espically in multiple exit point functions.
//
class CAutoMutex
{
private:
    CMutex & _mu;
public:
    CAutoMutex(CMutex & mu) :
        _mu(mu)
    {
        _mu.lock();
    }
    ~CAutoMutex()
    {
        _mu.unlock();
    }
};

#endif /*MUTEX_H_*/
