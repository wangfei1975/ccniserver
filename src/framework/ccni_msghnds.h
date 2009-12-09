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

typedef  int (CClient::*msghnd_t)(CXmlNode msg, CCNIMsgPacker & res, CBroadCaster & bd);
struct hndtable_t
{
    const char * label;
    msghnd_t     fun;
};
static hndtable_t msghnds[];

void procMsgs(CCNIMsgParser & msg, CCNIMsgPacker & res, CBroadCaster & bd);
int  doCCNI(CXmlNode msg, CCNIMsgPacker & res, CBroadCaster & bd);
int  doMyState(CXmlNode  msg, CCNIMsgPacker & res, CBroadCaster & bd);
int  doLogout(CXmlNode  msg, CCNIMsgPacker & res, CBroadCaster & bd);
int  doUnknow(CXmlNode  msg, CCNIMsgPacker & res, CBroadCaster & bd);
int  doEnterRoom(CXmlNode  msg, CCNIMsgPacker & res, CBroadCaster & bd);


#endif /*CCNI_MSGHNDS_H_*/
