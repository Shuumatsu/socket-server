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

#include "utils.h"

// https://stackoverflow.com/questions/2862071/how-large-should-my-recv-buffer-be-when-calling-recv-in-the-socket-library
static int BUFF_SIZE = 256 * KILOBYTE;

int main(int argc, char* argv[]) {
    printf("initializing client...\n");

    assert(argc == 3);
    const char* hostname = argv[1];
    const char* servname = argv[2];

    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* servinfo = NULL;

    int status = getaddrinfo(hostname, servname, &hints, &servinfo);
    if (status != 0) {
        const char* e = gai_strerror(status);
        fprintf(stderr, "getaddrinfo: %s\n", e);
        exit(1);
    }
    assert(servinfo != NULL);

    printf("requesting ");
    print_addr((struct sockaddr_storage*)servinfo->ai_addr);
    printf("\n");

    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
                        servinfo->ai_protocol);
    if (sockfd == -1) {
        fprintf(stderr, "Error allocating socket: %s\n", strerror(errno));
        exit(1);
    }

    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(sockfd);
        fprintf(stderr, "Error connecting socket: %s\n", strerror(errno));
        exit(1);
    }

    freeaddrinfo(servinfo);

    char* buf = malloc(BUFF_SIZE);
    memset(buf, 0, BUFF_SIZE);
    int bytes = recv(sockfd, buf, BUFF_SIZE - 1, 0);
    if (bytes == -1) {
        close(sockfd);
        fprintf(stderr, "Error recving: %s\n", strerror(errno));
        exit(1);
    }

    printf("client: received '%s'\n", buf);
    close(sockfd);
}