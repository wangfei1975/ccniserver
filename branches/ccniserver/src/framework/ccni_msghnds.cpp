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
#include "notifier.h"

#define IMPL_MSG_HANDLE(name) int CClient::name(CXmlNode  msg, CCNIMsgPacker & res, CNotifier & notifier, CBroadCaster & bd)

CClient::hndtable_t CClient::msghnds[] =
{
{ xmlTagCCNI, &CClient::doCCNI },
{ xmlTagMyState, &CClient::doMyState },
{ xmlTagLogout, &CClient::doLogout },
{ xmlTagEnterRoom, &CClient::doEnterRoom },
{ xmlTagLeaveRoom, &CClient::doLeaveRoom },
{ xmlTagNewSession, &CClient::doNewSession },
{ xmlTagEnterSession, &CClient::doEnterSession },
{ xmlTagWatchSession, &CClient::doWatchSession },
{ xmlTagReady, &CClient::doReady },
{ xmlTagMove, &CClient::doMove },
{ xmlTagDraw, &CClient::doDraw },
{ xmlTagGiveUp, &CClient::doGiveUp },
{ xmlTagListRooms, &CClient::doListRooms },
{ xmlTagListSessions, &CClient::doListSessions },
{ xmlTagSendMessage, &CClient::doSendMessages },
{ NULL, &CClient::doUnknow } };

void CClient::procMsgs(CCNIMsgParser & pmsg, CCNIMsgPacker & res, CNotifier & notifier, CBroadCaster & bd)
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
        if ((this->*msghnds[i].fun)(msg, res, notifier, bd) < 0)
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

/*
 * 
 *   Message handles
 * 
 * 
 * */

IMPL_MSG_HANDLE(doEnterRoom)
{
    return 0;
}

IMPL_MSG_HANDLE(doLeaveRoom)
{
    return 0;
}

IMPL_MSG_HANDLE(doNewSession)
{
    return 0;
}

IMPL_MSG_HANDLE(doEnterSession)
{
    return 0;
}

IMPL_MSG_HANDLE(doWatchSession)
{
    return 0;
}

IMPL_MSG_HANDLE(doReady)
{
    return 0;
}

IMPL_MSG_HANDLE(doMove)
{
    return 0;
}

IMPL_MSG_HANDLE(doDraw)
{
    return 0;
}

IMPL_MSG_HANDLE(doGiveUp)
{
    return 0;
}
IMPL_MSG_HANDLE(doListRooms)
{
    return 0;
}

IMPL_MSG_HANDLE(doListSessions)
{
    return 0;
}

IMPL_MSG_HANDLE(doSendMessages)
{
    return 0;
}

IMPL_MSG_HANDLE(doUnknow)
{
    CXmlMsg lgmsg;
    lgmsg.create(xmlTagUnknowMessage);
    res.appendmsg(lgmsg);
    return 0;
}

IMPL_MSG_HANDLE(doCCNI)
{
    // LOGV("enter\n");
    CXmlMsg lgmsg, svrinfo;
    lgmsg.create(xmlTagCCNIRes);
    svrinfo.create(xmlTagSeriverInformation);

    svrinfo.addParameter(xmlTagServerType, "CCNIServer");
    svrinfo.addParameter(xmlTagServerVersion, "1.0");
    svrinfo.addParameter(xmlTagCCNIVersion, "1.0");
    svrinfo.addParameter(xmlTagDescription, "CCNI Chinese Chess server 1.0 for Linux");
    lgmsg.addChild(svrinfo);
    res.appendmsg(lgmsg);
    // LOGV("out\n");
    return 0;
}
IMPL_MSG_HANDLE(doMyState)
{
    CXmlMsg lgmsg;
    lgmsg.create(xmlTagMyStateRes);
    lgmsg.addParameter(xmlTagState, strstate());
    res.appendmsg(lgmsg);
    return 0;
}
IMPL_MSG_HANDLE(doLogout)
{
    CXmlMsg lgmsg;
    lgmsg.create(xmlTagLogoutRes);
    lgmsg.addParameter(xmlTagReturnCode, 0);
    res.appendmsg(lgmsg);
    return 0;
}
