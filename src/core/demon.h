#ifndef DEMON_H_
#define DEMON_H_

#define MAX_NUMBER_OF_DRIVERS 2
#define MAX_NUMBER_OF_SENSORS 32

int get_fd(int id_capteur);

typedef struct{
int fd;
int id;
} infos_sensor;

#endif
