/* 利用统一事件源处理信号 */
#include "network.h"

int pipefd[2] ;  /* 用于信号处理的管道 */
void sig_handler(int sig); /* 统一的信号处理函数 */
void addsig(int sig); /* 给信号添加信号处理函数 */
int main(int argc , char** argv) {
    if(argc < 2) {
        error_handler("too few arguments!");
    }
    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int sockfd , listenfd , epollfd ;
    int connfd ;
    int optval = 1;
    char buf[1024];
    char client[100];
    char sigset[100];
    struct epoll_event events[1024];
    struct epoll_event event;
    int stop_server = 0;
    int ret ;
    int n ;

    Pipe(pipefd);

    listenfd = Socket(AF_INET , SOCK_STREAM , 0);

    serv_addr.sin_addr.s_addr = INADDR_ANY ;
    serv_addr.sin_family = AF_INET ;
    serv_addr.sin_port = htons(port);

    Bind(listenfd , &serv_addr , sizeof(serv_addr));

    Listen(listenfd, 10);
    setsockopt(listenfd, SOL_SOCKET , SO_REUSEADDR , &optval , sizeof(optval));

    epollfd = Epoll_create(1024);
    Addfd(epollfd , listenfd);
    Addfd(epollfd , pipefd[0]);
    setnoblocking(pipefd[1]);

    addsig(SIGINT);
    addsig(SIGCHLD);
    addsig(SIGHUP);
    addsig(SIGTERM); // kill 默认发送的信号

    while(!stop_server) {
        ret = Epoll_wait(epollfd , events , 1024 , -1);
        if((ret < 0) && (errno != EINTR)) {
            printf("Epoll_wait() failed!");
            break;
        }

        for(int i = 0 ; i < ret ; i ++) {
           sockfd = events[i].data.fd ;
           if(sockfd == listenfd) {
               if(events[i].events & EPOLLIN) {
                    connfd = Accept(listenfd , &clnt_addr , &clnt_addr_len);
                    printf("[client]%s:%d\n" , inet_ntop(AF_INET , (void*)&(clnt_addr.sin_addr.s_addr) , client , clnt_addr_len) , ntohs(clnt_addr.sin_port)); 
                    Addfd(epollfd , connfd);
               }
           } else if(sockfd == pipefd[0]) {
               if(events[i].events & EPOLLIN) {
                    n = read(pipefd[0] , sigset , sizeof(sigset)); 
                    if(n < 0) {
                        printf("Read() failed!\n");
                        break;
                    } else if(n == 0) {
                        printf("Pipe() closed!\n");
                        break;
                    } else {
                        for(int i = 0 ; i < n ; i ++) {
                            switch (sigset[i]) {
                                case SIGHUP:
                                    printf("SIGHUP!\n");
                                    break;
                                case SIGCHLD:
                                    printf("SIGCHLD!\n");
                                    break;
                                case SIGINT:
                                    printf("SIGINT!\n");
                                    stop_server = 1;
                                    break;
                                case SIGTERM:
                                    printf("SIGTERM!\n");
                                    break;
                                default: 
                                    continue; 
                            }
                        }
                    }
               }
           } else {
                if(events[i].events & EPOLLIN) {
                    n = read(sockfd , buf , sizeof(buf));
                    if(n < 0) {
                        printf("read() failed!\n");
                        break;
                    } else if(n == 0) {
                        Epoll_ctl(epollfd , EPOLL_CTL_DEL , sockfd , NULL);                        
                        continue;
                    } else {
                        write(sockfd , buf , n);
                    }
                }
           } 
        }
    } 
}

void sig_handler(int sig) {
    int olderrno = errno ;
    int msg = sig ;
    write(pipefd[1] , (char*)&msg , 1);
    errno = olderrno ;  /* 保证函数的可重入性 */
}

void addsig(int sig) {
   struct sigaction sa;
   sa.sa_handler = sig_handler ;
   sa.sa_flags |= SA_RESTART ; 
   sigfillset(&sa.sa_mask); /* 填充所有信号 */
   Sigaction(sig , &sa , NULL);
}