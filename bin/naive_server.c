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
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "server.h"
#include "utils.h"

int req_hanlder(struct sockaddr_storage* client_addr, int client_fd) {
    const char* content = "Hello, world!\n";
    if (send(client_fd, content, strlen(content), 0) == -1) {
        fprintf(stderr, "Error sendong message: %s\n", strerror(errno));
    }

    close(client_fd);
    return 0;
}

void accept_req(int server_fd) {
    struct sockaddr_storage client_addr = {0};
    socklen_t addr_len = sizeof(struct sockaddr_storage);
    int client_fd =
        accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd == -1) {
        fprintf(stderr, "Error accepting request: %s\n", strerror(errno));
        exit(1);
    }

    printf("accept request from ");
    print_addr(&client_addr);
    printf("\n");

    fflush(stdout);

    int pid = fork();
    if (pid == -1) {
        close(client_fd);
        fprintf(stderr, "Error accepting request: %s\n", strerror(errno));
    } else if (pid == 0) {
        close(server_fd);
        exit(req_hanlder(&client_addr, client_fd));
    }
}

int main(int argc, char* argv[]) {
    printf("initializing server...\n");

    assert(argc == 2);
    const char* servname = argv[1];

    struct addrinfo* serv_addrinfo = construct_tcp_serv_addrinfo(servname);
    assert(serv_addrinfo != NULL);
    int server_fd = initialize_server(serv_addrinfo);

    freeaddrinfo(serv_addrinfo);

    printf("server: waiting for connections...\n");

    for (;;) { accept_req(server_fd); }

    close(server_fd);
    return 0;
}