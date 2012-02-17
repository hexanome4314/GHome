#ifndef DEMON_H_
#define DEMON_H_

#define MAX_NUMBER_OF_DRIVERS 2
#define MAX_NUMBER_OF_SENSORS 32

typedef struct{
int fd;
int id;
unsigned char* name;
} infos_sensor;

#endif
