/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             counter.h                                                               */
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
#ifndef COUNTER_H_
#define COUNTER_H_
#include "mutex.h"
#include "log.h"
class CCounter
{
private:
    volatile uint32_t _msgcnt;

    volatile uint32_t _logincnt;

    volatile uint32_t _udpacccnt;

public:
    CCounter() :
        _msgcnt(0), _logincnt(0), _udpacccnt(0)
    {

    }
    uint32_t udpCount()
    {
        return _udpacccnt;
    }

    uint32_t msgCount()
    {
        return _msgcnt;
    }
    uint32_t loginCount()
    {
        return _logincnt;
    }
    uint32_t incMsgCnt()
    {
        __gnu_cxx::__atomic_add_dispatch((_Atomic_word*)&_msgcnt, 1);
        return _msgcnt;
    }
    uint32_t incLoginCnt()
    {
        __gnu_cxx::__atomic_add_dispatch((_Atomic_word*)&_logincnt, 1);
        return _logincnt;
    }
    uint32_t incUdpCount()
    {
        __gnu_cxx::__atomic_add_dispatch((_Atomic_word*)&_udpacccnt, 1);
        return _udpacccnt;
    }
};
#endif /*COUNTER_H_*/
