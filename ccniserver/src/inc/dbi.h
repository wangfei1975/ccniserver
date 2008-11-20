/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             dbi.h                                                                   */
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
/*             2008-11-20     initial draft                                            */
/***************************************************************************************/

#ifndef _DBI_H_
#define _DBI_H_

#include <sqlite3.h>
#include <string>
#include <time.h>
#include <sys/time.h>

#include "mutex.h"

#define CCNI_DB_NAME   "DataBase/ccniDataBase.db"
/*
 *   idea of database interface.
 *    . just a very simple wrapper of sqlite3, aims to abstrace db details.
 *    . multi-threads safe.
 * */

namespace CCNIDataBase
{

    const int sqlBufLen   = 2048;
    const int NameLen     = 40;
    const int PasswordLen = 40;
    const int TimeLen     = 16;
    struct CRecord
    {
        std::string name;
        std::string pwd;
        std::string nicName;
        std::string lastLoginTime;
        int  score;
        CRecord():score(0){}
    };

    class CDataBase
    {
    protected:
        CMutex _lk;
        sqlite3 * _db;
    public:
        CDataBase();
        ~CDataBase();

        int  open(const char  * url = CCNI_DB_NAME, const char * usr = NULL, const char * pwd = NULL);
        void close();

        int  verifyUser(const char * name, const char * pwd, CRecord & rec);
        int  updateUser(const CRecord * rec);
        int  updateUserLoginTime(const char * uname);

    };
}

#endif    /*end _DBI_H_*/

