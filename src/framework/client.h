/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             client.h                                                                */
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

#ifndef CLINET_H_
#define CLINET_H_
#include "log.h"
#include "ccni_msg.h"

class CClient
{
private:
    int _sock;
    struct sockaddr_in _udpaddr;
    secret_key_t _k1, _k2;
    
    
public:
    int sock(){return _sock;}
   
};
#endif /*CLINET_H_*/
