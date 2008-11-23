#include "log.h"
#include "utils.h"
#include "config.h"
#include "ccni_log.h"

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
    
    LOGI("loading configuration file %s\n", cfgpname.c_str());
    CConfig cfg;
    
    if (!cfg.create(cfgpname.c_str()))
    {
        LOGE("create server configuration error!\n");
        return 0;
    }
    
    CCNILog loger(cfg.logcfg);
    
    loger.print("%s", copyright);
    
    loger.print("server startup success!\n");
    
    LOGI("server startup success!\n");

    while(1)
    {
       sleep(1);
    }
    return 0;
}
