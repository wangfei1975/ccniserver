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
