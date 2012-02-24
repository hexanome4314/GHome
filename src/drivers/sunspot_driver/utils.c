#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "utils.h"

void prepare_sock(int port, int addr, int * sock, struct sockaddr_in * saddr)
{
    
    *sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&sock, 0, sizeof(sock));
    saddr->sin_addr.s_addr = addr;
    saddr->sin_family = AF_INET;
    saddr->sin_port = htons(port);
}

int mk_sock(int port, int addr, int flags)
{
  int sock;
  struct sockaddr_in saddr;

  prepare_sock(port, addr, &sock, &saddr);

  bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
  perror("bind");

  listen(sock, 10);
  perror("listen");

  return sock;
}

int connect_to(int addr, int c_port, int proto)
{
  int sock;
  struct sockaddr_in saddr;
  prepare_sock(c_port, addr, &sock, &saddr);
  printf("Connecting to %s on port %d... ", inet_ntoa(saddr.sin_addr), c_port);
  if(connect(sock, (struct sockaddr*)&saddr, sizeof(struct sockaddr))==0)
	 printf("OK.\n", inet_ntoa(saddr.sin_addr), c_port);
  else{
	 printf("Failed !\nConnexion fail : %s\n", strerror(errno));
	 return -1;
  }
  return sock;
}
