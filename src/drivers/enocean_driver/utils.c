#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "utils.h"
#include "config.h"

void prepare_sock(int port, int addr, int * sock, struct sockaddr_in * saddr)
{
  *sock = socket(AF_INET, SOCK_STREAM, 0);
  perror("socket");
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

  if(connect(sock, (struct sockaddr*)&saddr, sizeof(struct sockaddr))==0)
	 printf("Connexion to %s on port %d\n", inet_ntoa(saddr.sin_addr), htons(saddr.sin_port));
  else{
	 printf("Connexion fail : %s\n", strerror(errno));
	 return -1;
  }
  return sock;
}

/*
id_capteur* read_sensors_list_file(int* a_number_of_sensor){

	FILE* sensors_file;
	sensors_file = fopen(SENSORS_FILE,"rt");
	fseek(sensors_file,0L,SEEK_END);
	int size = ftell(sensors_file);
	fseek(sensors_file,0L,SEEK_SET);
	*(a_number_of_sensor) = (size/10)%1;

	id_capteur* sensors = malloc(int[*(a_number_of_sensor)]);
	int sensor_num = 0;
	while(fgets(sensors[sensor_num],9,sensors_file)!=NULL
		  || sensor_num != *(a_number_of_sensor) )
	{
		sensor_num++;
	}
	return sensors;
}*/
