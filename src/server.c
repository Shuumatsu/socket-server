#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

int initialize_server(struct addrinfo* serv_addrinfo) {
    int server_fd = socket(serv_addrinfo->ai_family, serv_addrinfo->ai_socktype,
                           serv_addrinfo->ai_protocol);
    if (server_fd == -1) {
        fprintf(stderr, "[initialize_server] allocate socket failed: %s\n",
                strerror(errno));
        exit(1);
    }

    int enable = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) ==
        -1) {
        fprintf(stderr,
                "[initialize_server] setsockopt(SO_REUSEADDR) failed: %s\n",
                strerror(errno));
        exit(1);
    }

    if (bind(server_fd, serv_addrinfo->ai_addr, serv_addrinfo->ai_addrlen) ==
        -1) {
        close(server_fd);
        fprintf(stderr, "[initialize_server] bind socket failed: %s\n",
                strerror(errno));
        exit(1);
    }

    if (listen(server_fd, BACKLOG) == -1) {
        close(server_fd);
        fprintf(stderr, "[initialize_server] listen socket failed: %s\n",
                strerror(errno));
        exit(1);
    }

    printf("[initialize_server] initialized server at ");
    print_addr((struct sockaddr_storage*)serv_addrinfo->ai_addr);
    printf("\n");

    return server_fd;
}

struct addrinfo* construct_tcp_serv_addrinfo(const char* servname) {
    struct addrinfo* serv_addrinfo = NULL;

    // 一个主机名或者地址串(IPv4的点分十进制串或者IPv6的16进制串)
    const char* hostname = NULL;

    // 服务名可以是十进制的端口号，也可以是已定义的服务名称，如ftp、http等
    // char servname[(int)((ceil(log10(PORT)) + 1) * sizeof(char))];
    // sprintf(servname, "%d", PORT);

    // If the AI_PASSIVE flag is specified in hints.ai_flags, and hostname is
    // NULL, then the returned socket addresses will be suitable for bind(2)ing
    // a socket that will accept(2) connections. The returned socket address
    // will contain the "wildcard address" [...]. If hostname is not NULL, then
    // the AI_PASSIVE flag is ignored.

    // If the AI_PASSIVE flag is not set in hints.ai_flags,
    // then the returned socket addresses will be suitable for use with
    // connect(2), sendto(2), or sendmsg(2). If hostname is NULL, then the
    // network address will be set to the loopback interface address [...].

    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;  // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;  // assign the address of my local host to the

    int status = getaddrinfo(hostname, servname, &hints, &serv_addrinfo);
    if (status != 0) {
        const char* e = gai_strerror(status);
        fprintf(stderr, "getaddrinfo: %s\n", e);
        exit(1);
    }

    return serv_addrinfo;
}

struct addrinfo* construct_udp_serv_addrinfo(const char* servname) {
    struct addrinfo* serv_addrinfo = NULL;

    const char* hostname = NULL;

    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(hostname, servname, &hints, &serv_addrinfo);
    if (status != 0) {
        const char* e = gai_strerror(status);
        fprintf(stderr, "getaddrinfo: %s\n", e);
        exit(1);
    }

    return serv_addrinfo;
}