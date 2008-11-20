#include <stdio.h>

#include "log.h"

int main(int argc, char * argv[])
{
    printf("%d\n", abcde);
    LOGE("ccni server, log error.\n");
    LOGW("ccni server, log warning.\n");
    LOGI("ccni server, log information.\n");
    LOGD("ccni server, log debug.\n");
    LOGV("ccni server, log verbos.\n");
    return 0;
}

