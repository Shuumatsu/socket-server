#include <netdb.h>

struct addrinfo* construct_tcp_serv_addrinfo(const char* servname);

struct addrinfo* construct_udp_serv_addrinfo(const char* servname);

int initialize_server(struct addrinfo* serv_addrinfo);
