#include <netinet/in.h>

#define BACKLOG 10

#define KILOBYTE 1024
#define MEGABYTE (1024 * KILOBYTE)
#define GIGABYTE (1024 * MEGABYTE)
#define TERABYTE (1024 * GIGABYTE)

void* get_in_addr(struct sockaddr_storage* addr);

in_port_t get_in_port(struct sockaddr_storage* addr);

void print_addr(struct sockaddr_storage* addr);
