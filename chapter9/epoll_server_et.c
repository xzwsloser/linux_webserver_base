#include "../utils/network.h"
#define MAX_EPOLL_EVENTS 1024

int main(int argc , char** argv) {
    if(argc < 2) {
        error_handler("to few arguement!");
    }

    int port = atoi(argv[1]);
    int listenfd ;
    int connfd;
    int sockfd;
    int enfd;
    int n ;
    int ret;
    int optVal = 1;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    struct epoll_event event;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    char buf[1024];
    char client[100];

    listenfd = Socket(AF_INET , SOCK_STREAM , 0);

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;

    Bind(listenfd , &serv_addr , sizeof(serv_addr));

    Listen(listenfd , 10);

    setsockopt(listenfd , SOL_SOCKET , SO_REUSEADDR , &optVal , sizeof(optVal));

    enfd = Epoll_create(MAX_EPOLL_EVENTS);

    event.data.fd = listenfd;
    event.events = EPOLLIN | EPOLLET ;
    setnoblocking(listenfd);        
    Epoll_ctl(enfd , EPOLL_CTL_ADD , listenfd , &event);

    while(1) {
        ret = Epoll_wait(enfd , events , MAX_EPOLL_EVENTS , -1);
        for(int i = 0 ; i < ret ; i ++) {
            sockfd = events[i].data.fd;
            if(sockfd == listenfd) {
                connfd = Accept(listenfd , &clnt_addr , &clnt_addr_len);
                printf("[client]%s:%d\n" , inet_ntop(AF_INET , (void*)&(clnt_addr.sin_addr.s_addr) , client , clnt_addr_len) , ntohs(clnt_addr.sin_port));
                setnoblocking(connfd);
                event.data.fd = connfd;
                event.events = EPOLLIN | EPOLLET ;
                Epoll_ctl(enfd , EPOLL_CTL_ADD , connfd , &event);
            } else {
                if(events[i].events & POLLIN) {
                    // 循环读取
                    while(1) {
                        n = read(sockfd , buf , sizeof(buf)) ;
                        if(n < 0) {
                            // 读取完毕
                            if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                                printf("Read end!\n");
                                break;
                            }
                        } else if(n == 0) {
                            Epoll_ctl(enfd , EPOLL_CTL_DEL , sockfd , NULL);
                            close(sockfd);
                            printf("Read to an End!\n");
                            break;
                        } else {
                            write(sockfd , buf , sizeof(buf));
                        }
                    }
                }
            }
        }
    } 
    close(listenfd);
}