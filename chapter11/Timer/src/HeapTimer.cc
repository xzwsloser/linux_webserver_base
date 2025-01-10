#include "HeapTimer.h"

time_heap::time_heap(int cap): capacity(cap) , cur_size(0) 
{
    array = new heap_timer*[cap];
    for(int i = 0 ; i < cap ; i ++) {
        array[i] = nullptr;
    }
}

time_heap::time_heap(heap_timer** init_array , int cap , int size): capacity(cap) , cur_size(size)
{
    array = new heap_timer* [cap];
    for(int i = 0 ; i < cap ; i ++) {
        array[i] = nullptr;
    } 

    for(int i = 0 ; i < size ; i ++) {
        array[i] = init_array[i];
    }

    for(int i = (cur_size - 1) / 2 ; i >= 0 ; i ++) {
        percolate_down(i);
    }
}


time_heap::~time_heap() 
{
    for(int i = 0 ; i < cur_size ; i ++) {
        delete array[i];
    }

    delete [] array;
}

void time_heap::add_timer(heap_timer* timer)
{
    if(timer == nullptr) return ;
    cur_size ++;
    if(cur_size >= capacity) {
        resize();
    }

    int hole = cur_size - 1;
    int parent = 0;
    for( ; hole > 0 ; hole = parent) {
        parent = (hole - 1) / 2;
        if(timer -> expire >= array[parent] -> expire) {
            break;
        }

        array[hole] = array[parent];
    }

    array[hole] = timer;
}

void time_heap::del_timer(heap_timer* timer)
{
    if(timer == nullptr) return ;
    timer -> cb_func = nullptr;
}

heap_timer* time_heap::top() const
{
    if(empty()) return nullptr;
    return array[0] ;
}

void time_heap::pop_timer()
{
    if(empty()) return ;
    if(array[0] != nullptr) {
        delete array[0];
        cur_size --;
        array[0] = array[cur_size]; /* 相当于逻辑删除,只是根据长度删除 */
        percolate_down(0);  
    } 
}

void time_heap::tick()
{
    heap_timer* tmp = array[0];
    time_t cur = time(nullptr);
    while(!empty()) {
        if(tmp == nullptr) break;
        if(tmp -> expire > cur) {
            break;
        } 

        if(array[0] -> cb_func) {
            array[0] -> cb_func(array[0] -> user);
        }
        
        pop_timer();
        tmp = array[0];
    }
}

void time_heap::percolate_down(int hole)
{
    if(hole >= cur_size) return ;
    heap_timer* tmp = array[hole];
    int child = 0;
    for( ; (hole * 2 + 1) <= (cur_size - 1) ; hole = child) {
        child = hole * 2 + 1;
        if(child < (cur_size - 1) && (array[child + 1] -> expire < array[child] -> expire)) {
            child ++;
        }

        if(tmp -> expire > array[child] -> expire) {
            array[hole] = array[child];
        } else {
            break;
        }
    }
}

void time_heap::resize() 
{
    heap_timer** temp = new heap_timer* [2 * capacity];
    for(int i = 0 ; i < 2 * capacity ; i ++) {
        temp[i] = nullptr;
    }

    capacity *= 2;
    for(int i = 0 ; i < cur_size ; i ++) {
        temp[i] = array[i];
   }
   delete [] array;
   array = temp;
}

bool time_heap::empty() const 
{
    return cur_size == 0;
}