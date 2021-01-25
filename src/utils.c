#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

void* get_in_addr(struct sockaddr_storage* addr) {
    switch (addr->ss_family) {
        case AF_INET: {
            return (void*)&(((struct sockaddr_in*)addr)->sin_addr);
        };
        case AF_INET6: {
            return (void*)&(((struct sockaddr_in6*)addr)->sin6_addr);
        };
        default: {
            fprintf(stderr, "[get_in_addr] unsupported addr->ss_family: %d\n",
                    addr->ss_family);
            exit(1);
        };
    }
}

in_port_t get_in_port(struct sockaddr_storage* addr) {
    switch (addr->ss_family) {
        case AF_INET: {
            return (((struct sockaddr_in*)addr)->sin_port);
        };
        case AF_INET6: {
            return (((struct sockaddr_in6*)addr)->sin6_port);
        };
        default: {
            fprintf(stderr, "[get_in_port] unsupported addr->ss_family: %d\n",
                    addr->ss_family);
            exit(1);
        };
    }
}

void print_addr(struct sockaddr_storage* addr) {
    char ipstr[INET6_ADDRSTRLEN];
    inet_ntop(addr->ss_family, get_in_addr(addr), ipstr, sizeof ipstr);
    printf("%s:%d", ipstr, ntohs(get_in_port(addr)));
}
