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
// static int BUFF_SIZE = 256 * KILOBYTE;

int main(int argc, char* argv[]) {
    printf("initializing client...\n");

    struct addrinfo* serv_addrinfo = NULL;

    assert(argc == 3);
    const char* hostname = argv[1];
    const char* servname = argv[2];

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
    assert(serv_addrinfo != NULL);

    int server_fd = socket(serv_addrinfo->ai_family, serv_addrinfo->ai_socktype,
                           serv_addrinfo->ai_protocol);
    if (server_fd == -1) {
        fprintf(stderr, "Error allocating socket: %s\n", strerror(errno));
        exit(1);
    }

    const char* content = "Hello, world!";
    int bytes = sendto(server_fd, content, strlen(content), 0,
                       serv_addrinfo->ai_addr, serv_addrinfo->ai_addrlen);
    if (bytes == -1) {
        close(server_fd);
        fprintf(stderr, "Error sending: %s\n", strerror(errno));
        exit(1);
    }
    printf("sent %d bytes\n", bytes);

    freeaddrinfo(serv_addrinfo);
    close(server_fd);
}