/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             conn_listener.h                                                         */
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
#ifndef CONN_LISTENER_H_
#define CONN_LISTENER_H_

#include <map>
#include "ccni.h"
#include "config.h"
#include "log.h"
#include "udp_listener.h"
#include "tcp_listener.h"

class CConListener
{
private:
    const CConfig     & _cfg;
    CThreadsPool      & _pool;
    CSecKeyMap          _keymap;
    CUdpListener        _udpListener;
    CTcpListener        _tcpListener;
public:
    CConListener(const CConfig & cfg, CThreadsPool & pool) :
        _cfg(cfg), _pool(pool), _udpListener(cfg, _keymap, pool), _tcpListener(cfg, _keymap, pool)
    
    {
        
    }
public:
    bool create();
    void destroy();
   
};
#endif /*CONN_LISTENER_H_*/
