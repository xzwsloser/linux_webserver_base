/* 利用 select 实现的并发服务器 */
#include "../utils/network.h"

int main(int argc , char** argv) {
    if(argc < 2) {
        error_handler("too many arguement to client!\n");
    } 

    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr) ;
    char buf[1024];
    char client[100];
    int n;
    int connfd; 
    int listenfd;  /* 表示监听 fd */
    int maxfds; /* 最大 fd + 1 */
    int ret; /* select 函数返回值 */
    int retVal = 1;
    fd_set all_set; /* 表示所有的集合 */
    fd_set old_set; /* 表示之前的集合 */

    listenfd = Socket(AF_INET  , SOCK_STREAM , 0);

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET ;

    Bind(listenfd , &serv_addr , sizeof(serv_addr));

    Listen(listenfd , 10);

    // 设置端口复用
    setsockopt(listenfd , SOL_SOCKET , SO_REUSEADDR , &retVal , sizeof(retVal));
    
    FD_ZERO(&all_set);
    FD_ZERO(&old_set);
    FD_SET(listenfd , &all_set);
    FD_SET(listenfd , &old_set);
    maxfds = listenfd;

    while(1) {
        old_set = all_set;
        ret = Select(maxfds + 1 , &old_set , NULL , NULL , NULL); 
        if(FD_ISSET(listenfd , &old_set)) {
            connfd = Accept(listenfd , &clnt_addr , &clnt_addr_len);
            if(connfd > maxfds) maxfds = connfd ;
            printf("[client]%s:%d\n" , inet_ntop(AF_INET , (void*)&(clnt_addr.sin_addr.s_addr) , client , clnt_addr_len) , ntohs(clnt_addr.sin_port));
            FD_SET(connfd , &all_set);
            if(--ret == 0) continue; /* 没有可以处理的文件描述符了 */
        } 

        for(int i = listenfd + 1 ; i <= maxfds ; i ++) {
            if(FD_ISSET(i , &old_set)) {
                n = read(i , buf , sizeof(buf));
                if(n < 0) {
                    error_handler("Read() failed!\n");
                } else if(n == 0) {
                    printf("Read to an End!\n");
                    FD_CLR(i , &all_set);
                } else {
                    // 进行转换逆转
                    write(i , buf , n);
                }
            }
        }
    }
}