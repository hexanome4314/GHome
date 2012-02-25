#ifndef CORE_H_
#define CORE_H_

#define MAX_NUMBER_OF_DRIVERS 2
#define MAX_NUMBER_OF_SENSORS 32

typedef struct{
	int fd; // File descriptor assigné par l'ios
	int id; // Id du capteur
	unsigned char* name; // Nom donné au capteur
	int used_fields; // flag indiquant les champs utilisés par le capteur
} infos_sensor;

#endif
