/* Libevent 库使用实例 */ 
/* 如果需要添加定时事件,可以在client_data中添加一个属性timeval并且添加定时事件 */
#include <event2/event.h>
#include <event2/event_compat.h>
#include<stdio.h>
#include<event.h>
#include<sys/signal.h>
#include <sys/socket.h>
#include "../utils/network.h"

struct client_data {
    struct event* client_ev;
    struct sockaddr_in client_addr;
    int sockfd;
    char buf[100];   
};

void signal_cb(int fd , short event , void* argc); /* 信号回调函数 */
void io_cb(int fd  , short event , void* argc); /* io 事件回调函数 */
void client_cb(int fd , short event , void* argc); /* 客户读写回调函数 */

struct client_data users[100]; /* 用户信息数组 */
struct event_base* base;  /* 使用的 Reactor */

int main(int argc , char** argv) {
    if(argc < 2) {
        error_handler("<usage> too few arguement!");
    }

    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    int lfd;
    
    lfd = Socket(AF_INET , SOCK_STREAM , 0);

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET ;
    Bind(lfd , &serv_addr , sizeof(serv_addr));

    Listen(lfd , 1024);

    for(int i = 0 ; i < 100 ; i ++) {
        users[i].sockfd = -1; 
        users[i].client_ev = NULL;
    }

    // 1. 创建 event_base
    base = event_init();  /* 创建 Reactor 对象 */
    // 2. 创建事件对象
    struct event* signal_ev = event_new(base , SIGINT , EV_SIGNAL|EV_PERSIST , signal_cb , NULL);
    struct event* listen_ev = event_new(base , lfd , EV_READ|EV_PERSIST ,io_cb , NULL);
    // 3. 添加到事件处理队列中
    event_add(signal_ev , NULL);
    event_add(listen_ev , NULL);
    // 4. 开始循环
    event_base_dispatch(base);
    // 5. 关闭资源
    event_free(signal_ev);
    event_free(listen_ev);
    for(int i = 0 ; i < 100 ; i ++) {
        if(users[i].client_ev != NULL) {
            event_free(users[i].client_ev);
        }
    }
    event_base_free(base);
    return 0;
}

void signal_cb(int fd , short event , void* argc) 
{
    printf("Catch signal: %d\n" , fd);
}

void io_cb(int fd , short event , void* argc)
{
    char buf[100];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int cfd = Accept(fd , &client_addr , &client_addr_len);  
    printf("Connect to:[client]%s:%d\n" , 
           inet_ntop(AF_INET , (void*)&client_addr.sin_addr.s_addr , buf , client_addr_len) , 
           ntohs(client_addr.sin_port));
    int index = 0;
    for(int i = 0 ; i < 100 ; i ++) {
        if(users[i].sockfd < 0) {
            index = i;
            break;
        }
    }
    users[index].sockfd = cfd;
    users[index].client_addr = client_addr;
    users[index].client_ev = event_new(base , cfd , EV_READ|EV_PERSIST , client_cb , (void*)index);
    event_add(users[index].client_ev, NULL);
}

void client_cb(int fd , short event , void* argc)
{
    int n ;
    int index = (int)argc;
    if(event & EV_READ) {
        n = read(fd , users[index].buf , sizeof(users[index].buf)); 
        if(n == -1) {
            printf("read failed!: %d\n" , fd);
            return ;
        } else if(n == 0) {
           event_del(users[index].client_ev);
           event_free(users[index].client_ev);
           users[index].sockfd = -1;
           users[index].client_ev = NULL;
        }
        printf("receive from client[%d]: %s\n" , fd , users[index].buf);
    } 
}
