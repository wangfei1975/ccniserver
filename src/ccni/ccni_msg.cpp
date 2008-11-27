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
    if (_state == st_rdhd)
    {
        return _readRdhd(sock);
    }
    if (_state == st_rdbd)
    {
        return _readRdbd(sock);
    }
    return _state;
}

CCNIMsgParser::parse_state_t CCNIMsgParser::_readRdhd(int sock)
{
    uint8_t * buf = (uint8_t *)&_hd;

    int rlen =:: read(sock, buf+_pos, sizeof(_hd)-_pos);
    if (rlen < 0)
    {
        if (errno == EAGAIN)
        {
            return _state;
        }
        return (_state = st_rderror);
    }
    //rlen == 0 means remote closed connection
    if (rlen == 0)
    {
        return (_state = st_rderror);
    }
    //hd not ready,need next read.
    if (rlen < ((int)sizeof(_hd)-_pos))
    {
        _pos += rlen;
        return (_state = st_rdhd);
    }

    //read a error header
    if (!_hd.verify())
    {
        return (_state = st_hderror);;
    }

    //header ok, then read body.
    _data = new unsigned char[_hd.datalen];
    _pos = 0;
    _state = st_rdbd;
    return _readRdbd(sock);
}

CCNIMsgParser::parse_state_t CCNIMsgParser::_readRdbd(int sock)
{
    int rlen =:: read(sock, _data+_pos, _hd.datalen-_pos);
    if (rlen < 0)
    {
        if (errno == EAGAIN)
        {
            return _state;
        }
        return (_state = st_rderror);
    }

    if (rlen == 0)
    {
        return (_state = st_rderror);
    }

    if (rlen < (_hd.datalen-_pos))
    {
        _pos += rlen;
        return (_state = st_rdbd);
    }
    return (_state = st_bdok);

}

