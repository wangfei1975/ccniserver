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
/*             libccni.h                                                               */
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
#ifndef LIBCCNI_H_
#define LIBCCNI_H_

#include "thread.h"
#include "ccni_msg.h"




class CCNIListener
{
public:
    virtual int onmessage() = 0;
};

class CCNIConnection:public CThread
{
public:
       enum state_t
       {
           stOffline = 0,
           stOnline = 0x01,
           stIdle = 0x02,
           stSessional = 0x04,
           stReady = 0x08,
           stWatching = 0x10,
           stMoving = 0x20,
           stPondering = 0x40,
       };
       class CRoom
       {
       public:
           int id;
           string des;
           int capacity;
           int usrcount;
       };
       typedef list<CRoom> room_lst_t; 
public:
    CCNIConnection(CCNIListener * lis):_udpsock(-1),_tcpsock(-1),_listener(lis),_timeout(3000)
    {
        
    }
    ~CCNIConnection()
    {
        disconnect();
    }
public:
    bool connect(const char * serverip, int tport, int uport);
    void disconnect();
        
    int login(const char * uname, const char * pwd);
    int logout();
    
    int listrooms(room_lst_t & lst);
    int enterroom(int roomid);
    int leaveroom();
    
    int state();
    
    int ccni(string & ccnistr);
    
    
    
    string lasterr(){return _lasterr;}
public:
    virtual void doWork();
    
private:
    void doServerMessage(int sock);
    
    CCMsgQueue * getMsgQue(uint32_t seq);
    bool sendRes(uint32_t seq, CCNIMsgParser * res);
    
private:
    typedef map<uint32_t, CCMsgQueue *> event_lst_t;
     int _udpsock;
     int _tcpsock;
     CCNIListener * _listener;
     string _uname;
     string _passwd;
     
     CMutex      _evtlk;
     event_lst_t _evts;
     secret_key_t _k1, _k2;
       
     int _timeout;
     
     string _lasterr;
};
 
#endif /*LIBCCNI_H_*/
