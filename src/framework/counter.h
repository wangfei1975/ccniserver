#ifndef COUNTER_H_
#define COUNTER_H_
#include "mutex.h"
#include "log.h"
class CCounter
{
private:
    volatile uint32_t _msgcnt;
    CMutex            _msglk;
    
    volatile uint32_t _logincnt;
    CMutex            _loglk;
    
public:
    CCounter():_msgcnt(0),_logincnt(0)
    {
        
    }
    uint32_t msgCount()
    {
        CAutoMutex du(_msglk);
        return _msgcnt;
    }
    uint32_t loginCount()
    {
        CAutoMutex du(_loglk);
        return _logincnt;
    }
    void incMsgCnt()
    {
        CAutoMutex du(_msglk);
        ++_msgcnt;
    }
    void incLoginCnt()
    {
        CAutoMutex du(_loglk);
        ++_logincnt;
    }
};
#endif /*COUNTER_H_*/
