//
// Created by parthka on 11/6/22.
// timerfd' example
//


#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/epoll.h"
#include "sys/timerfd.h"
#include "time.h"

int epfd;

typedef struct timer_event_s{
    int fd;
    void (*handler)(struct epoll_event*);
} timer_event;

void hs_server_timer_cb(struct epoll_event* ev) {
    uint64_t res;
    timer_event* tev = (timer_event*)ev->data.ptr;
    //printf("%d", tev->fd);
    int bytes = read(tev->fd, &res, sizeof(res)); // important
    (void)bytes; // suppress warning
    printf("Time Is Running...\n");
}

void hs_server_timer_cb_2(struct epoll_event* ev) {
    uint64_t res;
    timer_event* tev = (timer_event*)ev->data.ptr;
    //printf("%d", tev->fd);
    int bytes = read(tev->fd, &res, sizeof(res)); // important
    (void)bytes; // suppress warning
    printf("Time Is Not Stoppable...\n");
}

int main(){

    epfd = epoll_create1(0);

    // add timer for 1 second
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    struct itimerspec ts = {};
    ts.it_value.tv_sec = 1;
    ts.it_interval.tv_sec = 1;
    timerfd_settime(tfd, 0, &ts, NULL);

    timer_event* te = (timer_event*)malloc(sizeof(timer_event));
    te->handler = hs_server_timer_cb;
    te->fd = tfd;

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = te;
    epoll_ctl(epfd, EPOLL_CTL_ADD, tfd, &ev); // add for poll


    // add new timer
    int tfd2 = timerfd_create(CLOCK_MONOTONIC, 0);
    struct itimerspec ts2 = {};
    ts2.it_value.tv_sec = 1;
    ts2.it_interval.tv_sec = 5;
    timerfd_settime(tfd2, 0, &ts2, NULL);

    timer_event* te2 = (timer_event*)malloc(sizeof(timer_event));
    te2->handler = hs_server_timer_cb_2;
    te2->fd = tfd2;

    struct epoll_event ev2;
    ev2.events = EPOLLIN | EPOLLET;
    ev2.data.ptr = te2;
    epoll_ctl(epfd, EPOLL_CTL_ADD, tfd2, &ev2); // add for poll

    //hs_server_timer_cb(&ev);

    while (1) {
        struct epoll_event ev_list[1];
        int nev = epoll_wait(epfd, ev_list, 1, -1);
        for (int i = 0; i < nev; i++) {
            timer_event* t = (timer_event*)ev_list[i].data.ptr;
            t->handler(&ev_list[i]);
        }
    }

}
