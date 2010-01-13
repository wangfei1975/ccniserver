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
/*             libccni.cpp                                                             */
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
/*             2010-01-13     initial draft                                            */
/***************************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "libccni.h"

#include "log.h"
#include "ccni.h"
#include "ccni_msg.h"

const char * copyright = "\n"
    "CCNI client library 1.0.\n"
    "Copyright (C) bjwf 2008-2016. All rights reserved.\n"
    "bjwf2000@gmail.com  11-20-2008\n"
    "http://ccniserver.googlecode.com\n\n";

int CCNIConnection::connect(const char * serverip, int tport, int uport)
{
    CCNI_HEADER hd;
    struct sockaddr_in uad;

    memset(&uad, 0, sizeof(uad));
    uad.sin_family = AF_INET;
    uad.sin_port = htons(uport);
    uad.sin_addr.s_addr = inet_addr(serverip);
    LOGV("server ip:port is %s:%d\n", inet_ntoa(uad.sin_addr), ntohs(uad.sin_port));

    _udpsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sendto(_udpsock, &hd, sizeof(hd), 0, (struct sockaddr *)&uad, sizeof(uad)) != sizeof(hd))
    {
        LOGE("send error. %s\n", strerror(errno));
        disconnect();
        return -1;
    }
    socklen_t rlen = (socklen_t)sizeof(uad);
    if (recvfrom(_udpsock, &hd, sizeof(hd), 0, (struct sockaddr *)&uad, &rlen) != sizeof(hd))
    {
        LOGE("receive error.\n");
        disconnect();
        return -1;
    }

    _tcpsock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in tcpraddr;
    memset(&tcpraddr, 0, sizeof(tcpraddr));
    tcpraddr.sin_family = AF_INET;
    tcpraddr.sin_addr.s_addr = inet_addr(serverip);
    tcpraddr.sin_port = htons(tport);

    if (::connect(_tcpsock, (struct sockaddr *)&tcpraddr, sizeof(tcpraddr)) < 0)
    {
        LOGE("connect error. %s\n", strerror(errno));
        disconnect();
        return -1;
    }
    
    if (!create())
    {
        LOGE("create listener thread error.\n");
        disconnect();
        return -1;
    }
    return 0;
}

int CCNIConnection::disconnect()
{
    if (_tcpsock >= 0)
    {
        close(_tcpsock);
        _tcpsock = -1;
    }
    if (_udpsock >= 0)
    {
        close(_udpsock);
        _udpsock = -1;
    }
    return 0;
}

int CCNIConnection::login(const char * uname, const char * pwd)
{
    return 0;
}
int CCNIConnection::logout()
{
    return 0;
}

int CCNIConnection::enterroom(int roomid)
{
    return 0;
}
int CCNIConnection::leaveroom()
{
    return 0;
}

int CCNIConnection::state()
{
    return 0;
}
void CCNIConnection::doWork()
{
    while(1)
    {
        
    }
}
