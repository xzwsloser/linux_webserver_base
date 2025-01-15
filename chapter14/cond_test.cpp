#include "CycleQueue.h"
#include <unistd.h>
void* producer(void* arg);
void* consumer(void* arg);

CycleQueue queue; /* 需要操作的全局变量 */

int main() {
    pthread_t pro[6];
    pthread_t con[5];
    for(int i = 0 ; i < 6 ; i ++) {
        int* arg = new int(i);
        pthread_create(&pro[i], nullptr , producer , (void*)arg);
    }

    for(int i = 0 ; i < 5 ; i ++) {
        int* arg = new int(i);
        pthread_create(&con[i], nullptr , consumer , (void*)arg);
    }

    for(int i = 0 ; i < 6 ; i ++) {
        pthread_join(pro[i] , nullptr);
    }

    for(int i = 0 ; i < 5 ; i ++) {
        pthread_join(con[i] , nullptr);
    }
    return 0;
}

void* producer(void* arg)
{
    int index = *((int*)arg);
    for(int i = index * 100 ; i < index * 100 + 10 ; i ++) {
        queue.insert(i , index);
        sleep(1);
    } 
    return nullptr;
}

void* consumer(void* arg)
{
    int index = *((int*)arg);
    for(int i = 0 ; i < 10 ; i ++) {
        queue.pop(index);
        sleep(2);
    } 
    return nullptr;
}
