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
#include "string.h"
#include "assert.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "sys/timerfd.h"
#include "time.h"
#include "errno.h"
#include "netdb.h"
#include "stdarg.h"
#include "signal.h"
#include "limits.h"

#define HTTP_REQUEST_TIMEOUT 20

// Application configurable
#define HTTP_KEEP_ALIVE 1
#define HTTP_CLOSE 0
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

#define HTTP_FLG_STREAMED 0x1

#define HS_META_NOT_CHUNKED 0
#define HS_META_NON_ZERO 0
#define HS_META_END_CHK_SIZE 1
#define HS_META_END_CHUNK 2
#define HS_META_NEXT 0

#define HS_MATCH(str, meta) \
  in_bounds = parser->match_index < (int)sizeof(str) - 1; \
  m = in_bounds ? str[parser->match_index] : m; \
  low = c >= 'A' && c <= 'Z' ? c + 32 : c; \
  if (low != m) hs_trigger_meta(parser, meta);

enum hs_token {
    HS_TOK_NONE,        HS_TOK_METHOD,     HS_TOK_TARGET,     HS_TOK_VERSION,
    HS_TOK_HEADER_KEY,  HS_TOK_HEADER_VAL, HS_TOK_CHUNK_BODY, HS_TOK_BODY,
    HS_TOK_BODY_STREAM, HS_TOK_REQ_END,    HS_TOK_EOF,        HS_TOK_ERROR
};

enum hs_state {
    ST, MT, MS, TR, TS, VN, RR, RN, HK, HS, HV, HR, HE,
    ER, HN, BD, CS, CB, CE, CR, CN, CD, C1, C2, BR, HS_STATE_LEN
};

enum hs_char_type {
    HS_SPC,   HS_NL,  HS_CR,    HS_COLN,  HS_TAB,   HS_SCOLN,
    HS_DIGIT, HS_HEX, HS_ALPHA, HS_TCHAR, HS_VCHAR, HS_ETC,   HS_CHAR_TYPE_LEN
};

enum hs_meta_state {
    M_WFK, M_ANY, M_MTE, M_MCL, M_CLV, M_MCK, M_SML, M_CHK, M_BIG, M_ZER, M_CSZ,
    M_CBD, M_LST, M_STR, M_SEN, M_BDY, M_END, M_ERR
};

enum hs_meta_type {
    HS_META_NOT_CONTENT_LEN, HS_META_NOT_TRANSFER_ENC, HS_META_END_KEY,
    HS_META_END_VALUE,       HS_META_END_HEADERS,      HS_META_LARGE_BODY,
    HS_META_TYPE_LEN
};

typedef struct http_server_s http_server_t;
typedef struct http_request_s http_request_t;
typedef struct http_token_dyn_s http_token_dyn_t;
typedef struct http_header_s http_header_t;
typedef struct http_response_s http_response_t;
typedef struct hs_stream_s hs_stream_t;
typedef struct http_token_s http_token_t;
typedef struct http_parser_s http_parser_t;
typedef struct grwprintf_s grwprintf_t;
typedef struct http_string_s http_string_t;

struct http_string_s {
    char const * buf;
    int len;
};

int http_server_listen(struct http_server_s* server);
int hs_stream_read_socket(hs_stream_t* stream, int socket, int64_t* memused);
int hs_stream_next(hs_stream_t* stream, char* c);
int hs_case_insensitive_cmp(char const * a, char const * b, int len);
int hs_write_client_socket(http_request_t* session);
int hs_stream_jump(hs_stream_t* stream, int offset);
int hs_stream_jumpall(hs_stream_t* stream);
int hs_stream_can_contain(hs_stream_t* stream, int64_t size);
int http_request_iterate_headers(http_request_t* request, http_string_t* key, http_string_t* val, int* iter);
int hs_assign_iteration_headers(http_request_t* request, http_string_t* key, http_string_t* val, int* iter );

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
void hs_add_events(http_request_t* request);
void hs_request_timer_cb(struct epoll_event* ev);
void hs_write_response(http_request_t* request);
void hs_read_and_process_request(http_request_t* request);
void hs_error_response(http_request_t* request, int code, char const * message);
void hs_reset_timeout(http_request_t* request, int time);
void http_respond(http_request_t* request, http_response_t* response);
void http_response_body(http_response_t* response, char const * body, int length);
void http_response_header(http_response_t* response, char const * key, char const * value);
void http_response_status(http_response_t* response, int status);
void http_token_dyn_push(http_token_dyn_t* dyn, http_token_t a);
void http_end_response(http_request_t* request, http_response_t* response, grwprintf_t* printctx);
void grwprintf_init(grwprintf_t* ctx, int capacity, int64_t* memused);
void http_respond_headers(http_request_t* request, http_response_t* response, grwprintf_t* printctx);
void grwprintf(grwprintf_t* ctx, char const * fmt, ...);
void http_buffer_headers(http_request_t* request, http_response_t* response, grwprintf_t* printctx );
void hs_auto_detect_keep_alive(http_request_t* request);
void grwmemcpy(grwprintf_t* ctx, char const * src, int size);
void hs_add_write_event(http_request_t* request);
void hs_trigger_meta(http_parser_t* parser, int event);
void hs_stream_begin_token(hs_stream_t* stream, int token_type);
void hs_stream_shift(hs_stream_t* stream);
void hs_stream_anchor(hs_stream_t* stream);
void hs_stream_consume(hs_stream_t* stream);
void http_respond_chunk(http_request_t* request, http_response_t* response, void (*cb)(http_request_t*));
void http_respond_chunk_end(http_request_t* request, http_response_t* response);
void http_request_set_userdata(http_request_t* request, void* data);
void http_request_read_chunk(struct http_request_s* request, void (*chunk_cb)(struct http_request_s*));
void http_request_connection(http_request_t* request, int directive);
void* http_request_userdata(http_request_t* request);

http_token_t hs_stream_emit(hs_stream_t* stream);
http_token_t hs_stream_current_token(hs_stream_t* stream);
http_token_t http_parse(http_parser_t* parser, hs_stream_t* stream);
http_token_t hs_meta_emit(http_parser_t* parser);
http_token_t hs_transition_action(http_parser_t* parser, hs_stream_t* stream, char c, int8_t from, int8_t to );
http_string_t http_get_token_string(http_request_t* request, int token_type);
http_string_t http_request_header(http_request_t* request, char const * key);
http_string_t http_request_target(http_request_t* request);
http_string_t http_request_body(http_request_t* request);
http_string_t http_request_chunk(struct http_request_s* request);
http_server_t* http_server_init(int port, void (*handler)(struct http_request_s*));
http_response_t* http_response_init();

#endif //PHTTP_PHTTP_H
