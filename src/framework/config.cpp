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
    
    login  = (nd.findChild(xmlLogLogin).getIntContent() == 1);
    logout = (nd.findChild(xmlLogLogout).getIntContent() == 1);
    
    LOGD("ccni server event log path: %s\n", path.c_str());
    LOGD("ccni server event log login: %s\n", login ? "True" : "False");
    LOGD("ccni server event log logiout: %s\n", logout ? "True" : "False");
    
    return true;
}

bool CConfig::create(const char * cfgfname)
{
    if (!_cfg.createFromFile(cfgfname))
    {
        LOGE("read configuration file %s error.\n", cfgfname);
        return false;
    }

    CXmlNode root = _cfg.getRoot();

    if (strcmp(root.name(), xmlCCNIServerConfiguration) != 0)
    {
        LOGE("error configuration file.\n");
        return false;
    }

    if (!_parseAddrLst(tcplst, root.findChild(xmlTCPListenIPList)))
    {
        LOGW("no valid tcp listen list found, using default!\n");
    }

    if (!_parseAddrLst(udplst, root.findChild(xmlUDPListenIPList)))
    {
        LOGW("no valid udp listen list found, using default!\n");
    }

    if (!logcfg.create(root.findChild(xmlLogConfig)))
    {
        LOGW("create log config error!\n");
    }
    LOGD("create configuration success.\n");
    return true;
}

bool CConfig::_parseAddrLst(addrlist_t & lst, CXmlNode nd)
{
    const char * ips;
    struct sockaddr_in addr;
    char ipbuf[32];
    int port;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    lst.clear();

    for (CXmlNode n = nd.child(); !n.isEmpty(); n = n.next())
    {
        if (n.type() != XML_ELEMENT_NODE)
        {
            continue;
        }
        if (strcmp(n.name(), xmlIP) == 0)
        {
            ips = n.content();
            const char * p = strstr(ips, ":");
            if (p == NULL)
            {
                LOGW("cfg item %s is not a valid IP:PORT string.\n", ips);
                continue;
            }
            strncpy(ipbuf, ips, 32);
            ipbuf[p-ips] = 0;

            if (sscanf(p+1, "%d", &port) != 1)
            {
                LOGW("cfg item %s not contain a valid port.\n", ips);
                continue;
            }

            if (inet_pton(AF_INET, ipbuf, &addr.sin_addr) < 0)
            {
                LOGW("ip addr %s is not avlid.\n", ips);
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

