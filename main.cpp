#include <iostream>
#include "timer.h"
#include "cond.h"
#include <stdio.h>

using namespace std;

static unsigned int signo = 0;

void callback_fn(void* userData){
    cout << "callback function" << endl;
    // CondMgr *myCond= reinterpret_cast<CondMgr*>(userData);
    CondMgr *myCond= static_cast<CondMgr*>(userData);
    myCond->signal();

    signo |= 0x01;
}
void callback_fn2(void* userData){
    cout << "callback function2" << endl;
	CondMgr *myCond = (CondMgr*)(userData);
    myCond->signal();
    signo |= 0x02;
}

int main()
{
    timer_t myTimer, myTimer2;
	unsigned int sigmask;

    TimerMgr *timer = TimerMgr::instance();
    CondMgr *myCond = new CondMgr();

    timer->create(&myTimer);
    timer->setup(myTimer2, 2500, 2500, callback_fn, (void*)myCond);
    timer->start(myTimer);

    timer->create(&myTimer2);
    timer->setup(myTimer2, 3500, 3500, callback_fn2, (void*)myCond);
    timer->start(myTimer2);
    for(int i=0; i<20; i++){
        myCond->wait();
			printf("signo: %x\n",signo);
        if (signo & 0x01){
			sigmask = ~(0xFFFFFFFF & 0x01);
            signo &= sigmask;
			printf("signo1: %x, sigmask1: %x\n",signo, sigmask);
        }

        if (signo & 0x02){
			sigmask = ~(0xFFFFFFFF & 0x02);
            signo &= sigmask;
			printf("signo2: %x, sigmask2: %x\n",signo, sigmask);
        }
    }
    timer->stop(myTimer);
	timer->stop(myTimer2);
	myCond->wait_timeout(10000);
	return 0;




}
