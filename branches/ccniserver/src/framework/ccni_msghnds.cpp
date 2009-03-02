/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             ccni_msghnds.cpp                                                        */
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

#include "client.h"
#include "engine.h"
#include "broadcaster.h"

CClient::hndtable_t CClient::msghnds[] =
{
{ xmlTagCCNI, &CClient::doCCNI },
{ xmlTagMyState, &CClient::doMyState },
{ xmlTagLogout, &CClient::doLogout },
{ xmlTagEnterRoom, &CClient::doEnterRoom},
{ NULL, &CClient::doUnknow } 
};

void CClient::procMsgs(CCNIMsgParser & pmsg, CCNIMsgPacker & res, CBroadCaster & bd)
{

    int i;
    for (CXmlNode msg = pmsg.getfirst(); !msg.isEmpty(); msg = msg.next())
    {
        if (msg.type() != XML_ELEMENT_NODE)
        {
            continue;
        }
        for (i = 0; msghnds[i].label != NULL; i++)
        {
            if (strcmp(msghnds[i].label, msg.name()) == 0)
            {
                break;
            }
        }
        if ((this->*msghnds[i].fun)(msg, res, bd) < 0)
        {
            break;
        }
    }

    const CCNI_HEADER & hd(pmsg.header());

    if (!res.pack(hd.seq, hd.udata, hd.secret1, hd.secret2))
    {
        LOGE("pack error..\n");
    }

}
int CClient::doEnterRoom(CXmlNode msg, CCNIMsgPacker & res, CBroadCaster & bd)
{
    return 0;
}
int CClient::doUnknow(CXmlNode msg, CCNIMsgPacker & res, CBroadCaster & bd)
{
    CXmlMsg lgmsg;
    lgmsg.create(xmlTagUnknowMessage);
    res.appendmsg(lgmsg);
    return 0;
}
int CClient::doCCNI(CXmlNode msg, CCNIMsgPacker & res, CBroadCaster & bd)
{
    // LOGV("enter\n");

    CXmlMsg lgmsg, svrinfo;
    lgmsg.create(xmlTagCCNIRes);
    svrinfo.create(xmlTagSeriverInformation);

    svrinfo.addParameter(xmlTagServerType, "CCNIServer");
    svrinfo.addParameter(xmlTagServerVersion, "1.0");
    svrinfo.addParameter(xmlTagCCNIVersion, "1.0");
    svrinfo.addParameter(xmlTagDescription, "CCNI Chinese Chess Netword server 1.0 for Linux");
    lgmsg.addChild(svrinfo);
    res.appendmsg(lgmsg);
    // LOGV("out\n");
    return 0;
}
int CClient::doMyState(CXmlNode msg, CCNIMsgPacker & res, CBroadCaster & bd)
{
    CXmlMsg lgmsg;
    lgmsg.create(xmlTagMyStateRes);
    lgmsg.addParameter(xmlTagState, strstate());
    res.appendmsg(lgmsg);
    return 0;
}
int CClient::doLogout(CXmlNode msg, CCNIMsgPacker & res, CBroadCaster & bd)
{
    return 0;
}