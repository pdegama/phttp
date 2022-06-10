//
// Created by parthka on 10/6/22.
//

#ifndef PHTTP_PHTTP_H
#define PHTTP_PHTTP_H

#include "sys/socket.h"
#include "sys/types.h"
#include "sys/epoll.h"
#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"
#include "signal.h"
#include "stdlib.h"
#include "assert.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include <sys/timerfd.h>

typedef struct http_server_s http_server_t;
typedef struct http_request_s http_request_t;

http_server_t* http_server_init(int port, void (*handler)(struct http_request_s*));

int http_server_listen(struct http_server_s* server);

void hs_bind_localhost(int s, struct sockaddr_in* addr, const char* ipaddr, int port);
void hs_add_server_sock_events(struct http_server_s* serv);
void hs_server_init(struct http_server_s* serv);

#endif //PHTTP_PHTTP_H
