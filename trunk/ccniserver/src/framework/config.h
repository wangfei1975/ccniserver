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
/*             config.h                                                                */
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

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>

#include "xml.h"

using namespace std;
class CLogConfig
{
public:
    string path;
    bool login;
    bool logout;

public:
    CLogConfig()
    {
    }
    CLogConfig(const CLogConfig & c) :
        path(c.path), login(c.login), logout(c.logout)
    {
    }
    bool create(CXmlNode nd);

};

class CRoomConfig
{
public:
    unsigned int id;
    unsigned int capacity;
    string description;
public:
    CRoomConfig() :
        id(0), capacity(255)
    {
        id = generateId();
        char buf[255];
        sprintf(buf, "Room%03d", id);
        description = buf;
    }
    CRoomConfig(const CRoomConfig & o) :
        id(o.id), capacity(o.capacity), description(o.description)
    {

    }
    bool create(CXmlNode nd);
private:
    static int generateId()
    {
        static int ids = 0;
        return ++ids;
    }
};

class CHallConfig
{
public:
    typedef vector <CRoomConfig> roomlist_t;
    roomlist_t  rooms;
    
    bool create(CXmlNode nd);
    
    
};
class CConfig
{
private:
    CXmlDoc _cfg;
public:
    typedef vector<struct sockaddr_in> addrlist_t;

    addrlist_t tcplst;
    addrlist_t udplst;
    CLogConfig logcfg;
    string secret;
    string dburl;
    unsigned int pool_threads;
    unsigned int usr_listen_threads;
    unsigned int login_timeout;
    unsigned int secret_timeout;
    CHallConfig  hallcfg;
public:
    CConfig() :
        _cfg(NULL), pool_threads(4), usr_listen_threads(1), login_timeout(5), secret_timeout(5)
    {
    }
    ~CConfig()
    {
        destroy();
    }

    bool create(const char * fname);
    void destroy();

private:

    bool _parseAddrLst(addrlist_t & lst, CXmlNode nd);

};
/*
 * xml labels in configuration file.
 * */
#define xmlHallConfig               "HallConfig"
#define xmlRoom                     "Room"
#define xmlId                       "Id"
#define xmlDescription              "Description"
#define xmlCapacity                 "Capacity"

#define xmlCCNIServerConfiguration  "CCNIServerConfiguration"
#define xmlTCPListenIPList          "TCPListenIPList"
#define xmlUDPListenIPList          "UDPListenIPList"
#define xmlIP                       "IP"
#define xmlLogConfig                "LogConfig"
#define xmlPath                     "Path"
#define xmlLogLogin                 "LogLogin"
#define xmlLogLogout                "LogLogout"
#define xmlDBPathName               "DBPathName"
#define xmlSecret                   "Secret"
#define xmlPoolThreadCount          "PoolThreadCount"
#define xmlUserListenThreadCount    "UserListenThreadCount"
#define xmlLoginTimeOut             "LoginTimeOut"
#define xmlSecretKeyTimeOut         "SecretKeyTimeOut"
#endif /*CONFIG_H_*/
