//
// Created by parthka on 10/6/22.
//

#include "stdio.h"
#include "phttp.h"
#include "time.h"

typedef void (*epoll_cb_t)(struct epoll_event*);

typedef struct http_ev_cb_s {
    epoll_cb_t handler;
} ev_cb_t;

typedef struct {
    int index;
    int len;
    int type;
} http_token_t;

typedef struct {
    http_token_t* buf;
    int capacity;
    int size;
} http_token_dyn_t;

typedef struct {
    char* buf;
    int64_t total_bytes;
    int32_t capacity;
    int32_t length;
    int32_t index;
    int32_t anchor;
    http_token_t token;
    uint8_t flags;
} hs_stream_t;

typedef struct {
    int64_t content_length;
    int64_t body_consumed;
    int16_t match_index;
    int16_t header_count;
    int8_t state;
    int8_t meta;
} http_parser_t;

typedef struct http_server_s {
    epoll_cb_t handler;
    epoll_cb_t timer_handler;
    int64_t memused;
    int socket;
    int port;
    int loop;
    int timerfd;
    socklen_t len;
    void (*request_handler)(http_request_t*);
    struct sockaddr_in addr;
    void* data;
    char date[32];
} http_server_t;

typedef struct http_request_s {
    epoll_cb_t handler;
    epoll_cb_t timer_handler;
    int timerfd;
    void (*chunk_cb)(struct http_request_s*);
    void* data;
    hs_stream_t stream;
    http_parser_t parser;
    int state;
    int socket;
    int timeout;
    struct http_server_s* server;
    http_token_dyn_t tokens;
    char flags;
} http_request_t;

void hs_bind_localhost(int s, struct sockaddr_in* addr, const char* ipaddr, int port) {
    addr->sin_family = AF_INET;
    if (ipaddr == NULL) {
        addr->sin_addr.s_addr = INADDR_ANY;
    } else {
        addr->sin_addr.s_addr = inet_addr(ipaddr);
    }
    addr->sin_port = htons(port);
    int rc = bind(s, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
    if (rc < 0) {
        exit(1);
    }
}

void hs_add_server_sock_events(http_server_t* serv) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = serv;
    epoll_ctl(serv->loop, EPOLL_CTL_ADD, serv->socket, &ev);
}

void http_listen(http_server_t* serv, const char* ipaddr) {
    // Ignore SIGPIPE. We handle these errors at the call site.
    signal(SIGPIPE, SIG_IGN);
    serv->socket = socket(AF_INET, SOCK_STREAM, 0);
    int flag = 1;
    setsockopt(serv->socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    hs_bind_localhost(serv->socket, &serv->addr, ipaddr, serv->port);
    serv->len = sizeof(serv->addr);
    int flags = fcntl(serv->socket, F_GETFL, 0);
    fcntl(serv->socket, F_SETFL, flags | O_NONBLOCK);
    listen(serv->socket, 128);
    hs_add_server_sock_events(serv);
}

int http_server_listen(http_server_t* serv) {
    http_listen(serv, NULL);
    struct epoll_event ev_list[1];
    while (1) {
        int nev = epoll_wait(serv->loop, ev_list, 1, -1);
        for (int i = 0; i < nev; i++) {
            ev_cb_t* ev_cb = (ev_cb_t*)ev_list[i].data.ptr;
            ev_cb->handler(&ev_list[i]);
        }
    }
    return 0;
}

void hs_generate_date_time(char* datetime) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = gmtime(&rawtime);
    strftime(datetime, 32, "%a, %d %b %Y %T GMT", timeinfo);
}

void hs_server_timer_cb(struct epoll_event* ev) {
    http_server_t* server = (http_server_t*)((char*)ev->data.ptr - sizeof(epoll_cb_t));
    uint64_t res;
    int bytes = read(server->timerfd, &res, sizeof(res));
    (void)bytes; // suppress warning
    hs_generate_date_time(server->date);
}

void hs_server_init(http_server_t* serv) {
    serv->loop = epoll_create1(0);
    serv->timer_handler = hs_server_timer_cb;

    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    struct itimerspec ts = {};
    ts.it_value.tv_sec = 1;
    ts.it_interval.tv_sec = 1;
    timerfd_settime(tfd, 0, &ts, NULL);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = &serv->timer_handler;
    epoll_ctl(serv->loop, EPOLL_CTL_ADD, tfd, &ev);
    serv->timerfd = tfd;
}

void hs_server_listen_cb(struct epoll_event* ev) {
    hs_accept_connections((http_server_t*)ev->data.ptr);
}

void hs_accept_connections(http_server_t* server) {
    int sock = 0;
    do {
        sock = accept(server->socket, (struct sockaddr *)&server->addr, &server->len);
        if (sock > 0) {
            http_request_t* session = (http_request_t*)calloc(1, sizeof(http_request_t));
            assert(session != NULL);
            session->socket = sock;
            session->server = server;
            session->timeout = HTTP_REQUEST_TIMEOUT;
            session->handler = hs_session_io_cb;
            int flags = fcntl(sock, F_GETFL, 0);
            fcntl(sock, F_SETFL, flags | O_NONBLOCK);
            hs_add_events(session);
            http_session(session);
        }
    } while (sock > 0);
}

// init server
http_server_t* http_server_init(int port, void (*handler)(http_request_t*)) {
    http_server_t* serv = (http_server_t*)malloc(sizeof(http_server_t));
    assert(serv != NULL);
    serv->port = port;
    serv->memused = 0;
    serv->handler = hs_server_listen_cb;
    hs_server_init(serv);
    hs_generate_date_time(serv->date);
    serv->request_handler = handler;
    return serv;
}


