/* 时间轮算法 */
#pragma once
#include<time.h>
#include "network.h"
#define  BUFSIZE 100 
class tw_timer ;  /* 定时器对象 */

struct client_data {
    struct sockaddr_in address; /* 客户端地址信息 */
    int sockfd;  /* 通信套接字 */
    tw_timer* timer; /* 定时器对象 */
    char buf[BUFSIZE]; /* 缓冲区 */
} ;

class tw_timer {
public:
    tw_timer(int rot , int ts):
        rotation(rot) , time_slot(ts) , prev(nullptr) , next(nullptr) {};
public:
    int rotation ;  /* 时间轮转到这一个节点需要的圈数 */
    int time_slot ;  /* 定时器位于时间轮的哪一个槽上 */
    client_data* user; /* 用户对象,用于回调函数参数 */
    void(*cb_func)(client_data*); /* 回调函数 */
    tw_timer* prev;
    tw_timer* next; 
} ;

class time_wheel {
public:
    time_wheel();  /*  构造函数 */ 
    ~time_wheel();  /* 析构函数 */
    tw_timer* add_timer(int timeout);  /* 为超时时间添加一个 定时器对象并且插入到时间槽中 */
    void del_timer(tw_timer* timer);  /* 删除指定位置的时间槽定时器 */
    void adjust_timer(tw_timer* timer , int timeout);  /* 调整定时器位置 */
    void tick();  /* 每一次 slot 指针移动需要触发,注意这一个函数会在信号处理函数(or统一时间源中触发) */
private:
    static const int N = 60;  /* 总共的槽位数量 */
    static const int SI = 1;  /* 每一次 tick 的时间间隔 */
    tw_timer* slots[N];  /* 每一个槽子的头节点集合 */
    int cur_slot ;  /* 目前的槽位 */
} ;