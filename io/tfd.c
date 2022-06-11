//
// Created by parthka on 11/6/22.
// timerfd example
//

#include "stdio.h"
#include "sys/epoll.h"
#include "sys/timerfd.h"

int epfd;

void print_name();
void* p(void* p){

    // create timer fd
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    struct itimerspec ts = {};

    // set timer
    ts.it_value.tv_sec = 2;
    ts.it_interval.tv_sec = 5;
    timerfd_settime(tfd, 0, &ts, NULL);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = NULL;

    // add fd
    epoll_ctl(epfd, EPOLL_CTL_ADD, tfd, &ev);
}

int tfd;

int main() {

    epfd = epoll_create1(0);
    pthread_t t1;
    // pthread_create(&t1, NULL, p, NULL);

    p(NULL);

    // event loop
    while(1){
        struct epoll_event ev[1];
        // wait for fd
        int r = epoll_wait(epfd, ev, 1, -1);
        for (int i = 0; i < r; ++i) {
            // print function
            print_name();
            // p(NULL); //call again and make infinite loop
        }
    }

    // meaningless
    printf("Hello, World!");

}

void print_name(){
    printf("Hello, 123!\n");
}
