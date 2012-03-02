/*
 * ecoute.h
 *
 *  Created on: 13 janv. 2012
 *      Author: Raphael CLAUDE && Léo LEFEBVRE
 */

#ifndef LISTEN_H_
#define LISTEN_H_

/************************************************** TYPES */

/**
 * \brief The default structure for SunSpot packets
 *
 * Data structure for RPS, 1BS and 4BS packages.
 *
 **/

#include <stdint.h>

// Structure des messages SunSpot analysés par le driver
typedef struct sunspot_data_structure {
    int address; // Adresse du sunspot - 8 char
    int light; // luminosité - 10 char
    float temperature; // temperature - 12 char
    int voltage; // niveau de batterie - 3 char
} sunspot_data_structure;

// Taille en caractères de messages reçus
#define MESSAGE_LEN 36 // 8+10+12+3+3 (espaces)

/**
 * The default linked lists structure for SunSpot received messages
 * UNUSED
 **/
typedef struct _sunspot_data_structure_queue _sunspot_data_structure_queue;
struct _sunspot_data_structure_queue{
	sunspot_data_structure* sunspot_data_structure;
	struct _sunspot_data_structure_queue* previous;
};
typedef _sunspot_data_structure_queue sunspot_data_structure_queue;

/**
 * linked lists structure for sensors id.
 */
#define SENSORNAME_LEN 8
typedef struct _sensors_queue _sensors_queue;
struct _sensors_queue{
	char sensor[SENSORNAME_LEN];
	struct _sensors_queue* next;
};
typedef _sensors_queue sensors_queue;

typedef struct{
	int sock;
	sensors_queue* sensors;
} listen_and_filter_params;

/************************************************ GLOBALS */

/*---------------- sensors */

typedef char id_sensor[9];

/*------------------ logs */

#define DEBUG_MODE 0
#define LOG 0

/*--------------- network */

#define IP_BORNE_SUNSPOT "127.0.0.1"
#define PORT_BORNE_SUNSPOT 6666


/********************************************** FUNCTIONS */

void initialisation_for_listener();
int listenAndFilter(listen_and_filter_params* params);
void interpretAndSend(int* msgq_id);

/**
 * return the list of sensors id
 */
sensors_queue* read_sensors_list_file(int* a_number_of_sensor);

#endif /* LISTEN_H_ */
