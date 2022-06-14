//
// Created by parthka on 14/6/22.
//

#include "../src/phttp.h"

#define RESPONSE "Hello, World!"

void handle_request(/*struct http_request_s* request*/) {
    // struct http_response_s* response = http_response_init();
    // http_response_status(response, 200);
    // http_response_header(response, "Content-Type", "text/plain");
    // http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
    // http_respond(request, response);
}

int main() {
    printf("Server Start On 8080...\n");
    printf("http://localhost:8080/\n");
    //struct http_server_s* server = http_server_init(8080, handle_request);
    // http_server_listen(server);
}