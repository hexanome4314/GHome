#ifndef DEMON_H_
#define DEMON_H_

#define MAX_NUMBER_OF_DRIVERS 2
#define MAX_NUMBER_OF_SENSORS 32

int get_fd(int id_capteur);
int get_fd_by_name(char* name);
int init_demon();

typedef struct{
int fd;
int id;
unsigned char* name;
} infos_sensor;

#endif
