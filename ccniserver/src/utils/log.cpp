/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             log.cpp                                                                 */
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
#include "log.h"
static const char * dbg_fname = "./logs/ccni.dbg.";

CLog dbgLog(dbg_fname, CLog::DEBUG_LOG);
int abcde = 55;
int CLog::_instance = 0;

pthread_t CLog::_pth = 0;

CMsgQueue * CLog::_queue= NULL;

CLog::CLog(const char * fname, LOGTYPE type, LOGFILE_CFG policy, int fsize) :
	_fname(fname), _fnumber(0), _lpolicy(policy), _fsize(fsize), _logfp(0), _type(type)
{
#ifdef LOG_LEVEL
	_logLevel = LOG_LEVEL;
#else
	_logLevel = 5;
#endif
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
CLog::~CLog()
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
int CLog::print(int level, const char * file, const char * function, int line, const char * fmt, ...)
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

		len = sprintf(item->txt, "[%s][%s][%d][%06lu][%06ld:%06ld][%d] ", file, function, line,
				(unsigned long)pthread_self(), (long)tm.tv_sec, (long)tm.tv_usec, level);
	}
	else
	{
		len = sprintf(item->txt, "%04d-%02d-%02d %02d:%02d:%02d:%03d  ", item->logtime.tm_year+1900,
				item->logtime.tm_mon+1, item->logtime.tm_mday, item->logtime.tm_hour, item->logtime.tm_min,
				item->logtime.tm_sec, ((int)tm.tv_usec)/1000);
	}
	va_list body;
	va_start(body, fmt);
	len = vsnprintf(item->txt+len, LOG_BUF_SIZE-len, fmt, body);
	va_end(body);

	if (!_queue->send(item))
	{
		return -1;
	}

	return len;
}

void CLog::update(LOGITEM * item)
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
		if (item->logtime.tm_year != _curftime.tm_year || item->logtime.tm_mon != _curftime.tm_mon
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
	if (_type == DEBUG_LOG)
	{
	     printf(item->txt);
	}
	fprintf(_logfp, item->txt);
	fflush(_logfp);
	delete item;
}

bool CLog::_createDateLogFile(const struct tm * t)
{
	char fname[1024];
	if (_fname.c_str() == NULL)
	{
		sprintf(fname, "log%04d%02d%02d.log", t->tm_year+1900, t->tm_mon+1, t->tm_mday);
	}
	else
	{
		sprintf(fname, "%s%04d%02d%02d.log", _fname.c_str(), t->tm_year +1900, t->tm_mon+1, t->tm_mday);
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

bool CLog::_createSizeLogFile()
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

bool CLog::_createLogFile()
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

void * CLog::thread_func(void *)
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

