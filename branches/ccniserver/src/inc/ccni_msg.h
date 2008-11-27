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
class CCNIMsgPacker
{
public:
    bool create();
    bool appendmsg(const char * label, const char * content);
};

//read msg from net & parse.
class CCNIMsgParser
{
public:
    enum parse_state_t
    {
        st_rdhd,    //reading msg header, header is not ready. 
        st_rdbd,    //reading msg body.
        st_bdok,    //msg ok.
        st_rderror, //read socket error, socket closed or read error.
        st_hderror, //read a illegal header.
    };
private:
    CCNI_HEADER _hd;
    unsigned char * _data;
    parse_state_t _state;
    int _pos;
public:

    CCNIMsgParser():_data(NULL),_state(st_rdhd),_pos(0)
    {}
    ~CCNIMsgParser()
    {
        if (_data)
        {
            delete []_data;
            _data = NULL;
            _pos = 0;
            _state = st_rdhd;
        }
    }
public:
    /*
     * read data from non-blocking tcp sock fd
     *  
     * */
    parse_state_t read(int sock);
private:
 
    parse_state_t _readRdhd(int sock); 
     parse_state_t _readRdbd(int sock);
    
    
};
#endif /*CCNI_MSG_H_*/
