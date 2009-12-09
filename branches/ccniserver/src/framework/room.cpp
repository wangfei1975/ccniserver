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
/*             room.cpp                                                                */
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
#include "room.h"

bool CRoom::enter(CClient * c, CBroadCaster & bd)
{
    usr_list_t::iterator it;
    CAutoMutex du(_lk);
    for (it = _usrlist.begin(); it != _usrlist.end(); ++it)
    {
        bd.insertaddr((*it)->udpsock(), (*it)->udpaddr());
    }
    return _usrlist.insert(c).second;
}

bool CRoom::leave(CClient * c, CBroadCaster & bd)
{
    usr_list_t::iterator it;
    CAutoMutex du(_lk);
    it = _usrlist.find(c);
    if (it != _usrlist.end())
    {
        _usrlist.erase(it);
        for (it = _usrlist.begin(); it != _usrlist.end(); ++it)
        {
            bd.insertaddr((*it)->udpsock(), (*it)->udpaddr());
        }
        return true;
    }
    return false;
}



