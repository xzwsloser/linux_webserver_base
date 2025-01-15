#pragma once
#include<iostream>
#include<pthread.h>
using namespace std;

class CycleQueue {
public:
    CycleQueue() {
        front = 0;
        rear = 0;
        pthread_mutex_init(&mutex , nullptr);
        pthread_cond_init(&full_cond , nullptr);
        pthread_cond_init(&empty_cond , nullptr);
    }

    ~CycleQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&full_cond);
        pthread_cond_destroy(&empty_cond);
    }

    void insert(int val,int index); 
    int pop(int index);  
    bool empty();
    bool full();
private:
    int queue[10];
    int front ; /* 指向第一个元素 */
    int rear;  /* 指向最后一个元素的后面一个元素 */
    pthread_mutex_t mutex;
    pthread_cond_t full_cond;
    pthread_cond_t empty_cond;
};
