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
/*             ccni.h                                                                  */
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

#ifndef CCNI_H_
#define CCNI_H_

#include <map>
#include "log.h"

#define  CCNI_MAX_LEN   (1024*32)
using namespace std;

typedef uint64_t secret_key_t;

/*
 * 包头长度      hdlen      1  数据包包头的长度，值为32
 * 版本号        ver_major  1  主版本号，值为1
 * 副版本号      ver_minor  1  副版本号，值为0
 * 数据包类型    type       1  数据段的类刑，0 表示连接请求，1 表示的正常数据包。
 * 数据长度      data_len   2  包头后面的数据信息的长度，最大为64K
 * 数据包序列号  seq        4  本数据包的序列号
 * 用户数据      udata      4  用户数据
 * 数据包        Key secret 16 用于加密身份验证的Key
 * 保留          reserved   2  保留，值为0
 */
#pragma pack(push, 1)
struct CCNI_HEADER
{
    uint8_t hdlen;
    uint8_t ver_major;
    uint8_t ver_minor;
    uint8_t type;
    uint16_t datalen;
    uint32_t seq;
    uint32_t udata;
    secret_key_t secret1;
    secret_key_t secret2;
    uint8_t reserved[2];
    CCNI_HEADER()
    {
        memset(this, 0, sizeof(*this));
        hdlen = sizeof(*this);
        ver_major = 1;
    }

    bool verify()
    {
        return ((hdlen == sizeof(*this)) && (ver_major == 1) && (ver_minor == 0) && (datalen < CCNI_MAX_LEN));
    }
};

#pragma pack(pop)

#endif /*CCNI_H_*/

