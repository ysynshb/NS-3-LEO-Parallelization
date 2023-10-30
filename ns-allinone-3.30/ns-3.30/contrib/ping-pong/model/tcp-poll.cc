#include <iostream>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h>  //inet_addr
#include <netdb.h>
#include "tcp-poll.h"
namespace tcp{
int setnonblocking(int fd)
{
    if (-1==fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK)){
        return -1;
    }
    return 0;
}
void epoll_ctl_add(int epfd, int fd,uint32_t events)
{
    struct epoll_event ev;
    ev.events = events|EPOLLERR|EPOLLHUP;
    ev.data.fd=fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev)==-1) {
        std::cout<<"epoll_ctl_add: "<<strerror(errno)<<std::endl;
        exit(1);
    }
}
void epoll_ctl_mod(int epfd, int fd,uint32_t events)
{
    struct epoll_event ev;
    ev.events = events|EPOLLERR|EPOLLHUP;
    ev.data.fd=fd;
    if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev)==-1) {
        std::cout<<"epoll_ctl_mod: "<<strerror(errno)<<std::endl;
        exit(1);
    }
}
void epoll_ctl_del(int epfd, int fd){
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev)==-1) {
        std::cout<<"epoll_ctl_del: "<<strerror(errno)<<std::endl;
        exit(1);
    }
}
int create_listen_fd(const char *ip,uint16_t port,int backlog){
    int fd=-1;
    int yes=1;
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr =inet_addr(ip);
    servaddr.sin_port = htons(port);
    size_t addr_size = sizeof(struct sockaddr_in);
    fd=socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0){
        return fd;
    }
    if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))!=0){
        close(fd);
        fd=-1;
        return fd;
    }
    if(setsockopt(fd,SOL_SOCKET, SO_REUSEPORT,(const void *)&yes ,sizeof(int))!=0){
        close(fd);
        fd=-1;
        return fd;        
    }
    if(bind(fd, (struct sockaddr *)&servaddr, addr_size)<0){
        std::cout<<"bind failed"<<strerror(errno)<<std::endl;
        close(fd);
        fd=-1;
        return fd;
    }
    if(-1==listen(fd,backlog)){
        close(fd);
        fd=-1;
        return fd;
    }
    return fd;
}
}
