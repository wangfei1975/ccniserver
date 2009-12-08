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
int CCNIMsgPacker::block_send(int sock)
{
    uint8_t * buf = (uint8_t *)&_hd;

    int slen = tcp_write(sock, buf, sizeof(_hd));
    if (slen < 0)
    {
        return slen;
    }
    return tcp_write(sock, _data, _hd.datalen);
}

CCNIMsgPacker::state_t CCNIMsgPacker::send(int sock)
{
    if (_state == st_sdhd)
    {
        return _sendsdhd(sock);
    }
    if (_state == st_sdbd)
    {
        return _sendsdbd(sock);
    }
    return _state;

}
CCNIMsgPacker::state_t CCNIMsgPacker::_sendsdhd(int sock)
{
    uint8_t * buf = (uint8_t *)&_hd;

    int slen =:: send(sock, buf+_pos, sizeof(_hd)-_pos, MSG_NOSIGNAL|MSG_DONTWAIT);
    if (slen <= 0)
    {
        if (errno == EAGAIN)
        {
            LOGV("send EAGAIN\n");
            return _state;
        }
        return (_state = st_sderror);
    }
    if (slen < (int)(sizeof(_hd)-_pos))
    {
        _pos += slen;
        return (_state = st_sdhd);
    }
    _pos = 0;
    _state = st_sdbd;
    return _sendsdbd(sock);
}
CCNIMsgPacker::state_t CCNIMsgPacker::_sendsdbd(int sock)
{
    int slen =:: send(sock, _data+_pos, _hd.datalen-_pos, MSG_NOSIGNAL|MSG_DONTWAIT);
    if (slen <= 0)
    {
        if (errno == EAGAIN)
        {
            LOGV("send EAGAIN\n");
            return _state;
        }
        return (_state = st_sderror);
    }
    if (slen < _hd.datalen-_pos)
    {
        _pos += slen;
        LOGV("send body..\n");
        return (_state = st_sdbd);
    }
    return (_state = st_sdok);
}

CCNIMsgParser::state_t CCNIMsgParser::read(int sock)
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

CCNIMsgParser::state_t CCNIMsgParser::_readRdhd(int sock)
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
    _data = new char[_hd.datalen+1];
    _pos = 0;
    _state = st_rdbd;
    return _readRdbd(sock);
}

CCNIMsgParser::state_t CCNIMsgParser::_readRdbd(int sock)
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
    return (_state = st_rdok);

}

