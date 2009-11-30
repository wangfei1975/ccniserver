#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "log.h"
#include "ccni.h"
#include "ccni_msg.h"

static const char * copyright = "\n"
    "CCNI Management tool 1.0.\n"
    "Copyright (C) bjwf 2008-2016. All rights reserved.\n"
    "bjwf2000@gmail.com  11-20-2008\n"
    "http://ccniserver.googlecode.com\n\n";

void dumpbin(void * buf, int len)
{
    unsigned char * p = (unsigned char *)buf;
    int i;
    for (i = 0; i < len; i++)
    {
        printf("%02X ", p[i]);
        if (((i+1)%16) == 0)
        {
            printf("\n");
        }
    }
    if ((i%16) != 0)
    {
        printf("\n");
    }
}
int main(int argc, char * argv[])
{
    printf(copyright);
    if (argc < 2)
    {
        printf("ccni [serverip:port]\n");
        return 0;
    }

    char ipbuf[100];
    strcpy(ipbuf, argv[1]);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    char * p = strstr(ipbuf, ":");
    if (p == NULL)
    {
        printf("%s is no valid ip:port\n", ipbuf);
        return 0;
    }
    int port;
    sscanf(p+1, "%d", &port);
    addr.sin_port = htons(port);
    *p = 0;
    addr.sin_addr.s_addr = inet_addr(ipbuf);
    printf("server ip:port is %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in laddr;
    memset(&laddr, 0, sizeof(laddr));
    laddr.sin_family = AF_INET;

    struct sockaddr_in raddr;
    socklen_t rlen = (socklen_t)sizeof(raddr);
    char sbuf[1024];
    sprintf(sbuf, "hello");
    
    if (sendto(sock, sbuf, strlen(sbuf)+1, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("send error. %s\n", strerror(errno));
    }

    char rbuf[1024];
    recvfrom(sock, rbuf, 1024, 0, (struct sockaddr *)&raddr, &rlen);
    printf("received info:\n%s\n", rbuf);
    close(sock);
}

