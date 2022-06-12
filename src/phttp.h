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
#include "sys/timerfd.h"

#define HTTP_REQUEST_TIMEOUT 20

// Application configurable
#define HTTP_REQUEST_BUF_SIZE 1024
#define HTTP_RESPONSE_BUF_SIZE 1024
#define HTTP_REQUEST_TIMEOUT 20
#define HTTP_KEEP_ALIVE_TIMEOUT 120
#define HTTP_MAX_TOKEN_LENGTH 8192 // 8kb
#define HTTP_MAX_TOTAL_EST_MEM_USAGE 4294967296 // 4gb
#define HTTP_MAX_REQUEST_BUF_SIZE 8388608 // 8mb

#define HTTP_MAX_HEADER_COUNT 127

#define HTTP_FLAG_SET(var, flag) var |= flag
#define HTTP_FLAG_CLEAR(var, flag) var &= ~flag
#define HTTP_FLAG_CHECK(var, flag) (var & flag)

// stream flags
#define HS_SF_CONSUMED 0x1

// parser flags
#define HS_PF_IN_CONTENT_LEN 0x1
#define HS_PF_IN_TRANSFER_ENC 0x2
#define HS_PF_CHUNKED 0x4
#define HS_PF_CKEND 0x8
#define HS_PF_REQ_END 0x10

// http session states
#define HTTP_SESSION_INIT 0
#define HTTP_SESSION_READ 1
#define HTTP_SESSION_WRITE 2
#define HTTP_SESSION_NOP 3

// http session flags
#define HTTP_END_SESSION 0x2
#define HTTP_AUTOMATIC 0x8
#define HTTP_CHUNKED_RESPONSE 0x20

// http version indicators
#define HTTP_1_0 0
#define HTTP_1_1 1

typedef struct http_server_s http_server_t;
typedef struct http_request_s http_request_t;
typedef struct http_token_dyn_s http_token_dyn_t;

http_server_t* http_server_init(int port, void (*handler)(struct http_request_s*));

int http_server_listen(struct http_server_s* server);

void hs_bind_localhost(int s, struct sockaddr_in* addr, const char* ipaddr, int port);
void hs_add_server_sock_events(struct http_server_s* serv);
void hs_server_init(struct http_server_s* serv);
void hs_server_timer_cb(struct epoll_event* ev);
void hs_generate_date_time(char* datetime);
void hs_server_listen_cb(struct epoll_event* ev);
void hs_accept_connections(http_server_t*);
void hs_session_io_cb(struct epoll_event* ev);
void http_session(http_request_t* request);
void hs_end_session(http_request_t* session);
void hs_delete_events(http_request_t* request);
void hs_free_buffer(http_request_t* session);
void hs_init_session(http_request_t* session);
void http_token_dyn_init(http_token_dyn_t* dyn, int capacity);

#endif //PHTTP_PHTTP_H
