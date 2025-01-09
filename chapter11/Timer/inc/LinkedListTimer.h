/* 基于升序链表的定时器 */
#pragma once
#include<time.h>
#include<sys/socket.h>
#include "network.h"
#define BUFSIZE 64 
class util_timer;  /* 定时器类前置声明 */
struct client_data;

struct client_data {
    struct sockaddr_in address; /* 客户端 IP 地址信息 */
    char buf[BUFSIZE];  /* 读写缓冲区 */
    int sockfd ;  /* 连接文件描述符 */
    util_timer* timer;  /* 绑定的定时器对象 */
} ;

class util_timer {
public:
    util_timer(): prev(nullptr) , next(nullptr) {}
public:
    time_t expire;   /* 定时器过期时间 */ 
    util_timer* prev ;  /* 节点的前面一个节点 */
    util_timer* next;  /* 节点的后面一个节点 */
    client_data* client ; /* 用户对象 */
    void(*callback)(client_data*);   /* 定时回调函数 */
} ;

/* 升序链表对象 */
class sort_timer_list {
private:
    util_timer* head;  /* 头节点 */
    util_timer* tail;  /* 尾节点 */
    void add_timer(util_timer* timer , util_timer* front); /* 加入 timer 到 front 的后面部分 */
public:
    sort_timer_list() : head(nullptr) , tail(nullptr) {}
    ~sort_timer_list(); /* 析构函数 */
    void add_timer(util_timer* timer);  /* 加入 定时器对象到链表中 */
    void adjust_timer(util_timer* timer); /* 当过期时间发生变化的时候调整定时器位置 */
    void del_timer(util_timer* timer); 
    void tick(); /* 定接收到 SIGALARM 信号的时候信号处理函数中调用这一个函数 */  
};