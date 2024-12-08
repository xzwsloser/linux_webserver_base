/* 设置守护进程的方法 */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
int main() {
    int pid;  /* 子进程 pid */
    if((pid = fork()) == 0) {
        printf("I am child process , PID: %d , PGID: %d\n" , getpid() , getpgid(getpid()));
    } else {
        exit(0);
    }

    // 1. 首先设置掩码
    umask(0);
    // 2. 创建会话
    int sid = setsid();
    if(sid < 0) {
        exit(1);
    }
    // 3. 切换工作目录
    if((chdir("/")) < 0) {
        exit(1);
    }
    // 4. 关闭文件描述符
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    // 5. 将标准输入,标准输出和标准错误输出都重定向到 /dev/null 中
    open("/dev/null" , O_RDONLY);
    open("/dev/null" , O_RDWR);
    open("/dev/null" , O_RDWR);
    return 0;
}