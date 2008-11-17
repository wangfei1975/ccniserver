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

class CEvent
{
private:
	pthread_cond_t   _cond;
	pthread_mutex_t  _mutex;
public:
   CEvent()
   {
   	pthread_cond_init(&_cond, NULL);
   	pthread_mutex_init(&_mutex, NULL);
   }

   ~CEvent()
   {
   	pthread_mutex_destroy(&_mutex);
   	pthread_cond_destroy(&_cond);
   }

   //wait until the event happens, or time out
   bool wait(int millisecond = -1)
   {
   	struct timeval now;
    struct timespec timeout;
    int retcode;

   	 pthread_mutex_lock(&_mutex);
   	 if (millisecond > 0)
   	 {
   	 	gettimeofday(&now, NULL);
        timeout.tv_sec  = now.tv_sec + (millisecond/1000);
        if (now.tv_usec + ((millisecond%1000)*1000) > 1000000)
        {
             now.tv_sec += 1;
             now.tv_usec = (now.tv_usec+((millisecond%1000)*1000))%1000000;
        }
        timeout.tv_nsec = now.tv_usec * 1000;
        retcode = pthread_cond_timedwait(&_cond, &_mutex, &timeout);
   	 }
   	 else
   	 {
   	 	retcode = pthread_cond_wait(&_cond, &_mutex);
   	 }

     pthread_mutex_unlock(&_mutex);
   	 return (retcode == 0); //ETIMEDOUT)
   }
   //if signal before someone waiting for it, nothing will happen.
   //if someone waiting for it after some one signal it, it will waiting until some signal it next time.
   void signal()
   {
       pthread_mutex_lock(&_mutex);
       pthread_cond_broadcast(&_cond);
       pthread_mutex_unlock(&_mutex);

   }
};



#endif /*EVENT_H_*/
