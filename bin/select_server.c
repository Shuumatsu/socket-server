#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
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

    fd_set rset = {0}, eset = {0}, wset = {0};
    FD_SET(server_fd, &rset);
    FD_SET(server_fd, &eset);

    for (;;) {
        // Returns: positive count of descriptors ready, 0 on timeout, -1 error
        int nready = select(FD_SETSIZE, &rset, &wset, &eset, NULL);
        if (nready == -1) {
            fprintf(stderr, "Error selecting socket: %s\n", strerror(errno));
            exit(1);
        }
        assert(nready > 0);

        for (int fd = 0; fd < FD_SETSIZE; fd += 1) {
            if (FD_ISSET(fd, &rset) && fd == server_fd) {
                accept_req(server_fd);
            }
        }
    }
}
