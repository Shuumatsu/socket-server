#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "reqh.h"
#include "server.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    printf("initializing server...\n");

    assert(argc == 2);
    const char* servname = argv[1];

    struct addrinfo* serv_addrinfo = construct_tcp_serv_addrinfo(servname);
    assert(serv_addrinfo != NULL);
    int server_fd = initialize_server(serv_addrinfo);

    freeaddrinfo(serv_addrinfo);

    printf("server: waiting for connections...\n");

    int fd_count = 0, fd_size = 5;
    struct pollfd pfds[fd_size];

    pfds[0].fd = server_fd;
    pfds[0].events = POLLIN;
    fd_count += 1;

    for (;;) {
        int nready = poll(pfds, fd_count, -1);
        if (nready == -1) {
            fprintf(stderr, "Error selecting socket: %s\n", strerror(errno));
            exit(1);
        }
        assert(nready > 0);

        for (int i = 0; i < fd_count; i += 1) {
            if ((pfds[i].revents & POLLIN) && pfds[i].fd == server_fd) {
                accept_req(server_fd);
            }
        }
    }
}
