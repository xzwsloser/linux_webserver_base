/* 处理非活动连接 */
#include "../utils/network.h"
#include "Timer/inc/WheelTimer.h"

#define MAX_USER_NUMBER 1024
#define MAX_EVENT_NUMBER 1024 /* 最大可以监听的数量 */
#define TIMESHOT 5            /* 过期时间 */
#define TIMECP 1

static int pipefd[2];    /* 信号处理的管道 */
static time_wheel wheel; /* 升序链表 */
static int epollfd = 0;  /* epoll 文件描述符 */

void sig_handler(int sig);       /* 信号处理函数 */
void timer_handler();            /* 定时任务处理函数,用于清除超时连接 */
void cb_func(client_data *user); /* 定时器回调函数用于关闭连接 */
void AddSigi(int sig);
void sig_handler(int sig) {
  int saverrno = errno;
  int msg = sig;
  write(pipefd[1], (char *)&msg, 1);
  errno = saverrno;
}

void timer_handler() {
  wheel.tick();
  alarm(TIMECP); /* 继续产生周期性的时钟信号 */
}

void cb_func(client_data *user) {
  Epoll_ctl(epollfd, EPOLL_CTL_DEL, user->sockfd, nullptr);
  close(user->sockfd);
  printf("close fd: %d\n", user->sockfd);
}

void AddSigi(int sig) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sig_handler;
  sa.sa_flags |= SA_RESTART; // 重启中断系统调用
  sigfillset(&sa.sa_mask);
  Sigaction(sig, &sa, nullptr);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    error_handler("<usage>: please input a port!");
  }

  int port = atoi(argv[1]);
  int listenfd, connfd, sockfd;
  struct sockaddr_in serv_addr, clnt_addr;
  socklen_t clnt_addr_len = sizeof(clnt_addr);
  epoll_event events[MAX_EVENT_NUMBER];
  bool server_stop = false; /* SIGTERM 信号控制服务器的开始和结束 */
  client_data *users = new client_data[MAX_USER_NUMBER];
  int number, ret; /* Epoll_wait 和 read 函数的返回值 */
  char signals[1024];
  bool timeout = false; /* 是否处理超时时间 */
  int optval = 1;

  listenfd = Socket(AF_INET, SOCK_STREAM, 0);

  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_family = AF_INET;
  Bind(listenfd, &serv_addr, sizeof(serv_addr));

  Listen(listenfd, MAX_USER_NUMBER);

  wheel = time_wheel();
  Pipe(pipefd);
  epollfd = Epoll_create(MAX_EVENT_NUMBER);
  Addfd(epollfd, listenfd);
  setnoblocking(pipefd[1]);
  Addfd(epollfd, pipefd[0]);
  /* 设置信号处理函数 */
  AddSigi(SIGALRM);
  AddSigi(SIGTERM);

  setsockopt(listenfd, SOCK_STREAM, SO_REUSEADDR, &optval, sizeof(optval));

  alarm(TIMECP);
  while (!server_stop) {
    number = Epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
    if ((number == -1) && (errno != EINTR)) {
      printf("epoll wait failed!\n");
      break;
    }

    for (int i = 0; i < number; i++) {
      sockfd = events[i].data.fd;
      /* 处理连接的用户 */
      if (sockfd == listenfd) {
        connfd = Accept(listenfd, &clnt_addr, &clnt_addr_len);
        Addfd(epollfd, connfd);
        users[connfd].address = clnt_addr;
        users[connfd].sockfd = connfd;
        tw_timer *timer = wheel.add_timer(TIMESHOT);
        timer->cb_func = cb_func;
        timer->user = &users[connfd];
        users[connfd].timer = timer;
      }

      /* 处理信号 */
      else if (sockfd == pipefd[0]) {
        memset(signals, 0, sizeof(signals));
        ret = read(pipefd[0], signals, sizeof(signals));
        if (ret == -1) {
          printf("recv from pipefd[0] failed");
          continue;
        } else if (ret == 0) {
          continue;
        } else {
          for (int i = 0; i < ret; i++) {
            switch (signals[i]) {
            case SIGALRM: {
              timeout = true;
              break;
            }
            case SIGTERM: {
              server_stop = true;
              break;
            }
            }
          }
        }

      }
      /* 处理客户端连接 */
      else if (events[i].events & EPOLLIN) {
        memset(users[sockfd].buf, 0, sizeof(users[sockfd].buf));
        ret = recv(sockfd, users[sockfd].buf, sizeof(users[sockfd].buf), 0);
        printf("get %d bytes client msg: %s from %d \n", ret, users[sockfd].buf,
               sockfd);
        tw_timer *timer = users[sockfd].timer;
        if (ret == -1) {
          if (errno != EAGAIN) {
            cb_func(&users[sockfd]);
            if (timer) {
              wheel.del_timer(timer);
            }
          }
        } else if (ret == 0) {
          cb_func(&users[connfd]);
          if (timer) {
            wheel.del_timer(timer);
          }
        } else {
          /* 正常维持连接所以可以增加连接时间 */
          if (timer) {
            // timer -> rotation ++;
            wheel.adjust_timer(timer, TIMESHOT);
          }
        }
      }
    }
    /* 处理定时时间 */
    if (timeout) {
      printf("...tick...\n");
      timer_handler();
      timeout = false;
    }
  }

  close(listenfd);
  close(pipefd[0]);
  close(pipefd[1]);
  delete[] users;
  return 0;
}
