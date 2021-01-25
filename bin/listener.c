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

// https://stackoverflow.com/questions/2862071/how-large-should-my-recv-buffer-be-when-calling-recv-in-the-socket-library
static int BUFF_SIZE = 1400;

int main(int argc, char* argv[]) {
    printf("initializing client...\n");

    assert(argc == 2);
    const char* servname = argv[1];

    struct addrinfo* serv_addrinfo = construct_udp_serv_addrinfo(servname);
    assert(serv_addrinfo != NULL);

    int server_fd = initialize_server(serv_addrinfo);

    printf("waiting to recvfrom...\n");

    char* buf = malloc(BUFF_SIZE);
    assert(buf != NULL);
    memset(buf, 0, BUFF_SIZE);

    struct sockaddr_storage client_addr = {0};
    socklen_t addr_len = sizeof(struct sockaddr);
    int bytes = recvfrom(server_fd, buf, BUFF_SIZE - 1, 0,
                         (struct sockaddr*)&client_addr, &addr_len);
    if (bytes == -1) {
        close(server_fd);
        fprintf(stderr, "Error recving: %s\n", strerror(errno));
        exit(1);
    }

    printf("got packet from ");
    print_addr(&client_addr);
    printf("\n%s\n", buf);

    free(buf);
    close(server_fd);
    freeaddrinfo(serv_addrinfo);
}
