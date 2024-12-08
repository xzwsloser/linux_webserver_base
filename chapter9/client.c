/* 客户端程序 */
#include "../utils/network.h"

int main(int argc , char** argv) {
    if(argc < 2) {
        error_handler("to many arguement to client!\n");
    }

    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    int connfd;
    int n ; /* 读取的次数 */
    char buf[1024];  /* 读取缓冲区大小 */

    connfd = Socket(AF_INET , SOCK_STREAM , 0);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET ;

    Connect(connfd , &serv_addr , sizeof(serv_addr));

    while(1) {
        scanf("%s" , buf);
        write(connfd , buf , strlen(buf));
        n = read(connfd , buf , sizeof(buf));
        if(n < 0) {
            error_handler("Read() failed!\n");
        } else if (n == 0) {
            printf("Read to an End !\n");
            break;
        }
        write(STDOUT_FILENO , buf , strlen(buf));
    }

    close(connfd);
}