#include "timer.h"
#include <stdio.h>
#include <errno.h>

void  TimerMgr::timer_handler(int sig, siginfo_t *si, void *uc)
{
    timer_t *tidp;
    tidp = (void **)(si->si_value.sival_ptr);
 
    timer_info timeInfo = m_pInstance->timerMap[*tidp];

    void (*callback)(void*) = timeInfo.cb_func;
    void* userdata = timeInfo.userData;
    callback(userdata);

    struct itimerspec its;
    timer_gettime(*tidp, &its);
    if ((its.it_value.tv_sec == 0) && (its.it_value.tv_nsec==0)
        &&(its.it_interval.tv_sec ==0 ) && (its.it_interval.tv_nsec == 0)){

        timeInfo.activated = false;
        m_pInstance->timerMap[*tidp] = timeInfo;
    }
}

TimerMgr* TimerMgr::m_pInstance = NULL;

TimerMgr::TimerMgr()
{

    m_pInstance = this;
    struct sigaction sa;

    sa.sa_flags  = SA_SIGINFO;
    sa.sa_sigaction = (void(*)(int, siginfo_t *, void *)) timer_handler;
    // sa.sa_sigaction = timer_handler;

    // sa.sa_flags  = 0;
    // sa.sa_handler = timer_handler;

    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGALRM, &sa, NULL) == -1){
        perror("fail sigaction");
    }
}

TimerMgr* TimerMgr::instance()
{
    static TimerMgr *timer = NULL;
    if (timer == NULL){
        timer = new TimerMgr();
    }
    return timer;
}

bool TimerMgr::create(timer_t* timerId)
{
    struct sigevent sigEvt;

    sigEvt.sigev_notify    = SIGEV_SIGNAL;
    sigEvt.sigev_signo    = SIGALRM;
    sigEvt.sigev_value.sival_ptr = timerId;

    if(timer_create(CLOCK_REALTIME, &sigEvt, timerId) == -1){
        perror("fail to create timer");
        return 0;
    }
    return true;
}


bool TimerMgr::setup(timer_t timerId, unsigned int timeval, unsigned int interval, void(*callback_func)(void*), void* data )
{
    timer_info timeInfo;

    timeInfo.timeValue  = (timeval);
    timeInfo.itsVal = (interval);
    timeInfo.cb_func = callback_func;
    timeInfo.userData = data;
    timeInfo.activated = false;

    timerMap.insert(std::make_pair(timerId, timeInfo));

    return true;
}

bool TimerMgr::start(timer_t timerId)
{
    struct itimerspec its;

    timer_info timeInfo = timerMap[timerId];

    its.it_value.tv_sec  = (timeInfo.timeValue)/1000;
    its.it_value.tv_nsec  = ((timeInfo.timeValue)%1000) * 1000;

    its.it_interval.tv_sec = (timeInfo.itsVal) / 1000;
    its.it_interval.tv_nsec = ((timeInfo.itsVal)%1000) * 1000;
    
    if (timer_settime(timerId, 0, &its, NULL) == -1){
        perror(" fail to timer_settime");
        return false;
    }
    timeInfo.activated = true;
    timerMap[timerId] = timeInfo;
    return true;

}
bool TimerMgr::stop(timer_t timerId)
{
    struct itimerspec its;

    its.it_value.tv_sec  = 0;
    its.it_value.tv_nsec  = 0;

    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    
    if (timer_settime(timerId, 0, &its, NULL) == -1){
        perror(" fail to timer_settime");
        return false;
    }
    return true;
}

void TimerMgr::destroy(timer_t timerId)
{
    timerMap.erase(timerId);
    timer_delete(timerId);
}
