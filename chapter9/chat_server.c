/* 聊天室 server */
#include "../utils/network.h"
#define CLIENT_LEN 1024
#define MAX_CLIENT_NUM 10 /* 最大客户端数量 */
#define MAX_EPOLL_SIZE 1024 /* 最大 epoll 连接数量 */
typedef struct {
    char buf[1024];
    int sockfd ;  /* 通信套接字 */
} client ;  /* 客户信息结构体 */

int main(int argc , char** argv) {
    if(argc < 2) {
        error_handler("too few arguement to server!");
    }

    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int n ;
    int ret;
    int epollfd ;
    int listenfd ;
    int connfd ;
    int sockfd ;
    int optVal = 1;
    struct epoll_event events[MAX_EPOLL_SIZE]; /* 用于监听的 epoll 事件 */
    struct epoll_event event ;  /* 单个 epoll 事件 */
    client clients[MAX_CLIENT_NUM];  /* 表示客户列表 */    
    char client_buf[100];

    listenfd = Socket(AF_INET , SOCK_STREAM , 0);
    epollfd = Epoll_create(MAX_EPOLL_SIZE);

    serv_addr.sin_addr.s_addr = INADDR_ANY ;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET ;

    Bind(listenfd , &serv_addr , sizeof(serv_addr));

    Listen(listenfd , MAX_CLIENT_NUM);

    event.data.fd = listenfd ;
    event.events = EPOLLIN  ;
    Epoll_ctl(epollfd , EPOLL_CTL_ADD , listenfd ,&event);

    for(int i = 0 ; i < MAX_CLIENT_NUM; i ++) {
        clients[i].sockfd = -1;
    }

    setsockopt(listenfd , SOL_SOCKET , SO_REUSEADDR , &optVal , sizeof(optVal));
    
    while(1) {
        ret = Epoll_wait(epollfd , events , MAX_EPOLL_SIZE , -1);
        for(int i = 0 ; i < ret ; i ++) {
            sockfd = events[i].data.fd ;
            if(sockfd == listenfd) {
                if(events[i].events & EPOLLIN) {
                    connfd = Accept(listenfd , &clnt_addr , &clnt_addr_len);
                    printf("[client]%s:%d\n" , inet_ntop(AF_INET , (void*)&clnt_addr.sin_addr.s_addr , client_buf , sizeof(client_buf)) , ntohs(clnt_addr.sin_port)); 
                    int i ;
                    for(i = 0 ; i < MAX_CLIENT_NUM ; i ++) {
                        if(clients[i].sockfd < 0) {
                            clients[i].sockfd = connfd ;
                            break;
                        }
                    }
                    if(i == MAX_CLIENT_NUM) {
                        printf("connection fulled !\n");
                        continue;
                    }
                    event.data.fd = connfd;
                    event.events = EPOLLIN  ;
                    Epoll_ctl(epollfd , EPOLL_CTL_ADD , connfd , &event);
                }
            } else {
                client* curClient;
                for(int i = 0 ; i < MAX_CLIENT_NUM ; i ++) {
                    if(clients[i].sockfd == sockfd) {
                        curClient = &clients[i];
                        break;
                    }
                }

                n = read(sockfd , curClient -> buf , sizeof(sizeof(curClient -> buf)));
                if(n == 0) {
                    curClient -> sockfd = -1;
                    memset(curClient -> buf , 0 , sizeof(curClient -> buf)); 
                    printf("Read to an End!\n");
                    Epoll_ctl(epollfd , EPOLL_CTL_DEL , sockfd ,NULL);
                    close(sockfd);
                    break;
                } else if(n < 0) {
                    printf("Read failed!\n");
                    exit(1);
                } else {   
                    for(int i = 0 ; i < MAX_CLIENT_NUM ; i ++) {
                        if(clients[i].sockfd == sockfd || clients[i].sockfd < 0) {
                            continue;
                        } else {
                            write(clients[i].sockfd , curClient -> buf , n);
                        }
                    } 
                }
            }
        }  
    }
}