#include "network.h"


void error_handler(const char* message) {
    fputs(message , stderr);
    fputs("\n" , stderr); 
    exit(1);
}

int Socket(int domain , int type , int protocol) {
    int sockfd ;
    if((sockfd = socket(domain , type , protocol)) < 0) {
        error_handler("Socket() failed!");
    }
    return sockfd ;
}

void Bind(int sockfd , const struct sockaddr_in* addr , socklen_t addr_len ) {
    if(bind(sockfd , (struct sockaddr*)addr , addr_len) < 0) {
        error_handler("Bind() failed!");
    }
}

void Listen(int sockfd , int backlog) {
    if(listen(sockfd , backlog) < 0) {
        error_handler("Listen() failed!");
    }
}

int Accept(int sockfd , struct sockaddr_in* addr , socklen_t* addr_len) {
    int connfd ;
    if((connfd = accept(sockfd , (struct sockaddr*)addr , addr_len)) < 0) {
        error_handler("Accept() failed!");
    }
    return connfd;
}

void Connect(int sockfd , const struct sockaddr_in* addr , socklen_t addr_len) {
    if(connect(sockfd , (struct sockaddr*)addr , addr_len) < 0) {
        error_handler("Connect() failed!");
    }
}

int Select(int nfds , fd_set* read_set , fd_set* write_set , fd_set* exception_set , struct timeval* timeout) {
    int fds;
    if((fds = select(nfds , read_set , write_set , exception_set , timeout)) < 0) {
        error_handler("Select() failed!");
    }
    return fds;
}

int Poll(struct pollfd* fds , nfds_t nfds , int timeout) {
    int ret;
    if((ret = poll(fds , nfds , timeout)) < 0) {
        error_handler("Poll() failed!");
    }
    return ret;
}

int Epoll_create( int size ) {
   int enfd ;
   if((enfd = epoll_create(size)) < 0) {
        error_handler("Epoll_create() failed!");
   }  
   return enfd;
}

void Epoll_ctl(int enfd , int op , int fd , struct epoll_event* event) {
    if(epoll_ctl(enfd , op , fd , event) < 0) {
        error_handler("Epoll_ctl() failed!");
    }
}

int Epoll_wait(int enfd , struct epoll_event* events , int maxevents , int timeout) {
    int ret;
    if((ret = epoll_wait(enfd , events , maxevents , timeout)) < 0 && errno != EINTR) {
        error_handler("Epoll_wait failed()!");
    }
    return ret;
}

int setnoblocking(int fd) {
    int old_option = fcntl(fd , F_GETFL);
    int new_option = old_option | O_NONBLOCK ;
    fcntl(fd , F_SETFL , new_option);
    return old_option;   
}

void Pipe(int pipefd[2]) {
    if((pipe(pipefd)) < 0) {
        error_handler("Pipe() failed!");
    }
}

void Sigaction(int signum , const struct sigaction* act , struct sigaction* oldact) {
    if(sigaction(signum , act , oldact) < 0) {
        error_handler("Sigaction() failed!");
    }
}

void Sigprocmask(int how , const sigset_t* set , sigset_t* oldset) {
    if(sigprocmask(how , set , oldset) < 0) {
        error_handler("Sigprocmask() failed!");
    }
}


void Addfd(int epollfd , int fd) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET ;
    Epoll_ctl(epollfd , EPOLL_CTL_ADD , fd , &event);
    setnoblocking(fd);
}