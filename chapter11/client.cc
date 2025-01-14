/* 客户端 */
#include "../utils/network.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    error_handler("<usage> too few arguement to client!");
  }

  int port = atoi(argv[1]);
  struct sockaddr_in serv_addr;
  int connfd;
  int ret;
  char buf[1024];
  connfd = Socket(AF_INET, SOCK_STREAM, 0);

  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_family = AF_INET;
  Connect(connfd, &serv_addr, sizeof(serv_addr));

  while (1) {
    scanf("%s", buf);
    send(connfd, buf, strlen(buf), 0);
  }
}
