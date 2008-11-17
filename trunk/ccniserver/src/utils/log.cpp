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

int CLog::_instance = 0;

pthread_t CLog::_pth = 0;

CMsgQueue * CLog::_queue = NULL;

#ifdef LOG_LEVEL
int CLog::_logLevel = LOG_LEVEL;
#else
int CLog::_logLevel = 5;
#endif

//CLog debugLog("./logs/deubg", CLog::DEBUG_LOG);
//CLog eventLog("./logs/APN01", CLog::EVENT_LOG, CLog::SPLIT_ON_DATE);
//CLog usrLog("./logs/user.trace", CLog::EVENT_LOG, CLog::NO_SPLIT);
