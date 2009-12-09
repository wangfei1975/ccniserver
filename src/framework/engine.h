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
#include "counter.h"
#include "cmder.h"
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
    CCounter  _counter;
    CCmder    _cmder;
    CEngine() :
        _conLiser(_cfg, _pool), _lg(_cfg.logcfg)
    {

    }
public:
    CCounter & counter()
    {
        return _counter;
    }
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
       _cmder.dowork();
        
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
        if (!_cmder.create())
        {
            LOGE("create command listener error.\n")
            return false;
        }
        LOGI("CCNI engine create success.\n");
        return true;
    }
    void destroy()
    {
        _cmder.destroy();
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
