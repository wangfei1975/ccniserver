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

#include "msgqueue.h"
#include "mutex.h"
//class CLog;

//extern CLog debugLog;
//extern CLog eventLog;
//extern CLog usrLog;


//#define TRACE(...)  debugLog.print(1, __FILE__, __LINE__, __VA_ARGS__)
//#define TRACEL(fmt, ...) debugLog.print(1, __FILE__, __LINE__, fmt"\n", __VA_ARGS__)
//#define TRACE1(...) debugLog.print(2, __FILE__, __LINE__, __VA_ARGS__)
//#define TRACE2(...) debugLog.print(3, __FILE__, __LINE__, __VA_ARGS__)
//#define TRACE3(...) debugLog.print(4, __FILE__, __LINE__, __VA_ARGS__)
//#define TRACE4(...) debugLog.print(5, __FILE__, __LINE__, __VA_ARGS__)
//#define ERROR(...)  debugLog.print(0, __FILE__, __LINE__, __VA_ARGS__)

//#define EVENTLOG(...)    eventLog.print(0, __FILE__, __LINE__, __VA_ARGS__)

//#define UTRACE(...) usrLog.print(0, __FILE__, __LINE__, __VA_ARGS__)

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
protected:
	static pthread_t _pth;
	static CMsgQueue * _queue;
	static int _instance;
public:
	static int _logLevel;
private:
	std::string _fname;    //log file name
	int _fnumber;         //file number  the exact file name is : fnameXXXX.log
	LOGFILE_CFG _lpolicy; //how we split the log file.
	int _fsize;           //if split in fixed size
	FILE * _logfp;
	struct tm _curftime; //used for split on date
	LOGTYPE _type;
	CMutex _lock;
	CMutex _ulock;
public:

	CLog(const char * fname = NULL, 
		LOGTYPE type = DEBUG_LOG,
		LOGFILE_CFG policy = SPLIT_ON_SIZE, 
		int fsize = DEFAULT_LOGFILE_SIZE) :
		_fname(fname), 
		_fnumber(0), 
		_lpolicy(policy), 
		_fsize(fsize), 
		_logfp(0),
		_type(type)
	{

		if (_queue == NULL)
		{
			_queue = new CMsgQueue();
			_queue->create();
		}
		if (_pth == 0)
		{
			if (pthread_create(&_pth, NULL, (void *(*)(void *))thread_func,
					NULL) != 0)
			{
				_pth = 0;
				printf("thread create error:%s\n", strerror(errno));
				return;
			}

		}
		if (_fsize <= 0)
		{
			_fsize = 100*1024;
		}
		_instance++;
	}
	~CLog()
	{
		_instance --;

		//          struct msqid_ds info;
		//           LOGITEM * item = NULL;
		//            do
		//            {
		//             memset(&info, 0, sizeof(info));
		//             msgctl(_queue->getQueue(), IPC_STAT, &info);
		//             usleep(10);
		//
		//             if (info.msg_qnum > 0)
		//             {
		//               if ((item = (LOGITEM *)_queue->receive()) == NULL)
		//               {
		//                   break;
		//               }
		//                item->logger->update(item);
		//             }
		//
		//            }while(info.msg_qnum > 0);

		if (_instance == 0)
		{
			pthread_cancel(_pth);
			_pth = 0;
			_queue->destroy();
			delete _queue;
		}

		if (_logfp != NULL)
		{
			fflush(_logfp);
			fclose(_logfp);
			_logfp = NULL;
		}
	}

	//build string send to msg queue.
	int print(int level, const char * file, const char * function, int line,
			const char * fmt, ...)
	{

		if (level > _logLevel)
		{
			return 0;
		}
		LOGITEM * item = new LOGITEM(this);
		struct timeval tm;
		gettimeofday(&tm, NULL);
		localtime_r(&(tm.tv_sec), &(item->logtime));

		int len = 0; // sprintf(item->txt, "%04d%02d%02d ", item->logtime.tm_year+1900,
		//                      item->logtime.tm_mon+1,
		//                     item->logtime.tm_mday);
		if (_type == DEBUG_LOG)
		{

			len = sprintf(item->txt, "[%s][%s][%d][%06lu][%06ld:%06ld][%d] ",
					file, function, line, (unsigned long)pthread_self(),
					(long)tm.tv_sec, (long)tm.tv_usec, level);
		}
		else
		{
			len = sprintf(item->txt, "%04d-%02d-%02d %02d:%02d:%02d:%03d  ",
					item->logtime.tm_year+1900, item->logtime.tm_mon+1,
					item->logtime.tm_mday, item->logtime.tm_hour,
					item->logtime.tm_min, item->logtime.tm_sec,
					((int)tm.tv_usec)/1000);
		}
		va_list body;
		va_start(body, fmt);
		len = vsnprintf(item->txt+len, LOG_BUF_SIZE-len, fmt, body);
		va_end(body);
		if (_type == DEBUG_LOG)
		{
           printf(item->txt);
		}
		if (!_queue->send(item))
		{
			return -1;
		}

		return len;
	}

	//write to file
	void update(LOGITEM * item)
	{

		if (_logfp == NULL)
		{
			if (!_createLogFile())
			{
				printf("create logfile error:%s\n", strerror(errno));
				_logfp = NULL;
				return;
			}
		}

		if (_lpolicy == SPLIT_ON_DATE)
		{
			//create a new file.
			if (item->logtime.tm_year != _curftime.tm_year
					|| item->logtime.tm_mon != _curftime.tm_mon
					|| item->logtime.tm_mday != _curftime.tm_mday)
			{
				fclose(_logfp);
				if (!_createDateLogFile(&(item->logtime)))
				{
					_logfp = NULL;
					return;
				}
			}
		}
		else if (_lpolicy == SPLIT_ON_SIZE)
		{
			//printf("%s", item->txt);
			size_t s = ftell(_logfp);
			while (s >= (size_t)_fsize)
			{
				fclose(_logfp);
				if (!_createSizeLogFile())
				{
					_logfp = NULL;
					return;
				}
                                s = ftell(_logfp);
			}

		}
		fprintf(_logfp, item->txt);
		fflush(_logfp);
		delete item;
	}
