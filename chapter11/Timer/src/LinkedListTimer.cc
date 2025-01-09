#include "LinkedListTimer.h"

sort_timer_list::~sort_timer_list() 
{
    util_timer* tmp = head;
    while(tmp != nullptr) {
       head = tmp -> next;
       delete tmp;
       tmp = head; 
    }
}

void sort_timer_list::add_timer(util_timer* timer , util_timer* front)
{
    util_timer* prev = front;
    util_timer* temp = prev -> next;
    while(temp != nullptr) {
        if(temp -> expire > timer -> expire) {
            break;
        }
        prev = temp;
        temp = temp -> next;
    }

    timer -> next = temp;
    temp -> prev = timer;
    timer -> prev = prev ;
    prev -> next = timer; 
}

void sort_timer_list::add_timer(util_timer* timer) {
    if(head == nullptr && tail == nullptr) {
        head = timer;
        tail = head;
        return ;
    }

    if(timer -> expire < head -> expire) {
        timer -> next = head ;
        head -> prev = timer;
        head = timer;
        return ;
    }

    if(timer -> expire > tail -> expire) {
        tail -> next = timer;
        timer -> prev = tail;
        timer -> next = nullptr;
        tail = timer;
        return ;
    }

    add_timer(timer , head);
}

void sort_timer_list::adjust_timer(util_timer* timer) 
{
    if(timer == nullptr) return ;
    util_timer* tmp = timer -> next;
    if(tmp == nullptr || timer -> expire < tmp -> expire) return ;
    if(timer == head) {
        head = timer -> next;
        timer -> next = nullptr;
        timer -> prev = nullptr;
        add_timer(timer , head);
    } else {
        timer -> prev -> next = tmp;
        tmp -> prev = timer -> prev;
        add_timer(timer , tmp);
    }
}

void sort_timer_list::del_timer(util_timer* timer)
{
    if(timer == nullptr) return ;
    if(timer == head && timer == tail) {
        delete timer;
        head = nullptr;
        tail = nullptr;
        return ;
    }

    if(timer == head) {
        head = timer -> next;
        head -> prev = nullptr;
        delete timer;
        return ;
    }

    if(timer == tail) {
        tail = tail -> prev;
        tail -> next = nullptr;
        delete timer;
        return ;
    }

    timer -> prev -> next = timer -> next;
    timer -> next -> prev = timer -> prev;
    delete timer;
}

void sort_timer_list::tick() 
{
    if(head == nullptr) return ;
    time_t cur = time(nullptr);
    util_timer* tmp = head;
    while(tmp) {
        if(tmp -> expire > cur) { // 注意这里的 tmp 是一个绝对时间
            break;
        }

        tmp -> callback(tmp -> client); // 执行回调函数
        head = tmp -> next;
        if(head) {
            head -> prev = nullptr;
        }
        delete tmp;
        tmp = head;
    }
}