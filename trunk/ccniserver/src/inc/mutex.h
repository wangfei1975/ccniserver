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
private:
    pthread_mutex_t  _mutex;
    friend class CEvent;
    friend class CCondition;
public:
 
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
// copy
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
