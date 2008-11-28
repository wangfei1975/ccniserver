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
#include "log.h"
#include "client.h"
#include "ccni_msg.h"

class CDataMgr
{
public:
public:
    CClient * findClient(const char * uname)
    {
        return NULL;
    }
    bool addClient(CClient * c)
    {
        return true;
    }
    void delClient(CClient * c)
    {
        
    }
    bool create()
    {
        return true;
    }
    void destroy()
    {
        
    }
};
#endif /*DATA_MGR_H_*/
