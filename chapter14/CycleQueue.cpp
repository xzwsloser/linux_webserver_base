#include "CycleQueue.h"
#include <pthread.h>

void CycleQueue::insert(int val,int index) 
{
    pthread_mutex_lock(&this -> mutex);
    while(this -> full()) {
       pthread_cond_wait(&this -> full_cond, &this ->mutex); 
    }    
    this -> queue[rear] = val;
    rear = (rear + 1) % 10;
    printf("[producer %d] insert into queue: %d\n" , index , val);
    pthread_cond_signal(&this -> empty_cond);
    pthread_mutex_unlock(&this -> mutex);
}

int CycleQueue::pop(int index) 
{
    int res;
    pthread_mutex_lock(&mutex);
    while(this -> empty()) {
        pthread_cond_wait(&this -> empty_cond , &this -> mutex);
    }
    res = this -> queue[front];
    front = (front + 1) % 10;
    printf("[consumer %d] get from queue: %d\n" , index , res);
    pthread_cond_signal(&this -> full_cond);
    pthread_mutex_unlock(&mutex);
    return res;
}

bool CycleQueue::empty()
{
    return rear == front;
}


bool CycleQueue::full() 
{
    return (rear + 1) % 10 == front;
}
