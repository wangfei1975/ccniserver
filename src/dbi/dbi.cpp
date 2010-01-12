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
/*             dbi.cpp                                                                 */
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

#include "dbi.h"
#include "log.h"

static const int SQL_BUFLEN = 2048;
static const int COL_ID = 0;
static const int COL_Name = 1;
static const int COL_NickName = 2;
static const int COL_Password = 3;
static const int COL_LastLoginTime = 4;
static const int COL_Score = 5;

const char * sqlCreateTable = "CREATE TABLE IF NOT EXISTS USER("
    "ID             INTEGER  CONSTRAINT PK_USERID PRIMARY KEY, "
    "Name           TEXT NOT NULL UNIQUE, "
    "NickName       TEXT NOT NULL, "
    "Password       TEXT NOT NULL, "
    "LastLoginTime  TEXT, "
    "Score          INTEGER        ); "
    "CREATE INDEX IF NOT EXISTS IDX_USER_NAME ON USER (Name ASC);";

const char * sqlAddUser = "INSERT INTO USER(ID, Name, NickName, Password, LastLoginTime, Score) "
    "VALUES(NULL, '%s', '%s', '%s', %d, %d);";

const char * sqlUpdateUserLoginTm = "UPDATE USER SET "
    "LastLoginTime = '%s' "
    "WHERE Name='%s';";

const char * sqlUpdateUser = "UPDATE USER SET "
    "NickName = '%s, "
    "Password = '%s', "
    "LastLoginTime = '%s', "
    "Score = '%d' "
    "WHERE Name='%s';";

const char * sqlSelectUser = "SELECT * FROM USER WHERE Name='%s';";

CDataBase::CDataBase() :
    _db(NULL)
{
}

CDataBase::~CDataBase()
{
    close();
}

int CDataBase::open(const char * url, const char * usr, const char * pwd)
{
    char * zErrMsg= NULL;
    if (sqlite3_open(url, &_db) != 0)
    {
        LOGE("Can't open database: %s\n", sqlite3_errmsg(_db));
        sqlite3_close(_db);
        _db = NULL;
        return -1;
    }

    CAutoMutex dumy(_lk);
    if (sqlite3_exec(_db, sqlCreateTable, NULL, 0, &zErrMsg) != SQLITE_OK)
    {
        LOGE("SQLite error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    return 0;
}
void CDataBase::close()
{
    if (_db)
    {
        sqlite3_close(_db);
        _db = NULL;
    }
}

int CDataBase::verifyUser(const char * name, const char * pwd, CRecord & rec)
{
    char sqlBuf[SQL_BUFLEN];
    //char *zErrMsg = NULL;
    sqlite3_stmt *pStmt= NULL;

    /* if (strlen(name) >= NameLen)
     {
     LOGINFO("name too long! %s", name);
     return -1;
     }
     if (strlen(pwd) >= PasswordLen)
     {
     LOGINFO("pwd too long! %s", pwd);
     return -1; 
     }*/

    _lk.lock();
    sprintf(sqlBuf, sqlSelectUser, name);
    int ret = sqlite3_prepare(_db, sqlBuf, -1, &pStmt, 0);
    if (ret != SQLITE_OK || pStmt == NULL)
    {
        ret = -1;
        LOGW("sqlite prepare error.\n");
        goto _End;
    }

    ret = sqlite3_step(pStmt);
    if (ret == SQLITE_ROW)
    {
        char * dbpwd = (char *)sqlite3_column_text(pStmt, COL_Password);
        if (dbpwd == NULL || strcmp(dbpwd, pwd) == 0)
        {
            const char * tmp = (const char *)sqlite3_column_text(pStmt, COL_Name);
            rec.name = tmp ? tmp : "";

            tmp = (const char *)sqlite3_column_text(pStmt, COL_NickName);
            rec.nicName = tmp ? tmp : "";

            rec.pwd = dbpwd ? dbpwd : "";

            tmp = (const char *)sqlite3_column_text(pStmt, COL_LastLoginTime);
            rec.lastLoginTime = tmp ? tmp : "";

            rec.score = sqlite3_column_int(pStmt, COL_Score);

            ret = 0;
        }
        else
        {
            LOGW("%s error pwd\n", name);
            ret = -1;
        }

    }
    else
    {
        LOGW("error user name %s\n", name);
        ret = -1;
    }

    _End: sqlite3_reset(pStmt);
    sqlite3_finalize(pStmt);
    _lk.unlock();
    return ret;
}

int CDataBase::updateUserLoginTime(const char * uname)
{
    char sqlbuf[SQL_BUFLEN];
    char stm[16];

    char * zErrMsg= NULL;

    struct timeval tmv;
    struct tm ltm;

    gettimeofday(&tmv, NULL);
    localtime_r(&(tmv.tv_sec), &ltm);

    //SYSTEMTIME tm;
    //GetLocalTime(&tm);
    sprintf(stm, "%04d%02d%02d%02d%02d%02d", ltm.tm_year+1900, ltm.tm_mon+1, ltm.tm_mday,
            ltm.tm_hour, ltm.tm_min, ltm.tm_sec);

    sprintf(sqlbuf, sqlUpdateUserLoginTm, stm, uname);
    CAutoMutex dumy(_lk);
    if (sqlite3_exec(_db, sqlbuf, NULL, 0, &zErrMsg) != SQLITE_OK)
    {
        LOGE("SQLite error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    return 0;
}
int CDataBase::updateUser(const CRecord * rec)
{
    char sqlbuf[SQL_BUFLEN];
    char * zErrMsg= NULL;

    sprintf(sqlbuf, sqlUpdateUser, rec->nicName.c_str(), rec->pwd.c_str(),
            rec->lastLoginTime.c_str(), rec->score, rec->name.c_str());
    CAutoMutex dumy(_lk);
    if (sqlite3_exec(_db, sqlbuf, NULL, 0, &zErrMsg) != SQLITE_OK)
    {
        LOGE("SQLite error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    return 0;
}

