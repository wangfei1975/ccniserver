/*
  Copyright (C) 2009  Wang Fei (bjwf2000@gmail.com)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Generl Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>


//return executable binary path of current process.
extern const char * get_executable_path();
extern void run_as_daemon(void (*handler)(int));
extern void check_unique_instance(const char * pidfname);

extern int tcp_read(int sockfd, void * buf, int len);
extern int tcp_write(int sockfd, void * buf, int len);

extern int set_nonblock(int fd);

// xml parse utilities
//extern xmlNodePtr get_xml_childnode(xmlNodePtr parent, const char * ndname);

//extern int get_xml_node_intprop(xmlNodePtr nd, const char * propname);
//extern const char * get_xml_node_strprop(xmlNodePtr nd, const char * propname);
//extern const char * get_xml_node_content(xmlNodePtr nd);


// get xml node child's content.
//extern const char * get_xml_node_strfield(xmlNodePtr nd, const char * fieldname);
//
//extern int get_xml_node_intfield(xmlNodePtr nd, const char * fieldname);
//extern bool get_xml_node_intfield(xmlNodePtr nd, const char * fieldname, int * value);

// md5 calc
extern void md5_calc(unsigned char * out, unsigned char * in, unsigned int len);

//
// in_addr_t ip to string
#define  strip(ip)  inet_ntoa(*((struct in_addr *) &ip))

extern std::string & gb23122utf8(std::string & dst, const char * src);
extern std::string & utf82gb2312(std::string & dst, const char * src);

#endif /*COMMON_UTILITY_H_*/
