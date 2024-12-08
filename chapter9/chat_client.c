/* 聊天室 client */
#include "../utils/network.h"
#define MAX_EPOLL_SIZE 1024
int main(int argc , char** argv) {
    if(argc < 2) {
        error_handler("too few arguement to client!\n");
    }

    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr ;
    struct epoll_event events[2]; /* 监听服务器连接和 stdin */
    struct epoll_event event ;  
    struct epoll_event pre_events[2]; /* 准备好的连接 */
    int n ;
    int ret ;
    int epollfd ;
    int connfd ;
    int sockfd ;
    char buf[1024];

    connfd = Socket(AF_INET , SOCK_STREAM , 0);
    epollfd = Epoll_create(MAX_EPOLL_SIZE);
    
    serv_addr.sin_addr.s_addr = INADDR_ANY ;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET ;

    Connect(connfd , &serv_addr , sizeof(serv_addr));

    events[0].data.fd = connfd;
    events[0].events = EPOLLIN  ;
    events[1].data.fd = STDIN_FILENO ;
    events[1].events = EPOLLIN  ;
    Epoll_ctl(epollfd , EPOLL_CTL_ADD , connfd , &events[0]);
    Epoll_ctl(epollfd , EPOLL_CTL_ADD , STDIN_FILENO , &events[1]);

    while(1) {
        ret = Epoll_wait(epollfd , pre_events , MAX_EPOLL_SIZE , -1);
        for(int i = 0 ; i < ret ; i ++) {
            sockfd = pre_events[i].data.fd ;
            if(sockfd == events[0].data.fd) {
                if(pre_events[i].events & EPOLLIN) {
                        n = read(sockfd , buf , sizeof(buf));
                        if(n == 0) {
                            printf("Read to an End!\n");
                            exit(0);
                        } else if (n > 0) {
                            write(STDIN_FILENO , buf , n);
                        } else {
                            printf("Read() failed!\n");
                            exit(1);
                        }
                }
            } else if(sockfd == events[1].data.fd) {
                if(pre_events[i].events & EPOLLIN) {
                        n = read(sockfd , buf , sizeof(buf));
                        if(n == 0) {
                            printf("Read to an End!\n");
                            exit(0);
                        } else if (n > 0) {
                            write(connfd, buf , n);
                        } else {
                            printf("Read failed!\n");
                            exit(1);
                        }
                }
            }
        }
    }
}