/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             ccni_msg.cpp                                                            */
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
/*             2008-11-26     initial draft                                            */
/***************************************************************************************/
#include "ccni_msg.h"

CCNIMsgParser::parse_state_t CCNIMsgParser::read(int sock)
{
    switch (_state)
    {
    case st_init:
        return _readInit(sock);
        break;
    case st_rdhd:
        return _readRdhd(sock);
        break;
    case st_hdok:
        return _readHdok(sock);
        break;
    case st_rdbd:
        return _readRdbd(sock);
        break;
    default:
        break;
    }
    return _state;
}

CCNIMsgParser::parse_state_t CCNIMsgParser::_readInit(int sock)
{
    int rlen = ::read(sock, &_hd, sizeof(_hd));
    if (rlen == EAGAIN)
    {
        return _state;
    }
    //rlen == 0 means remote closed connection
    if (rlen <= 0)
    {
       return (_state = st_rderror);
    }
    if (rlen < (int)sizeof(_hd))
    {
       _pos = rlen;
       return (_state = st_rdhd);
    }
     
    if (!_hd.verify())
    {
        return (_state = st_hderror);;
    }
    
    _data = new unsigned char[_hd.datalen];
    
    rlen = ::read(sock, _data, _hd.datalen);
    if (rlen == EAGAIN)
    {
        return (_state = st_rdbd);
    }
    if (rlen <= 0)
    {
        return (_state = st_rderror);
    }
    if (rlen < _hd.datalen)
    {
        _pos = rlen;
        return (_state = st_rdbd);
    }
    
    return (_state = st_bdok);
}
CCNIMsgParser::parse_state_t CCNIMsgParser::_readRdhd(int sock)
{
    return _state;
}
CCNIMsgParser::parse_state_t CCNIMsgParser::_readHdok(int sock)
{
    return _state;
}
CCNIMsgParser::parse_state_t CCNIMsgParser::_readRdbd(int sock)
{
    return _state;
}
