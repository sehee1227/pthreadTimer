#ifndef TIMER_H
#define TIMER_H

#include <map>
#include <time.h>
#include <signal.h>
#include <unistd.h>

struct timer_info{
    unsigned int timeValue;
    unsigned int itsVal;
    void (*cb_func)(void*);
    void* userData;
    bool activated;
};

class TimerMgr
{

    std::map<timer_t, timer_info> timerMap;
    static TimerMgr* m_pInstance; 

    void static timer_handler(int sig, siginfo_t *si, void *uc);

public:
    TimerMgr();
    ~TimerMgr()
    {
        timerMap.clear();
    }

    static TimerMgr* instance();
    bool create(timer_t*);
    bool setup(timer_t timerId, unsigned int tv, unsigned int its, void(*)(void*), void*);
    bool start(timer_t timerId);
    bool stop(timer_t timerId);
    void destroy(timer_t tiemrId);
};

#endif