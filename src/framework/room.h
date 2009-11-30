/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             room.h                                                                  */
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
#ifndef ROOM_H_
#define ROOM_H_
#include <string>
#include <set>
#include "client.h"
#include "log.h"
#include "broadcaster.h"
 
class CRoom
{
public:
    typedef set <CClient *> usr_list_t;
private:
    int _capacity;
    int _id;
    CMutex _lk;
    usr_list_t _usrlist;
public:
    bool enter(CClient * c, CBroadCaster & bd);
     
    bool leave(CClient * c, CBroadCaster & bd);
     
};

#endif /*ROOM_H_*/
