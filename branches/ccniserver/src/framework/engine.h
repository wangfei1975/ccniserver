/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             engine.h                                                                */
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

#ifndef ENGINE_H_
#define ENGINE_H_

#include "user_listener.h"
#include "conn_listener.h"
#include "seckey_map.h"
#include "config.h"
#include "utils.h"
#include "ccni_log.h"
#include "data_mgr.h"
#include "dbi.h"
class CEngine
{
private:
    CUserListener _usrLiser;
    CConListener _conLiser;
    CConfig _cfg;
    CCNILog _lg;
    CThreadsPool _pool;
    CDataMgr _dataMgr;
    CDataBase _dbi;
    CEngine() :
        _conLiser(_cfg, _pool), _lg(_cfg.logcfg)
    {

    }
public:
    CThreadsPool & threadsPool()
    {
        return _pool;
    }
    CDataMgr & dataMgr()
    {
        return _dataMgr;
    }
    CDataBase & db()
    {
        return _dbi;
    }
    CCNILog & logger()
    {
        return (_lg);
    }
    CUserListener & usrListener()
    {
        return (_usrLiser);
    }
    CConListener & conListener()
    {
        return (_conLiser);
    }
    CConfig & config()
    {
        return _cfg;
    }
    
public:

    void loop()
    {
        _pool.join();
    }
    bool create(const char * cfgfname)
    {
        if (!_cfg.create(cfgfname))
        {
            LOGE("create server configuration error!\n");
            return false;
        }

        if (!_pool.create(_cfg.pool_threads))
        {
            LOGE("create threads pool error.\n");
            return false;
        }
        LOGD("create %d threads in threads pool\n", _cfg.pool_threads);
        if (!_dataMgr.create())
        {
            LOGE("create data manager error.\n");
            return false;
        }
        
        if (_dbi.open(_cfg.dburl.c_str()) < 0)
        {
            LOGE("open data base %s error.\n", _cfg.dburl.c_str());
            return false;
        }
        
        if (!_conLiser.create())
        {
            LOGE("create connection listener error.\n")
            return false;
        }

        if (!_usrLiser.create(_cfg))
        {
            LOGE("create user listener error.\n");
            return false;
        }
        LOGI("CCNI engine create success.\n");
        return true;
    }
    void destroy()
    {
       
        _usrLiser.destroy();
        _conLiser.destroy();
        _pool.destroy();
        
        _dataMgr.destroy();
        _dbi.close();


    }
public:
    static CEngine & instance()
    {
        static CEngine eng;
        return eng;
    }

};
#endif /*ENGINE_H_*/
