`struct sockaddr_in` 是 ipv4 的标准格式
```c
struct sockaddr_in {
    __SOCKADDR_COMMON(sin_);
    in_port_t sin_port; /* Port number.  */
    struct in_addr sin_addr; /* Internet address.  */

    /* Pad to size of `struct sockaddr'.  */
    unsigned char sin_zero[sizeof(struct sockaddr) - __SOCKADDR_COMMON_SIZE -
                           sizeof(in_port_t) - sizeof(struct in_addr)];
};

// sizeof(struct sockaddr) = 16
```
`unsigned char sin_zero` 是为了保证这个结构体和struct sockaddr的大小一致。

`struct sockaddr_in6` 是 ipv6 的标准格式
```c
struct sockaddr_in6 {
    __SOCKADDR_COMMON(sin6_);
    in_port_t sin6_port; /* Transport layer port # */
    uint32_t sin6_flowinfo; /* IPv6 flow information */
    struct in6_addr sin6_addr; /* IPv6 address */
    uint32_t sin6_scope_id; /* IPv6 scope-id */
};

// sizeof(struct sockaddr) = 28
```

`struct sockaddr` 是 linux 网络编程接口中用来表示 IP 地址的标准结构体，`bind`, `connect` 等函数中都会用到这个结构体。
```c
#define	__SOCKADDR_COMMON(sa_prefix) sa_family_t sa_prefix##family

struct sockaddr {
    __SOCKADDR_COMMON(sa_); /* Common data: address family and length.  */
    char sa_data[14]; /* Address data.  */
};

// sizeof(struct sockaddr) = 16
```

显然 `struct sockaddr` 这个通用的数据结构大小不足以装下 `struct sockaddr_in6`，所以 `struct sockaddr_storage` 这个新结构横空出世了，应该能装得下目前所以协议的地址结构了。
```c
#define __ss_aligntype unsigned long int
#define _SS_PADSIZE (_SS_SIZE - __SOCKADDR_COMMON_SIZE - sizeof(__ss_aligntype))

struct sockaddr_storage {
    __SOCKADDR_COMMON(ss_); /* Address family, etc.  */
    char __ss_padding[_SS_PADSIZE];
    __ss_aligntype __ss_align; /* Force desired alignment.  */
};

// sizeof(struct sockaddr_storage) = 28
```

---

```c
struct pollfd {
	int fd;
	short events; // bitmap of events we're interested in   
        // POLLIN: data is ready to recv() on this socket
        // or POLLOUT: can send() data to this socket without blocking
	short revents; // when poll() returns, bitmap of events that occurred
};

// returns the number of elements that are ready
int poll(struct pollfd fds[], nfds_t nfds, int timeout);
```

当 poll 返回的时候就可以通过检查 revents to see if POLLIN or POLLOUT is set, indicating that event occurred.


---

```c
// Returns: positive count of descriptors ready, 0 on timeout, -1 error
extern int select(int __nfds,
                  fd_set* __restrict __readfds,
                  fd_set* __restrict __writefds,
                  fd_set* __restrict __exceptfds,
                  struct timeval* __restrict __timeout);
```

---
epoll 的核心是 the epoll instance,  an  in-kernel  data  structure。从 user-space 的角度，可以看作 a container for two lists:
- the interest list
- the ready list: the set of file descriptors that are "ready" for I/O.

In edge triggered mode we will only receive events when the state of the watched file descriptors change
in level triggered mode we will continue to receive events until the underlying file descriptor is no longer in a ready state