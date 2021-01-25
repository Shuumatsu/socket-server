#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "reqh.h"
#include "utils.h"

void free_client_req(struct client_req* req) {
    if (close(req->client_fd) == -1) {
        // but nothing we need to do
        fprintf(stderr, "[free_client_req] Failed to close client_fd %d: %s\n",
                req->client_fd, strerror(errno));
    }
    free(req);
}

accept_req_error accept_req(int server_fd, struct client_req* req) {
    struct sockaddr_storage* client_addr = &req->client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_storage);
    req->client_fd =
        accept(server_fd, (struct sockaddr*)client_addr, &addr_len);
    if (req->client_fd == -1) {
        fprintf(stderr, "[accept_req] Failed to accept request: %s\n",
                strerror(errno));
        return ACR_ACCEPT_ERROR;
    }

    printf("accept request from ");
    print_addr(&req->client_addr);
    printf("\n");

    return ACR_NO_ERROR;
}

req_hanlder_error req_hanlder(struct client_req* req) {
    const char* content = "Hello, world!\n";
    if (send(req->client_fd, content, strlen(content), 0) == -1) {
        fprintf(stderr,
                "[req_hanlder] error sending message to client_fd %d: %s\n",
                req->client_fd, strerror(errno));
        return SEND_ERROR;
    }
    return ACR_NO_ERROR;
}
