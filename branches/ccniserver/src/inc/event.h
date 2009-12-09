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
/*             event.h                                                                 */
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

#ifndef EVENT_H_
#define EVENT_H_
#include <pthread.h>
#include <sys/time.h>
#include "mutex.h"

class CEvent
{
private:
    pthread_cond_t  _cond;
    CMutex _lk;
public:
    CMutex & mutex()
    {
        return _lk;
    }
    CEvent()
    {
        pthread_cond_init(&_cond, NULL);
       
    }

    ~CEvent()
    {
        
        pthread_cond_destroy(&_cond);
    }
    
    void lock()
    {
        _lk.lock();
    }
    void unlock()
    {
        _lk.unlock();   
    }
    //wait until the event happens, or time out
    bool wait(int millisecond = -1)
    {
        struct timeval now;
        struct timespec timeout;
        int retcode;
 
        if (millisecond > 0)
        {
            gettimeofday(&now, NULL);
            timeout.tv_sec = now.tv_sec + (millisecond/1000);
            if (now.tv_usec + ((millisecond%1000)*1000) > 1000000)
            {
                now.tv_sec += 1;
                now.tv_usec = (now.tv_usec+((millisecond%1000)*1000))%1000000;
            }
            timeout.tv_nsec = now.tv_usec * 1000;
            retcode = pthread_cond_timedwait(&_cond, &(_lk._mutex), &timeout);
        }
        else
        {
            retcode = pthread_cond_wait(&_cond, &(_lk._mutex));
        }

       
        return (retcode == 0); //ETIMEDOUT)
    }
    //if signal before someone waiting for it, nothing will happen.
    //if someone waiting for it after some one signal it, it will waiting until some signal it next time.
    void signal()
    {
          pthread_cond_broadcast(&_cond);
    }
};

#endif /*EVENT_H_*/
