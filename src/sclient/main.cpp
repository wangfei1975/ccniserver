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
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "../libccni/libccni.h"

/*
 static const char * copyright = "\n"
 "CCNI Simple client 1.0.\n"
 "Copyright (C) bjwf 2008-2016. All rights reserved.\n"
 "bjwf2000@gmail.com  11-20-2008\n"
 "http://ccniserver.googlecode.com\n\n";

 */
int main(int argc, char * argv[])
{
    string ccniInfo;
    CCNIConnection con(NULL);
    CCNIConnection::room_lst_t rooms;
    if (con.connect("127.0.0.1", 20081, 20082) < 0)
    {
        printf("connect error.\n");
        return 0;
    }
    printf("connect success.\n");
    
    char uname[256];
    sprintf(uname, "bjwf%u", (unsigned int)pthread_self());
    con.login(uname, "12345");
    con.ccni(ccniInfo);
    con.state();
    con.listrooms(rooms);
    con.enterroom(1);
    
    con.listsessions();
    
    con.newsession();
    
    
    
    while(1)
    {
        sleep(1);
    }
    // con.ccni();
    return 0;
}

