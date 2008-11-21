/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             utils.h                                                                 */
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
/*             2008-06-05     initial draft                                            */
/***************************************************************************************/
#ifndef COMMON_UTILITY_H_
#define COMMON_UTILITY_H_

#include <libxml2/libxml/parser.h>
#include <openssl/md5.h>

extern void run_as_daemon(void (*handler)(int));
extern void check_unique_instance(const char * pidfname);

extern int tcp_read(int sockfd, void * buf, int len);
extern int tcp_write(int sockfd, void * buf, int len);

// xml parse utilities
extern xmlNodePtr get_xml_childnode(xmlNodePtr parent, const char * ndname);

extern int get_xml_node_intprop(xmlNodePtr nd, const char * propname);
extern const char * get_xml_node_strprop(xmlNodePtr nd, const char * propname);
extern const char * get_xml_node_content(xmlNodePtr nd);
// get xml node child's content.
extern const char * get_xml_node_strfield(xmlNodePtr nd, const char * fieldname);
//
extern int get_xml_node_intfield(xmlNodePtr nd, const char * fieldname);
extern bool get_xml_node_intfield(xmlNodePtr nd, const char * fieldname, int * value);

// md5 calc
extern void md5_calc(unsigned char * out, unsigned char * in, unsigned int len);

//
// in_addr_t ip to string
#define  strip(ip)  inet_ntoa(*((struct in_addr *) &ip))

#endif /*COMMON_UTILITY_H_*/
