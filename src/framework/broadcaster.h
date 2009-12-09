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
/*            broadcaster.h                                                            */
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
#ifndef BROADCASTER_H_
#define BROADCASTER_H_
#include <map>
#include <list>
#include "log.h"
#include "ccni_msg.h"
using namespace std;
class CBroadCaster
{
public:
typedef vector<struct sockaddr_in> addr_list_t;
typedef map <CUdpSockData *, addr_list_t *> broadcast_t;
private:
     CCNIMsgPacker _msg;
     broadcast_t   _broadcaster;
public:
     void append(CXmlMsg msg)
     {
         _msg.appendmsg(msg);
     }
     bool insertaddr(CUdpSockData * udp, const struct sockaddr_in & addr)
     {
         broadcast_t::iterator it;
         it = _broadcaster.find(udp);
         if (it == _broadcaster.end())
         {
             addr_list_t * lst = new addr_list_t;
             lst->push_back(addr);
             _broadcaster[udp] = lst;
         }
         else
         {
             it->second->push_back(addr);
         }
         return true;
     }
     bool create()
     {
         if (!_msg.create())
         {
             return false;
         }
         return true;
     }
     
     void destroy()
     {
         _msg.free();
         broadcast_t::iterator it;
         for (it = _broadcaster.begin(); it != _broadcaster.end(); ++it)
         {
              delete (it->second);
         }
         
     }
    
public:
    
     
};
#endif /*BROADCASTER_H_*/
