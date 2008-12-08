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
class CConfig
{
private:
    CXmlDoc _cfg;
public:
    typedef vector<struct sockaddr_in> addrlist_t;

    addrlist_t tcplst;
    addrlist_t udplst;
    CLogConfig logcfg;
    string     secret;
    string     dburl;
    unsigned int        pool_threads;
    unsigned int        usr_listen_threads;
    unsigned int        login_timeout;
    unsigned int        secret_timeout;
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
#endif /*CONFIG_H_*/
