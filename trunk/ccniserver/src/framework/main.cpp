#include "log.h"
#include "utils.h"

static const char * pidfname = "/tmp/ccnisvrpid"; 

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
    
    process_cmdline(argc, argv);

   // check_unique_instance(pidfname);
    LOGD("exec path: %s\n", get_executable_path());
    LOGI("server startup success!\n");

    while(1)
    {
       sleep(1);
    }
    return 0;
}
