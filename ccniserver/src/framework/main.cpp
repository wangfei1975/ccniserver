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
#include "engine.h" 


static const char * copyright = "\n" \
                                "CCNI Chinese chess network server version 1.0 for Linux.\n" \
                                "Copyright (C) bjwf 2008-2016. All rights reserved.\n" \
                                "bjwf2000@gmail.com  11-20-2008\n"
                                "http://ccniserver.googlecode.com\n\n";
                                
//static const char * pidfname = "/tmp/ccnisvrpid"; 
static const char * CFGFNAME = "ccniserver.conf.xml";
static void sig_handler(int signal)
{
    LOGI("daemon received signal %d, shutting down!\n", signal);
    CEngine::instance().destroy();
    usleep(10);
    exit(0);
}

static void process_cmdline(int argc, char * argv[])
{
    //process command line.
     LOGI("process command line.\n");
}

int main(int argc, char * argv[])
{
  
    run_as_daemon(sig_handler);
 
    LOGI(copyright);
    
    process_cmdline(argc, argv);
 
   // check_unique_instance(pidfname);
    
    string cfgpname(string(get_executable_path())+ CFGFNAME);
 
    if (!CEngine::instance().create(cfgpname.c_str()))
    {
        CEngine::instance().destroy();
        LOGE("create ccni server engine error.\n");
        return 0;
    }
    
    LOGI("server startup success!\n");

    CEngine::instance().loop();
    
    return 0;
}
