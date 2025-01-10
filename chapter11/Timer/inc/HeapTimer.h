/* 时间堆管理定时器 */
#pragma once
#include<iostream>
#include "network.h"
#include "time.h"
#define BUFSIZE 200

class heap_timer;

struct client_data {
    struct sockaddr_in address;
    int sockfd;
    heap_timer* timer;
    char buf[BUFSIZE];
};

class heap_timer {
public:
    heap_timer(int delay)
    {
        this -> expire = delay + time(nullptr);
    }
public:
    int expire;  /* 定时器过期时间 */
    void (*cb_func)(client_data*);  /* 定时器回调函数 */
    client_data* user;  /* 回调函数的参数 */
}; 


class time_heap {
public:
    time_heap(int cap);   /* 根据容量构造 */
    time_heap(heap_timer** init_array , int size , int cap);  /* 根据已经有的数组构造 */
    ~time_heap();   /* 析构函数 */
    void add_timer(heap_timer* timer); /* 添加定时器 */
    void del_timer(heap_timer* timer);  /* 删除定时器 */
    heap_timer* top() const;  /* 获取到过期时间最短的定时器 */
    void pop_timer();   /* 弹出堆顶定时器 */
    void tick();  /* 心搏函数 */
    bool empty() const;   /* 返回容量,注意一定需要时常对象 */
private:
    void percolate_down(int hole);  /* 把 hole 处的元素下沉到指定位置 */
    void resize();  /* 扩大容量 */
    heap_timer** array;  /* 底层的定时器堆 */
    int capacity ;   /* 堆的容量 */
    int cur_size;  /* 堆现在的大小 */
};