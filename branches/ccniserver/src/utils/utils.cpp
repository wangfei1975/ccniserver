/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             utils.cpp                                                               */
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>
#include <signal.h>
#include <pthread.h>
#include <stdarg.h>
#include <openssl/md5.h>
#include <string>
#include <libxml/parser.h>
#include "utils.h"
#include "log.h"

static bool lock_process(const char * pidfname)
{
    bool success = false;
    FILE *fp= NULL;
    int fd = -1;
    int pid = 0;

    if ((fd = open(pidfname, (O_RDWR | O_CREAT), 0644)) < 0)
    {
        LOGE("Can't open pid file. lock held by pid %d", pid);
        return false;
    }
    /* Write PID to file, set PID file to read/write for owner, read by others */

    if ( (fp = fdopen(fd, "r+")) != NULL)
    {
        if (flock(fd, (LOCK_EX | LOCK_NB)) < 0)
        {
            fscanf(fp, "%d", &pid);
            LOGE("Can't lock process, lock held by pid %d", pid);
        }
        else
        {
            pid = getpid();
            if ( (fprintf(fp, "%d", pid)) && (!fflush(fp)) && (!flock(fd, LOCK_UN)))
            {
                success = true;
            }
        }
        fclose(fp);
    }
    close(fd);

    return (success );
}

static int read_pid_file(const char *pid_file)
{
    FILE *f;
    int pid = 0;

    if ( (f = fopen(pid_file, "r")) != NULL)
    {
        fscanf(f, "%d", &pid);
        fclose(f);
    }
    return (pid);
}

static bool check_for_lock_process(const char * pidfname)
{
    bool is_locked = true;
    int stored_pid = read_pid_file(pidfname);
    if ( (stored_pid == 0) || (stored_pid == getpid()))
    {
        /* There was no stored PID, or, the active process is the locked process */
        is_locked = false;
    }
    else
    {
        /* The stored process is not the active process. Verify if the stored process
         is actually running, or if the stored PID was invalid. Do this by sending a kill
         with a siganl of 0 (do nothing, just do error checking).  If errno is set to
         ESRCH, then the process is not active */
        if ((kill(stored_pid, 0)) && (errno == ESRCH))
        {
            is_locked = false;
        }
    }

    return (is_locked);
}

const char * get_executable_path()
{
   static char path[PATH_MAX] = {0};
   if (path[0] == 0)
   {
       if (readlink("/proc/self/exe", path, PATH_MAX) > 0)
       {
           char * p = path + strlen(path) - 1;
           while(*p != '/' & p > path)
           {
               p--;
           }
           if (*p == '/') *(p+1) = 0;
       }
       
   }
   return path;
}
void run_as_daemon(void (*handler)(int))
{
    pid_t pid;
    
    dup2(STDERR_FILENO, STDOUT_FILENO);
    if ((pid = fork()) < 0)
    {
        printf("fork error!\n");
        exit(-1);
    }
    else if (pid > 0)
    {
        //exit parent process.
        exit(0);
    }
    
    // obtain a new process group 
    setsid();
     
    // set up the signal handlers
    signal(SIGINT, handler);
    signal(SIGHUP, handler);
    signal(SIGKILL, handler);
    signal(SIGTERM, handler);
   
    // ignore child
    signal(SIGCHLD, SIG_IGN);

    //ignore tty signals
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
   
}
int set_nonblock(int fd)
{
    int fg;
    if ((fg = fcntl(fd, F_GETFL)) < 0)
    {
        return fg;
    }
    return fcntl(fd, F_SETFL, fg|O_NONBLOCK);
    
}
int tcp_read(int fd, void * buf, int len)
{
    int rlen = 0;
    while (rlen < len)
    {
        int ret = recv(fd, ((char *)buf)+rlen, len-rlen, 0);
        if (ret < 0)
        {
            LOGE("recv from %d error: %s!\n", fd, strerror(errno));
            return -1;
        }
        else if (ret == 0)
        {
            return -2;
        }

        rlen += ret;
    }
    return rlen;
}

int tcp_write(int fd, void * buf, int len)
{
    int wlen = 0;
    while (wlen < len)
    {
        int ret = send(fd, ((char *)buf)+wlen, len-wlen, MSG_NOSIGNAL|MSG_DONTWAIT);
        if (ret <= 0 && errno != EAGAIN)
        {
            //LOGW("write to %d error: %s!\n", fd, strerror(errno));
            return -1;
        }
        if (errno == EAGAIN)
        {
            //LOGV("write EAGAIN\n");
            usleep(1);
        }
        if (ret > 0)
        {
           wlen += ret;
        }
    }
    return wlen;
}

