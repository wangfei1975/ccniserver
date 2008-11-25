/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             ccni_xml.h                                                              */
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
/*             2008-11-25     initial draft                                            */
/***************************************************************************************/

#ifndef CCNI_XML_H_
#define CCNI_XML_H_

#include "log.h"
#include "xml.h"
#include "ccni.h"
class CCNIXmlMsg : public CXmlDoc
{
public:
    bool parse(char * strxml);
    
    bool dump(char * buf, int len);
};
#endif /*CCNI_XML_H_*/
