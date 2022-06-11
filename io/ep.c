//
// Created by parthka on 11/6/22.
//

// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "sys/epoll.h"
#include "fcntl.h"
#include "time.h"

#define PORT 8081

void hs_generate_date_time(char* datetime) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = gmtime(&rawtime);
    strftime(datetime, 32, "%a, %d %b %Y %T GMT", timeinfo);
}

int main(int argc, char const* argv[])
{
    int server_fd, sfd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    int epfd = epoll_create(10);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 125) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = (void*)&server_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

    while (1) {
        struct epoll_event ev[1];
        char res[1024] = "";
        strcat(res, "HTTP/1.1 200 OK\r\nDate: ");
        char t[200] = "";
        hs_generate_date_time(t);
        strcat(res,t);
        strcat(res, "\r\nContent-Type: text/plain");
        strcat(res, "\r\nContent-Length: 13\r\n\r\nHello, World!");
        int nev = epoll_wait(epfd, ev, 1, -1);
        for (int i = 0; i < nev; i++) {
            sfd = accept(*(int*)ev[i].data.ptr, (struct sockaddr *) &address, (socklen_t *) &addrlen);
            char buff[1024] = {};
            read(sfd, buff, sizeof (buff));
            write(sfd, res, strlen(res));
            close(sfd);

        }
    }
}

