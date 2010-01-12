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
/*            broadcaster.cpp                                                          */
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
#include "udp_listener.h"

bool CBroadCaster::run()
{
    CNotifyMsgBufPtr bufptr = _msg.packNotification();

    if (bufptr->len() > 0)
    {
        broadcast_t::iterator it;
        for (it = _broadcaster.begin(); it != _broadcaster.end(); ++it)
        {
            addr_list_t * lst =  (it->second);
            CUdpSockData * sock = (it->first);
            addr_list_t::iterator addrit;

            for (addrit = lst->begin(); addrit != lst->end(); ++addrit)
            {
                if (!sock->sendto(bufptr->data(), bufptr->len(), &(*addrit), sizeof(*addrit)))
                {
                    LOGW("broad cast send to error  %s.\n", strerror(errno));
                }
            }
        }
    }
    delete this;
    return true;
}
