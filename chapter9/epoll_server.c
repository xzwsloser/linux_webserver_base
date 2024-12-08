/* epoll 实现的服务器 */
#include "../utils/network.h"
#define MAX_EVENTS_NUMBER 1024

int main(int argc , char** argv) {
    if(argc < 2) {
        error_handler("too few arguement to epoll server!");
    }

    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int n ;
    char buf[1024];
    char client[100];
    struct epoll_event event;  /* 用于注册监听时间 */
    struct epoll_event events[MAX_EVENTS_NUMBER];  /* 传入参数,用于收集准备好的事件 */
    int listenfd ;
    int connfd ;
    int enfd;
    int sockfd;
    int ret;

    listenfd = Socket(AF_INET , SOCK_STREAM , 0);

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;

    Bind(listenfd , &serv_addr , sizeof(serv_addr));

    Listen(listenfd , 1024);

    enfd = Epoll_create(MAX_EVENTS_NUMBER);

    event.events = EPOLLIN;
    event.data.fd = listenfd;

    // 注意添加内核事件
    Epoll_ctl(enfd , EPOLL_CTL_ADD , listenfd , &event);

    while(1) {
        ret = Epoll_wait(enfd , events , MAX_EVENTS_NUMBER , -1);
        printf("ret = %d\n" , ret);
        for(int i = 0 ; i < ret ; i ++) {
            if((sockfd = events[i].data.fd) == listenfd) {
                connfd = Accept(listenfd , &clnt_addr , &clnt_addr_len);
                printf("[client]%s:%d\n" , inet_ntop(AF_INET , (void*)&(clnt_addr.sin_addr.s_addr) , client , clnt_addr_len) , ntohs(clnt_addr.sin_port));
                event.data.fd = connfd;
                event.events = EPOLLIN;
                Epoll_ctl(enfd , EPOLL_CTL_ADD , connfd , &event);
            } else {
                n = read(sockfd , buf , sizeof(buf));
                if(n < 0) {
                    error_handler("Read() failed!");
                } else if(n == 0) {
                    printf("Read to an End!\n");
                    Epoll_ctl(enfd , EPOLL_CTL_DEL , sockfd , NULL);
                } else {
                    write(sockfd , buf , n);
                }
            }
        }
    } 
}