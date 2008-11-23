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

class CConListener
{
private:
    map<secret_key_t, struct sockaddr_in> _keymap;
public:
    bool create(const CConfig &cfg);
    void destroy();
   
};
#endif /*CONN_LISTENER_H_*/
