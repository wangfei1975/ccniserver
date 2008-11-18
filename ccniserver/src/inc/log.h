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

#include "msgqueue.h"
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

	CLog(const char * fname = NULL, 
		 LOGTYPE type = DEBUG_LOG, 
		 LOGFILE_CFG policy = SPLIT_ON_SIZE,
		 int fsize = DEFAULT_LOGFILE_SIZE);
	
	~CLog();

	//build string send to msg queue.
	int print(int level, const char * file, const char * function, int line, const char * fmt, ...);

	//write to file
	void update(LOGITEM * item);

private:


	static pthread_t _pth;
	static CMsgQueue * _queue;
	static int _instance;

private:
	std::string _fname;   //log file name
	int _fnumber;         //file number  the exact file name is : fnameXXXX.log
	LOGFILE_CFG _lpolicy; //how we split the log file.
	int _fsize;           //if split in fixed size
	FILE * _logfp;
	struct tm _curftime;  //used for split on date
	LOGTYPE _type;
	CMutex _lock;
	int _logLevel;

private:
	bool _createDateLogFile(const struct tm * t);
	bool _createSizeLogFile();
	bool _createLogFile();
private:

	static void * thread_func(void *);
};

#endif /*LOG_H_*/
