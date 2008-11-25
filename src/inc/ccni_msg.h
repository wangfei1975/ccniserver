/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             ccni_msg.h                                                              */
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

#ifndef CCNI_MSG_H_
#define CCNI_MSG_H_
#include "ccni.h"
#include "xml.h"

//create ccni msg and write to net.
class CCNIMsgWriter
{
public:
    bool create();
    bool appendmsg(const char * label, const char * content);
};

//read msg from net & parse.
class CCNIMsgReader
{
private:
    CCNI_HEADER _hd;
    unsigned char * _data;
    
public:
    CCNIMsg()_data(NULL){}
    ~CCNIMsg()
    {
       delete []_data;
       _data = NULL;
    }
    bool create(int sockfd)
    {
        if (_data)
        {
            delete []_data;
        }
        //read header.
        //read _data
        return true;
    }
    // pack from xml msg
    
    // generate xml msg 
};
#endif /*CCNI_MSG_H_*/
