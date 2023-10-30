#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstring>
#include <errno.h>   // for errno and strerror_r
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h>  //inet_addr
#include <netinet/tcp.h> //TCP_NODELAY
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
namespace{
const int kBufferSize=1500; 
static uint16_t HostToNet16(uint16_t x) { return __builtin_bswap16(x); }
static uint32_t HostToNet32(uint32_t x) { return __builtin_bswap32(x); }
static uint64_t HostToNet64(uint64_t x) { return __builtin_bswap64(x); }
static uint16_t NetToHost16(uint16_t x) { return HostToNet16(x); }
static uint32_t NetToHost32(uint32_t x) { return HostToNet32(x); }
static uint64_t NetToHost64(uint64_t x) { return HostToNet64(x); }
struct Request{
    Request(uint32_t u,uint32_t e,uint32_t s,uint32_t c):
    uuid(u),epoch(e),start(s),count(c){}
    uint32_t uuid;
    uint32_t epoch;
    uint32_t start;
    uint32_t count;
};
}

using namespace std;
int SendMessage(const char *ip,uint16_t port,Request *priv){
    int ret=-1;
    int sockfd=-1;
    char buffer[kBufferSize]={0};
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);
    int flag = 1;
    if ((sockfd= socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout<<"Error : Could not create socket"<<std::endl;
        return ret;
    }
    setsockopt (sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
    if (connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0) {
        std::cout<<"connection with the server failed"<<std::endl;
        close(sockfd);
        sockfd=-1;
        return ret;
    }
    int off=0;
    {
        uint32_t uuid=HostToNet32(priv->uuid);
        uint32_t epoch=HostToNet32(priv->epoch);
        uint32_t start=HostToNet32(priv->start);
        uint32_t count=HostToNet32(priv->count);
        uint16_t hz=sizeof(uuid)+sizeof(epoch)+sizeof(start)+sizeof(count);
        hz=HostToNet16(hz);
        memcpy(buffer+off,(void*)&hz,sizeof(hz));
        off+=sizeof(hz);
        memcpy(buffer+off,(void*)&uuid,sizeof(uuid));
        off+=sizeof(uuid);
        memcpy(buffer+off,(void*)&epoch,sizeof(epoch));
        off+=sizeof(epoch);
        memcpy(buffer+off,(void*)&start,sizeof(start));
        off+=sizeof(start);
        memcpy(buffer+off,(void*)&count,sizeof(count));
        off+=sizeof(count);
    }
    write(sockfd,buffer,off);
    close(sockfd);
    sockfd=-1;
    return 0;
}
static volatile bool g_running=true;
void signal_exit_handler(int sig)
{
    g_running=false;
}
using namespace std;
int main(int argc, char *argv[]){
    signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGHUP, signal_exit_handler);//ctrl+c
    signal(SIGTSTP, signal_exit_handler);//ctrl+z
    std::string remote_ip("127.0.0.1");
    uint16_t remote_port=2233;
    uint32_t agents=2;
    uint32_t epochs=4;
    uint32_t start=0;
    uint32_t count=10;
    for(uint32_t e=0;e<epochs;e++){
        for(uint32_t uuid=0;uuid<agents;uuid++){
            Request request(uuid,e,start,count);
            SendMessage(remote_ip.c_str(),remote_port,&request);
        }
    }
    return 0;
}
