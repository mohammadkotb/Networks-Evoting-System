#include "mutex_timer.h"
#include <unistd.h>

void* thread_func(void *args){
    void ** ar = (void **)args;
    int time = *((int *)ar[1]);
    pthread_mutex_t* mutex = (pthread_mutex_t *)ar[0];
    bool* cancel = (bool *)ar[2];
    usleep(time * 1000);
    if (!(*cancel))
        pthread_mutex_unlock(mutex);
    //delete(cancel);
    delete((int *)ar[1]);
    delete(ar);
    return NULL;
}

MutexTimer::MutexTimer(int millisec,pthread_mutex_t * mutex){
    this->cancel = new bool(false);
    int *ms = new int(millisec);
    void** args = new void*[3];

    args[0] =(void *) mutex;
    args[1] =(void *) ms;
    args[2] =(void *) cancel;

    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&thread, NULL, thread_func, (void *)args))
        throw -1;

    pthread_attr_destroy(&attr);
}

void MutexTimer::stop(){
    *cancel = true;
}

MutexTimer::~MutexTimer(){
    //the thread will delete the bool and int instance
}
