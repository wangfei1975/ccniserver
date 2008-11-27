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
#include "utils.h"
 bool CCNIMsgPacker::send(int sock)
 {
    uint8_t * buf = (uint8_t *)&_hd;
    tcp_write(sock, buf, 5);
    usleep(100);
    tcp_write(sock, buf+5, sizeof(_hd)-5);
    tcp_write(sock, _data, 2);
    usleep(100);
    tcp_write(sock, _data+2, _hd.datalen-2);
    return true;
    
 }
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
            LOGV("read EAGAIN\n");
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
        LOGV("stat:st_rdhd\n");
        return (_state = st_rdhd);
    }

    //read a error header
    if (!_hd.verify())
    {
        return (_state = st_hderror);;
    }

    //header ok, then read body.
    _data = new  char[_hd.datalen+1];
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
            LOGV("read EAGAIN\n");
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
        LOGV("stat:st_rdbd\n");
        return (_state = st_rdbd);
    }
    _data[_hd.datalen] = 0;
    return (_state = st_bdok);

}

