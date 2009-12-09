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
/*             conn_listener.h                                                         */
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
#ifndef CONN_LISTENER_H_
#define CONN_LISTENER_H_

#include <map>
#include "ccni.h"
#include "config.h"
#include "log.h"
#include "udp_listener.h"
#include "tcp_listener.h"

class CConListener
{
private:
    const CConfig     & _cfg;
    CThreadsPool      & _pool;
    CSecKeyMap          _keymap;
    CUdpListener        _udpListener;
    CTcpListener        _tcpListener;
public:
    CConListener(const CConfig & cfg, CThreadsPool & pool) :
        _cfg(cfg), _pool(pool), _keymap(cfg), _udpListener(cfg, _keymap, pool), _tcpListener(cfg, _keymap, pool)
    
    {
        
    }
public:
    bool create();
    void destroy();
   
};
#endif /*CONN_LISTENER_H_*/
