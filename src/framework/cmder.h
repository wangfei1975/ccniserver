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
/*             cmder.h                                                                 */
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
/*             2008-12-09     initial draft                                            */
/***************************************************************************************/
#ifndef CMDER_H_
#define CMDER_H_

#include "log.h"
#include "ccni.h"

class CCmder
{
private:
    int _sock;
public:
    bool create()
    {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(20080);
        if ((_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            LOGE("create commander sock error.\n");
            return false;
        }
        int opt = 1;
        if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            LOGE("set sockopt error %s\n", strerror(errno));
            return false;
        }
        if (bind(_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        {
            LOGE("bind socket to ip port error %s:%d!\n",
                    inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            return false;
        }
        return true;
    }

    void destroy()
    {
         close(_sock);
         _sock = -1;
    }

    void  dowork();
};
#endif /*CMDER_H_*/
