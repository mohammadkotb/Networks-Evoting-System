#ifndef TIMER_H_
#define TIMER_H_

#include <pthread.h>
#include <signal.h>

class MutexTimer{
    public:
        MutexTimer(int milliSec,pthread_mutex_t * mutex);
        ~MutexTimer();
        void stop();
    private:
        bool *cancel;
	pthread_t thread;
};
#endif
