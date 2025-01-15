/* sem信号量来控制进程同步 */
#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/ipc.h>
#include<sys/sem.h>

// 用于 sem_ctl 配置信号量的相关信息
union semun {
    int val;  /* 用于 SETVAL 选项,设置信号量的值 */   
    struct semid_ds* buf; 
    unsigned short int* array;
    struct seminfo* __buf;
};

void pv(int sem_id , int op); /* 对于信号量进行 pv 操作 */

int main() {
    int sem_id = semget(IPC_PRIVATE , 1 , 0666);    
    union semun sem_un;
    sem_un.val = 1;
    semctl(sem_id , 0 , SETVAL , sem_un);

    pid_t id = fork();
    if(id < 0) {
        return 1;
    } else if (id == 0) {
        printf("child try to get the sem\n");
        pv(sem_id , -1);
        printf("child get the sem ...\n");
        sleep(3);
        pv(sem_id , 1);
        exit(0);
    } else {
        printf("parent try to get the sem\n");
        pv(sem_id , -1);
        printf("parent get the sem\n");
        sleep(3);
        pv(sem_id , 1);
    }

    waitpid(-1 , NULL , 0);
    semctl(sem_id , 0 , IPC_RMID , sem_un); // 删除信号
    return 0;
}

void pv(int sem_id , int op)
{
    struct sembuf sem_b;
    sem_b.sem_num = 0; /* 信号量的编号 */
    sem_b.sem_op = op; /* 增减值 */
    sem_b.sem_flg = SEM_UNDO; /* 设置 semadj 标记 */
    semop(sem_id , &sem_b , 1);
}

