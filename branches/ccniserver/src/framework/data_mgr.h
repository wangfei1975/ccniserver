/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             data_mgr.h                                                              */
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
#ifndef DATA_MGR_H_
#define DATA_MGR_H_
#include <map>
#include <vector>
#include <string>
#include "log.h"
#include "mutex.h"
#include "client.h"
#include "ccni_msg.h"
#include "room.h"

class CDataMgr
{
public:
    typedef vector <CRoom> room_list_t;
    typedef map<string, CClientPtr> client_list_t;
private:
    client_list_t _list;
    CMutex _listlk;
public:
    int userCount()
    {
        CAutoMutex du(_listlk);
        return _list.size();
    }
    CClientPtr findClient(const char * uname)
    {
        CAutoMutex du(_listlk);
        client_list_t::iterator it = _list.find(uname);
        if (it == _list.end())
        {
            return CClientPtr();
        }
        return (it->second);
    }
    bool addClient(CClientPtr c)
    {
        CAutoMutex du(_listlk);
        client_list_t::iterator it = _list.find(c->uname());
        if (it != _list.end())
        {
            return false;
        }
        _list[c->uname()] = c;
        return true;
    }
    void delClient(const char * uname)
    {
        CAutoMutex du(_listlk);
        client_list_t::iterator it = _list.find(uname);
        if (it == _list.end())
        {
            LOGW("not find user %s.\n", uname);
            return;
        }
        _list.erase(it);
    }
    bool create()
    {
        return true;
    }
    void destroy()
    {
        client_list_t::iterator it;
        CAutoMutex du(_listlk);
        for (it = _list.begin(); it != _list.end(); ++it)
        {
          //  delete (it->second);
        }
        _list.clear();
    }
};
#endif /*DATA_MGR_H_*/
