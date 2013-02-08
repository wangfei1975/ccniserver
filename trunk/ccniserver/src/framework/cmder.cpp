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