private:
	bool _createDateLogFile(const struct tm * t)
	{
		char fname[1024];
		if (_fname.c_str() == NULL)
		{
			sprintf(fname, "log%04d%02d%02d.log", t->tm_year+1900, t->tm_mon+1,
					t->tm_mday);
		}
		else
		{
			sprintf(fname, "%s%04d%02d%02d.log", _fname.c_str(), t->tm_year
					+1900, t->tm_mon+1, t->tm_mday);
		}
		mode_t m = umask(0000);
		if ((_logfp = fopen(fname, "a+")) == NULL)
		{

			printf("create log file error!!!!\n");
			umask(m);
			return false;
		}
		umask(m);
		_curftime.tm_year = t->tm_year;
		_curftime.tm_mon = t->tm_mon;
		_curftime.tm_mday = t->tm_mday;
		return true;
	}
	bool _createSizeLogFile()
	{
		char fname[1024];
		if (_fname.c_str() == NULL)
		{
			sprintf(fname, "log%04d.log", _fnumber);
		}
		else
		{
			sprintf(fname, "%s%04d.log", _fname.c_str(), _fnumber);
		}
		mode_t m = umask(0000);
		if ((_logfp = fopen(fname, "a+")) == NULL)
		{
			printf("create log file error!!!!\n");
			umask(m);
			return false;
		}
		umask(m);
		_fnumber++;
		return true;
	}
	bool _createLogFile()
	{

		if (_lpolicy == SPLIT_ON_DATE)
		{
			time_t t = time(NULL);
			return _createDateLogFile(localtime(&t));

		}
		else
		{
			return _createSizeLogFile();
		}
	}
private:

	static void * thread_func(void *)
	{
		LOGITEM * item= NULL;
		while (1)
		{
			if ((item = (LOGITEM *)_queue->receive()) == NULL)
			{
				break;
			}
			item->logger->update(item);
			pthread_testcancel();
		}
		return NULL;
	}
};

#endif /*LOG_H_*/
