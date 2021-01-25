#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdio.h>  // for fprintf()
#include <stdlib.h>
#include <string.h>  // for strncmp
#include <sys/epoll.h>  // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <unistd.h>  // for close(), read()

#include "reqh.h"
#include "server.h"
#include "utils.h"

#define MAX_EVENTS 65535

int main(int argc, char* argv[]) {
    printf("initializing server...\n");

    assert(argc == 2);
    const char* servname = argv[1];

    struct addrinfo* serv_addrinfo = construct_tcp_serv_addrinfo(servname);
    assert(serv_addrinfo != NULL);
    int server_fd = initialize_server(serv_addrinfo);
    freeaddrinfo(serv_addrinfo);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        close(server_fd);
        fprintf(stderr,
                "[epoll_create1] Failed to create epoll file descriptor: %s\n",
                strerror(errno));
        exit(1);
    }

    struct epoll_event event = {.events = EPOLLIN, .data = {.fd = server_fd}};
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event)) {
        close(server_fd);
        close(epoll_fd);
        fprintf(stderr, "[epoll_ctl] Failed to add server_fd to epoll: %s\n",
                strerror(errno));
        exit(1);
    }

    printf("server: waiting for connections...\n");
    struct epoll_event events[MAX_EVENTS];
    for (;;) {
        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (event_count == -1) {
            fprintf(stderr, "[epoll_wait]: %s\n", strerror(errno));
            break;
        }

        for (int i = 0; i < event_count; i++) {
            struct epoll_event event = events[i];
            if (event.data.fd == server_fd) {
                assert((event.events & EPOLLIN) != 0);

                struct client_req* req = malloc(sizeof(struct client_req));
                accept_req_error code = accept_req(server_fd, req);
                if (code != ACR_NO_ERROR) {
                    fprintf(stderr, "Error accepting request: %d\n", code);
                    continue;
                }

                struct epoll_event event = {.events = EPOLLOUT,
                                            .data = {.ptr = req}};
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, req->client_fd,
                              &event) == -1) {
                    fprintf(
                        stderr,
                        "[epoll_ctl] Failed to add client_fd %d to epoll: %s\n",
                        req->client_fd, strerror(errno));
                    free_client_req(req);
                }
            } else {
                // for simplicity, ignore all other situations
                if (event.events & EPOLLOUT) {
                    struct client_req* req = event.data.ptr;
                    req_hanlder_error _code = req_hanlder(req);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, req->client_fd, NULL);
                    close(req->client_fd);
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}
