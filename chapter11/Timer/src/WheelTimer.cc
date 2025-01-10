#include "WheelTimer.h"

time_wheel::time_wheel(): cur_slot(0)
{
    for(int i = 0 ; i < N ; i ++) {
        slots[i] = nullptr;
    }
}

time_wheel::~time_wheel()
{
    for(int i = 0 ; i < N ; i ++) {
        tw_timer* tmp = slots[i];
        while(tmp != nullptr) {
            slots[i] = tmp -> next;
            while(slots[i] != nullptr) {
                slots[i] -> prev = nullptr;
            } 
            delete tmp;
            tmp = slots[i];
        }    
    }
}

tw_timer* time_wheel::add_timer(int timeout) 
{
    int tick = timeout < SI ? 1 : timeout / SI ;
    int rotation = tick / N ;  /* 新构造定时器的圈数 */
    int ts = (cur_slot + (tick % N)) % N ; /* 新的定时器所在的槽位 */
    tw_timer* timer = new tw_timer(rotation , ts);

    if(slots[ts] == nullptr) {
        slots[ts] = timer;
    } 

    else {
        timer -> next = slots[ts]; 
        slots[ts] -> prev = timer;
        slots[ts] = timer;
    }
    return timer;
}

void time_wheel::del_timer(tw_timer* timer)
{
    if(timer == nullptr) return ;
    int ts = timer -> time_slot ;
    if(timer == slots[ts]) {
        slots[ts] = slots[ts] -> next;
        if(slots[ts] != nullptr) {
            slots[ts] -> prev = nullptr;
        }
        delete timer;
    } 

    else {
        tw_timer* tmp = timer -> next;
        timer -> prev -> next = tmp;
        if(tmp != nullptr) {
            tmp -> prev = timer -> prev;
        } 
        delete timer;
    }
}

void time_wheel::tick() 
{
    tw_timer* tmp = slots[cur_slot];
    while(tmp) {
        if(tmp -> rotation > 0) {
            tmp -> rotation --;
            tmp = tmp -> next;
        }

        else {
            tmp -> cb_func(tmp -> user);
            if(tmp == slots[cur_slot]) {
                slots[cur_slot] = slots[cur_slot] -> next;
                if(slots[cur_slot] != nullptr) {
                    slots[cur_slot] -> prev = nullptr;
                }
                delete tmp;
                tmp = slots[cur_slot];
            } else {
                tmp -> prev -> next = tmp -> next;
                if(tmp -> next != nullptr) {
                    tmp -> next -> prev = tmp -> prev;
                }
                tw_timer* tmp2 = tmp -> next;
                delete tmp;
                tmp = tmp2 ;
            }
        }
    }

    cur_slot = (cur_slot + 1) % N;
}

void time_wheel::adjust_timer(tw_timer* timer , int timeout)
{
    if(timer == nullptr) return ;
    int ts = timer -> time_slot;
    if(timer == slots[ts]) {
        slots[ts] = slots[ts] -> next;
        if(slots[ts] != nullptr) {
            slots[ts] -> prev = nullptr;
        }
    }

    else {
        timer -> prev -> next = timer -> next;
        if(timer -> next != nullptr) {
            timer -> next -> prev = timer -> prev;
        }
    }

    int ticks =  (timeout < SI) ? 1 : (timeout / SI);
    int rotation = ticks / N ;
    int new_ts = (cur_slot + (ticks % N)) % N ; 
    timer -> time_slot = (new_ts + timer -> time_slot) % N;
    timer -> rotation = (rotation + timer -> rotation);
    ts = timer -> time_slot;
    if(slots[ts] == nullptr) {
        slots[ts] = timer;
        slots[ts] -> prev = nullptr;
        slots[ts] -> next = nullptr;
        return ;
    }

    else {
        timer -> next = slots[ts];
        slots[ts] -> prev = timer;
        slots[ts] = timer;  
        return ;
    }
}