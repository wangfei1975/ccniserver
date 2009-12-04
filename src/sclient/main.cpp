#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "log.h"
#include "ccni.h"
#include "ccni_msg.h"

static const char * copyright = "\n"
    "CCNI Simple client 1.0.\n"
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
    CCNI_HEADER hd, rhd;
    if (sendto(sock, &hd, sizeof(hd), 0, (struct sockaddr *)&addr, sizeof(addr)) != sizeof(hd))
    {
        printf("send error. %s\n", strerror(errno));
    }

   // printf("send header(%d):\n", sizeof(hd));
   // dumpbin(&hd, sizeof(hd));
    recvfrom(sock, &rhd, sizeof(rhd), 0, (struct sockaddr *)&raddr, &rlen);
    printf("received header(%d):\n", sizeof(hd));
   // dumpbin(&rhd, sizeof(rhd));
    close(sock);
    int tcpfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in tcpraddr;
    memset(&tcpraddr, 0, sizeof(tcpraddr));
    tcpraddr.sin_family = AF_INET;
    tcpraddr.sin_addr.s_addr = inet_addr(ipbuf);
    tcpraddr.sin_port = htons(port-1);

    if (connect(tcpfd, (struct sockaddr *)&tcpraddr, sizeof(tcpraddr)) < 0)
    {
        printf("connect error. %s\n", strerror(errno));
        return 0;
    }

    CCNIMsgPacker msg;
    msg.create();
    CXmlMsg lgmsg;
    lgmsg.create(xmlTagLogin);
    char usrn[128];
    sprintf(usrn, "bjwf%d", getpid());
    lgmsg.addParameter(xmlTagUserName, usrn);
    lgmsg.addParameter(xmlTagPassword, "123");

    msg.appendmsg(lgmsg);
    msg.pack(0, 0, rhd.secret1, rhd.secret2);

    msg.block_send(tcpfd);

    CCNIMsgParser rmsg;
    CCNIMsgParser::state_t st = rmsg.read(tcpfd);
    while (st != CCNIMsgParser::st_rdok)
    {
        usleep(1);
        st = rmsg.read(tcpfd);
        if (st == CCNIMsgParser::st_rderror)
            break;
    }
    rmsg.parse();
    //printf("return msg is\n %s\n", rmsg.data());

    msg.free();
    msg.create();

    lgmsg.create(xmlTagCCNI);
    msg.appendmsg(lgmsg);
    msg.pack(1, 2, rhd.secret1, rhd.secret2);
    for (int i = 0; i < 100; i++)
    {
        msg.block_send(tcpfd);
        rmsg.free();
       // printf("%d time send ok reading....\n", i);
        //usleep(10000000);
        st = rmsg.read(tcpfd);
        while (st != CCNIMsgParser::st_rdok)
        {
           // usleep(1);
            st = rmsg.read(tcpfd);
            if (st == CCNIMsgParser::st_rderror)
                break;
        }
        rmsg.parse();
        printf("return msg %d ok.\n", i);
        //printf("r msg is\n %s\n", rmsg.data());
    }
    msg.free();
    close(tcpfd);
}

