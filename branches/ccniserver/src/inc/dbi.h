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

/*
 *   idea of database interface.
 *    . just a very simple wrapper of sqlite3, aims to abstrace db details.
 *    . multi-threads safe.
 * */

struct CRecord
{
    std::string name;
    std::string pwd;
    std::string nicName;
    std::string lastLoginTime;
    int score;
    CRecord() :
        score(0)
    {
    }
};

class CDataBase
{
protected:
    CMutex _lk;
    sqlite3 * _db;
public:
    CDataBase();
    ~CDataBase();

    int open(const char * url, const char * usr = NULL, const char * pwd = NULL);
    void close();

    int verifyUser(const char * name, const char * pwd, CRecord & rec);
    int updateUser(const CRecord * rec);
    int updateUserLoginTime(const char * uname);

};

#endif    /*end _DBI_H_*/

