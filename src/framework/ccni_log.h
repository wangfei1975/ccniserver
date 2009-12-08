/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             ccni_log.h                                                              */
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
/*             2008-11-23     initial draft                                            */
/***************************************************************************************/

#ifndef CCNI_LOG_H_
#define CCNI_LOG_H_
#include "log.h"
#include "config.h"

class CCNILog : public CLog
{
private:
    const CLogConfig & _cfg;
public:
    //fixme: cfg.path was empty while the constructor invoked.
    //
    CCNILog(const CLogConfig & cfg) :
        CLog(cfg.path.c_str(), CLog::EVENT_LOG, CLog::SPLIT_ON_DATE), _cfg(cfg)
    {

    }
public:
};

#endif /*CCNI_LOG_H_*/
