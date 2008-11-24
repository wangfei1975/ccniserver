/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             conn_listener.cpp                                                       */
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
#include "conn_listener.h"

bool CConListener::create()
{

    if (_udpListener.create())
    {
        LOGE("create udp listener error!\n"); 
        return false;
    }
    
    if (_tcpListener.create())
    {
        LOGE("create tcp listener error!\n");
        return false;
    }
    
    return true;

    
}
void CConListener::destroy()
{
     _udpListener.destroy();
     _tcpListener.destroy();
}

