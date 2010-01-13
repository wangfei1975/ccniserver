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
/*             ccni_msghnds.h                                                          */
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
#ifndef CCNI_MSGHNDS_H_
#define CCNI_MSGHNDS_H_

typedef  int (CClient::*msghnd_t)(CXmlNode msg);
struct hndtable_t
{
    const char * label;
    state_t      vstate; // valid state of this ccni message.
    msghnd_t     fun;
};
static hndtable_t msghnds[];

void procMsgs(CCNIMsgParser & msg);

#define DECLARE_MSG_HANDLE(hname) int hname(CXmlNode  msg)

DECLARE_MSG_HANDLE(doCCNI);
DECLARE_MSG_HANDLE(doMyState);
DECLARE_MSG_HANDLE(doLogout);
DECLARE_MSG_HANDLE(doUnknow);
DECLARE_MSG_HANDLE(doEnterRoom);
DECLARE_MSG_HANDLE(doLeaveRoom);
DECLARE_MSG_HANDLE(doNewSession);
DECLARE_MSG_HANDLE(doEnterSession);
DECLARE_MSG_HANDLE(doWatchSession);
DECLARE_MSG_HANDLE(doReady);
DECLARE_MSG_HANDLE(doMove);
DECLARE_MSG_HANDLE(doDraw);
DECLARE_MSG_HANDLE(doGiveUp);
DECLARE_MSG_HANDLE(doListRooms);
DECLARE_MSG_HANDLE(doListSessions);
DECLARE_MSG_HANDLE(doSendMessages);
bool checkstate(CXmlNode msg, state_t vstate);
#endif /*CCNI_MSGHNDS_H_*/
