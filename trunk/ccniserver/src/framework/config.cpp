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
/*             config.cpp                                                              */
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
#include "log.h"
#include "config.h"
#include "utils.h"

bool CLogConfig::create(CXmlNode nd)
{
    if (nd.findChild(xmlPath).getContent(path).empty())
    {
        path = get_executable_path();
        path += "logs/";
    }
    if (path[0] != '/')
    {
        path = get_executable_path() + path;
    }

    path += "ccni.evt.";

    login = (nd.findChild(xmlLogLogin).getIntContent() == 1);
    logout = (nd.findChild(xmlLogLogout).getIntContent() == 1);

    LOGD("ccni server event log path: %s\n", path.c_str());
    LOGD("ccni server event log login: %s\n", login ? "True" : "False");
    LOGD("ccni server event log logiout: %s\n", logout ? "True" : "False");

    return true;
}

bool CRoomConfig::create(CXmlNode nd)
{
    int v = nd.findChild(xmlId).getIntContent();
    if (v > 0)
    {
        id = v;

    }
    v = nd.findChild(xmlCapacity).getIntContent();
    if (v > 0)
    {
        capacity = v;
    }
    nd.findChild(xmlDescription).getContent(description);
    LOGD("room id %d\n", id);
    LOGD("room capacity %d\n", capacity);
    LOGD("room description:%s\n", description.c_str());
    return true;
}
bool CHallConfig::create(CXmlNode nd)
{
    for (CXmlNode n = nd.child(); !n.isEmpty(); n = n.next())
    {
        if (n.type() != XML_ELEMENT_NODE)
        {
            continue;
        }
        LOGD("%s\n", n.name());
        if (strcmp(n.name(), xmlRoom) ==0)
        {
            CRoomConfig rcfg;
            if (rcfg.create(n))
            {
                rooms.push_back(rcfg);
            }
        }
    }
    return (rooms.size() > 0);
}

bool CConfig::create(const char * cfgfname)
{
    if (!_cfg.createFromFile(cfgfname))
    {
        LOGE("read configuration file %s error.\n", cfgfname);
        return false;
    }

    CXmlNode root = _cfg.getRoot();
    LOGD("cfg file root name:%s\n", root.name());
    if (strcmp(root.name(), xmlCCNIServerConfiguration) != 0)
    {
        LOGE("error configuration file.\n");
        return false;
    }
    LOGD("parse tcp addr list start.\n");
    if (!_parseAddrLst(tcplst, root.findChild(xmlTCPListenIPList)))
    {
        LOGW("no valid tcp listen list found, using default!\n");
    }
    LOGD("parse udp addr list start.\n");
    if (!_parseAddrLst(udplst, root.findChild(xmlUDPListenIPList)))
    {
        LOGW("no valid udp listen list found, using default!\n");
    }

    if (!logcfg.create(root.findChild(xmlLogConfig)))
    {
        LOGW("create log config error!\n");
    }

    if (root.findChild(xmlDBPathName).getContent(dburl).empty())
    {
        dburl = get_executable_path();
        dburl += "db/ccni.db";
    }
    if (dburl[0] != '/')
    {
        dburl = get_executable_path() + dburl;
    }

    if (root.findChild(xmlSecret).getContent(secret).empty())
    {
        LOGW("no scret in config file, using default secret.\n")
        secret = "hello";
    }

    pool_threads = root.findChild(xmlPoolThreadCount).getIntContent();
    if (pool_threads == 0)
    {
        LOGW("no valid pool threads count in conf file. using default value.\n");
        pool_threads = 4;
    }

    usr_listen_threads = root.findChild(xmlUserListenThreadCount).getIntContent();
    if (usr_listen_threads == 0)
    {
        LOGW("no valid usr listen threads count in conf file. using default value.\n");
        usr_listen_threads = 1;
    }

    login_timeout = root.findChild(xmlLoginTimeOut).getIntContent();
    if (login_timeout == 0)
    {
        LOGW("no valid login timeout in conf file. using default value.\n");
        login_timeout = 5;
    }

    secret_timeout = root.findChild(xmlSecretKeyTimeOut).getIntContent();
    if (secret_timeout == 0)
    {
        LOGW("no valid secret timeout in conf file. using default value.\n");
        secret_timeout = 5;
    }

    if (!hallcfg.create(root.findChild(xmlHallConfig)))
    {
        LOGW("create Hall config error!\n");
    }

    LOGD("create configuration success.\n");

    return true;
}

bool CConfig::_parseAddrLst(addrlist_t & lst, CXmlNode nd)
{

    string ips;
    struct sockaddr_in addr;
    char ipbuf[32];
    int port;

    if (nd.isEmpty())
    {
        return false;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    lst.clear();

    for (CXmlNode n = nd.child(); !n.isEmpty(); n = n.next())
    {
        if (n.type() != XML_ELEMENT_NODE)
        {
            continue;
        }
        LOGD("%s\n", n.name());
        if (strcmp(n.name(), xmlIP) == 0)
        {
            n.getContent(ips);
            LOGD("%s\n", ips.c_str());
            const char * p = strstr(ips.c_str(), ":");
            if (p == NULL || (p-ips.c_str()) > 16)
            {
                LOGW("cfg item %s is not a valid IP:PORT string.\n", ips.c_str());
                continue;
            }
            strncpy(ipbuf, ips.c_str(), 32);
            ipbuf[p-ips.c_str()] = 0;

            if (sscanf(p+1, "%d", &port) != 1)
            {
                LOGW("cfg item %s not contain a valid port.\n", ips.c_str());
                continue;
            }
            LOGD("port is %d\n", (port));
            if (inet_pton(AF_INET, ipbuf, &addr.sin_addr) < 0)
            {
                LOGW("ip addr %s is not avlid.\n", ips.c_str());
                continue;
            }
            addr.sin_port = htons((uint16_t)port);
            lst.push_back(addr);
            LOGD("IP list %d is %s:%d\n", lst.size(), ipbuf, port);
        }

    }
    return (lst.size() > 0);
}

void CConfig::destroy()
{
    _cfg.free();
}

