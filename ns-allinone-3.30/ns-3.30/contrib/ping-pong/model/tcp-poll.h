#pragma once
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
namespace tcp{
inline uint16_t HostToNet16(uint16_t x) { return __builtin_bswap16(x); }
inline uint32_t HostToNet32(uint32_t x) { return __builtin_bswap32(x); }
inline uint64_t HostToNet64(uint64_t x) { return __builtin_bswap64(x); }
inline uint16_t NetToHost16(uint16_t x) { return HostToNet16(x); }
inline uint32_t NetToHost32(uint32_t x) { return HostToNet32(x); }
inline uint64_t NetToHost64(uint64_t x) { return HostToNet64(x); }

int setnonblocking(int fd);
void epoll_ctl_add(int epfd, int fd,uint32_t events);
void epoll_ctl_mod(int epfd, int fd,uint32_t events);
void epoll_ctl_del(int epfd, int fd);
int create_listen_fd(const char *ip,uint16_t port,int backlog);

}