void check_unique_instance(const char * pidfname)
{
    //check to see if transceiver daemon is already running */
    if (check_for_lock_process(pidfname))
    {
        printf("daemon already running, exiting!\n");
        exit(1);
    }

    // Lock this process as only one process allowed to run
    if (!lock_process(pidfname))
    {
        printf("error writing pid to file, exiting!\n");
        exit(1);
    }
}

int get_xml_node_intprop(xmlNodePtr nd, const char * propname)
{
    int ret = 0;
    xmlChar * p = xmlGetProp(nd, BAD_CAST(propname));
    if (p)
    {
        LOGD("%s = %s\n", propname, (char *)p);
        sscanf((char *)p, "%d", &ret);
    }
    xmlFree(p);
    return ret;
}
const char * get_xml_node_strprop(xmlNodePtr nd, const char * propname)
{
    static char buf[256];
    xmlChar * p = xmlGetProp(nd, BAD_CAST(propname));
    if (p)
    {
        strncpy(buf, (const char *)p, 256);
    }
    else
    {
        buf[0] = 0;
    }
    xmlFree(p);
    return buf;
}

const char * get_xml_node_content(xmlNodePtr nd)
{
    static char buf[256];
    xmlChar * p = xmlNodeGetContent(nd);
    if (p)
    {
        strncpy(buf, (const char *)p, 256);
    }
    else
    {
        buf[0] = 0;
    }
    xmlFree(p);
    return buf;
}

const char * get_xml_node_strfield(xmlNodePtr nd, const char * fieldname)
{
    static char buf[256];
    for (xmlNodePtr ch = nd->children; ch != NULL; ch = ch->next)
    {
        if (strcmp((const char *)ch->name, fieldname) == 0)
        {
            return get_xml_node_content(ch);
        }
    }
    buf[0] = 0;
    return buf;
}

xmlNodePtr get_xml_childnode(xmlNodePtr parent, const char * ndname)
{
    for (xmlNodePtr ch = parent->children; ch != NULL; ch = ch->next)
    {
        if (strcmp((const char *)ch->name, ndname) == 0)
        {
            return ch;
        }
    }
    return NULL;
}

int get_xml_node_intfield(xmlNodePtr nd, const char * fieldname)
{
    int v = 0;
    for (xmlNodePtr ch = nd->children; ch != NULL; ch = ch->next)
    {
        if (strcmp((const char *)ch->name, fieldname) == 0)
        {
            const char * s = get_xml_node_content(ch);
            sscanf(s, "%d", &v);
            return v;
        }
    }
    return v;
}

bool get_xml_node_intfield(xmlNodePtr nd, const char * fieldname, int * value)
{
    int v = 0;
    for (xmlNodePtr ch = nd->children; ch != NULL; ch = ch->next)
    {
        if (strcmp((const char *)ch->name, fieldname) == 0)
        {
            const char * s = get_xml_node_content(ch);
            if (sscanf(s, "%d", &v) == 1)
            {
                *value = v;
                return true;
            }
            return false;
        }
    }
    return false;
}

void md5_calc(unsigned char * out, unsigned char * in, unsigned int len)
{
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, in, len);
    MD5_Final(out, &c);
}
 std::string & gb23122utf8(std::string & dst, const char * src)
{
    iconv_t icpt;

    size_t inlen = strlen(src), outlen;
    dst.erase(dst.begin(), dst.end());

    if ((icpt = iconv_open("utf-8", "gb2312")) == (iconv_t)-1)
    {
        return dst;
    }
    outlen = inlen*2;
    char * out = new char[outlen+1];
    char * sout = out;
    int ret;
    if ((ret = (int)iconv(icpt, (char **)&src, &inlen, &sout, &outlen)) != -1)
    {
        *sout = 0;
        dst = out;
    }

    delete []out;
    iconv_close(icpt);
    return dst;
}

std::string & utf82gb2312(std::string & dst, const char * src)
{
    iconv_t icpt;

    size_t inlen = strlen(src), outlen;
    dst.erase(dst.begin(), dst.end());

    if ((icpt = iconv_open("gb2312", "utf-8")) == (iconv_t)-1)
    {
        return dst;
    }
    outlen = inlen;
    char * out = new char[outlen+1];
    char * sout = out;
    int ret;
    if ((ret = (int)iconv(icpt, (char **)&src, &inlen, &sout, &outlen)) != -1)
    {
        *sout = 0;
        dst = out;
    }

    delete []out;
    iconv_close(icpt);
    return dst;

}
