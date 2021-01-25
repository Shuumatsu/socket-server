#include <netdb.h>

struct client_req {
    struct sockaddr_storage client_addr;
    int client_fd;
};

void free_client_req(struct client_req* req);

typedef enum { ACR_NO_ERROR, ACR_ACCEPT_ERROR } accept_req_error;
accept_req_error accept_req(int server_fd, struct client_req* req);

typedef enum { REQH_NO_ERROR, SEND_ERROR } req_hanlder_error;
req_hanlder_error req_hanlder(struct client_req* req);