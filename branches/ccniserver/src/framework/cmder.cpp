#include "cmder.h"
#include "engine.h"
void CCmder::dowork()
{
    char buf[1024];
    struct sockaddr_in addr;
    socklen_t flen = sizeof(addr);
    while (1)
    {
        if (recvfrom(_sock, buf, sizeof(buf), 0, (struct sockaddr *)&(addr), &flen) <= 0)
        {
            continue;
        }

        if (buf[0] == 'h')
        {
            int busycnt = CEngine::instance().threadsPool().getBusyCount();
            int jobs = CEngine::instance().threadsPool().getQueuedJobs();
            sprintf(buf, "pool status: busycnt: %d, queued jobs %d\n", busycnt, jobs);
            sendto(_sock, buf, strlen(buf)+1, 0, (struct sockaddr *)&(addr), sizeof(addr));

        }
    }
}

