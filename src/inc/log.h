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
/*             log.h                                                                   */
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
/*             2008-11-15     change log level to non-static                           */
/*                            move some implementations to log.cpp                     */
/***************************************************************************************/

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/epoll.h>

#include "cmsgqueue.h"
#include "mutex.h"
/*
 * main idea of log system.
 *    . create a message queue and a thread for all CLog instance.
 *    . function print will compose a LOGITEM and send it to msg queu.
 *    . log thread receive LOGITEM from msg queue, and do real output.(see function update) 
 *    . each CLog instance has its own log type, log file serials, split strategy, log level, etc.
 * */

class CLog
{
public:
    const static int LOG_BUF_SIZE = 1024;
    const static int DEFAULT_LOGFILE_SIZE = 1024*1024;
    enum LOGTYPE
    {
        DEBUG_LOG,
        EVENT_LOG
    };
    enum LOGFILE_CFG
    {
        NO_SPLIT,
        SPLIT_ON_DATE,
        SPLIT_ON_SIZE
    };
    struct LOGITEM
    {
        CLog * logger;
        struct tm logtime;
        char txt[LOG_BUF_SIZE];
        LOGITEM(CLog * l) :
            logger(l)
        {
        }
    };
public:

    CLog(const char * fname = NULL, LOGTYPE type = DEBUG_LOG, LOGFILE_CFG policy = SPLIT_ON_SIZE,
            int fsize = DEFAULT_LOGFILE_SIZE);

    ~CLog();

    //build string send to msg queue.
    int print(int level, const char * file, const char * function, int line, const char * fmt, ...);

    int print(const char * fmt, ...);
    
    int dumpbin(const void * buf, int len);
    //write to file
    void update(LOGITEM * item);
public:

    static CLog * dbgLog();

private:
    static pthread_t _pth;
    static CCMsgQueue * _queue;
    static int _instance;

private:
    std::string _fname; //log file name
    int _fnumber; //file number  the exact file name is : fnameXXXX.log
    LOGFILE_CFG _lpolicy; //how we split the log file.
    int _fsize; //if split in fixed size
    FILE * _logfp;
    struct tm _curftime; //used for split on date
    LOGTYPE _type;
    CMutex _lock;
    int _logLevel;

private:
    bool _createDateLogFile(const struct tm * t);
    bool _createSizeLogFile();
    bool _createLogFile();
private:

    static void * thread_func(void *);
    
   
private:
    /*
    class CAllocator
    {
    private:
        CMutex _lk;    
        list <LOGITEM *> _mempools;
        static const unsigned SSIZE = 1024; 
    public:
        LOGITEM * alloc(CLog * l)
        {
//            _lk.lock();
//            if (!_mempools.empty())
//            {
//                LOGITEM * p = *_mempools.begin();
//                _mempools.erase(_mempools.begin());
//                _lk.unlock();
//                p->logger = l;
//                return p;
//            }
//            _lk.unlock();
            return new LOGITEM(l);
        }
        void free(LOGITEM * p)
        {
//            _lk.lock();
//            if (_mempools.size() < SSIZE)
//            {
//               _mempools.push_back(p);
//               _lk.unlock();
//            }
//            else
//            {
//                _lk.unlock();
//                delete p;
//            }
            delete p;
        }
        CAllocator()
        {
            for (unsigned int i = 0; i < SSIZE; i++)
            {
                _mempools.push_back(new LOGITEM(NULL));
            }
        }
        ~CAllocator()
        {
            list <LOGITEM *>::iterator it;
            _lk.lock();
            for (it = _mempools.begin(); it != _mempools.end(); ++it)
            {
                delete (*it);
            }
            _lk.unlock();
        }
    };
    static CAllocator _alloc;
     
    */
};

/*  
 *  debug logger
 *
 *  LOGE  - error           0
 *  LOGW  - warning         1
 *  LOGI  - inforation      2
 *  LOGD  - debug           3
 *  LOGV  - verbos          4
 *
 * debug log format:
 [FileName][FunctionName][LineNumber][ThreadID][DateTime:Microsecond][LogLevel]log text.

 Example:
 [main.cpp][main][7][3081173744][1227202771:971276][0]ccni server, log error.
 */
#define LOGE(...)     {CLog::dbgLog()->print(0, __FILE__, __func__, __LINE__, __VA_ARGS__);}
#define LOGW(...)     {CLog::dbgLog()->print(1, __FILE__, __func__, __LINE__, __VA_ARGS__);}
#define LOGI(...)     {CLog::dbgLog()->print(2, __FILE__, __func__, __LINE__, __VA_ARGS__);}
#define LOGD(...)     {CLog::dbgLog()->print(3, __FILE__, __func__, __LINE__, __VA_ARGS__);}
#define LOGV(...)     {CLog::dbgLog()->print(4, __FILE__, __func__, __LINE__, __VA_ARGS__);}

#define DUMPBIN(buf, len) {CLog::dbgLog()->dumpbin(buf, len);}
#define OOPS(...)    {LOGE("****OOPS****\n");LOGE(__VA_ARGS__);LOGE("****OOPS****\n"); exit(0);}
#define ASSERT(v)    {if (!(v)){ OOPS("BUGS HERE.\n");}}

#endif /*LOG_H_*/
