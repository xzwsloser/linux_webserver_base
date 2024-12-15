#ifndef _NET_WORH_H_
#define _NET_WORK_H_
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<sys/poll.h>
#include<sys/epoll.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<signal.h>
#include <bits/sigaction.h>
#include <asm-generic/signal-defs.h>

void error_handler(const char* message);
int Socket(int domain , int type , int protocol);
void Bind(int sockfd , const struct sockaddr_in* addr , socklen_t addr_len);
void Listen(int sockfd , int backlog);
int Accept(int sockfd , struct sockaddr_in* addr , socklen_t* addr_len);
void Connect(int sockfd , const struct sockaddr_in* addr , socklen_t addr_len);
int Select(int nfds , fd_set* read_set , fd_set* write_set , fd_set* exception_set , struct timeval* timeout);
int Poll(struct pollfd* fds , nfds_t nfds , int timeout);
int Epoll_create( int size );
void Epoll_ctl(int edfd , int op , int fd , struct epoll_event* event);
int Epoll_wait(int edfd , struct epoll_event* events , int maxevents , int timeout);
int setnoblocking(int fd);
void Pipe(int pipefd[2]);
void Sigaction(int signum , const struct sigaction* act , struct sigaction* oldact);
void Sigprocmask(int how , const sigset_t* set , sigset_t* oldset);
void Addfd(int epollfd , int fd); /* 使用ET 模式添加文件描述符到 epoll监听事件数中 */

#endif