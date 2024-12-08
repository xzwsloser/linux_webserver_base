/* 利用 poll 实现并发服务器 */
#include "../utils/network.h"

int main(int argc , char** argv) {
    if(argc < 2) {
        error_handler("too few arguement to poll server!");
    }

    int port = atoi(argv[1]);
    int listenfd ;
    int connfd;
    int maxIndex;
    int ret;
    int curIndex;
    int sockfd;
    int n;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    struct pollfd fds[1024];
    nfds_t nfds = 1;  /* 集合中文件描述符的数量 */
    char client[100];
    char buf[1024];

    listenfd = Socket(AF_INET , SOCK_STREAM , 0);

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;
    Bind(listenfd , &serv_addr , sizeof(serv_addr));

    Listen(listenfd , 10);

    for(int i = 0 ; i < 1024 ; i ++) {
        fds[i].fd = -1;
        fds[i].events = 0;
        fds[i].revents = 0;
    }

    fds[0].fd = listenfd ;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    maxIndex =  0;

    while(1) {
        ret = Poll(fds , nfds , -1);       
        if(fds[0].revents & POLLIN) {
            connfd = Accept(listenfd , &clnt_addr, &clnt_addr_len);
            printf("[client]%s:%d\n" , inet_ntop(AF_INET , (void*)&(clnt_addr.sin_addr.s_addr) , client , clnt_addr_len) , ntohs(clnt_addr.sin_port));
            // 找到空闲位置
            for(int i = 0 ; i < 1024 ; i ++) {
                if(fds[i].fd < 0) {
                    fds[i].fd = connfd;
                    fds[i].events = POLLIN;
                    fds[i].revents = 0;    
                    curIndex = i;
                    break;
                }
            }
            if(curIndex > maxIndex) maxIndex = curIndex;
            nfds ++;
            if(--ret == 0) continue;
        }

        for(int i = 1 ; i <= maxIndex ; i ++) {
            if((sockfd = fds[i].fd) < 0) {
                continue;
            }

            if(fds[i].revents & POLLIN) {
                n = read(sockfd , buf , sizeof(buf));
                if(n == 0) {
                    printf("Read to an End!\n");
                    fds[i].fd = 0;
                } else if(n == -1) {
                    error_handler("Read() failed!");
                } else {
                    write(sockfd , buf , n);
                }
            }
        }

    }

}