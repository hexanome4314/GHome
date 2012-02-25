/*
 * ecoute.h
 *
 *  Created on: 13 janv. 2012
 *      Author: Raphael CLAUDE
 */

#ifndef LISTEN_H_
#define LISTEN_H_

#define SIMULATION 1

/************************************************** TYPES */

/**
 * \brief The default structure for SunSPOT packets
 *
 * Data structure for RPS, 1BS and 4BS packages.
 *
 **/

#include <stdint.h>
#define BYTE uint8_t
typedef struct sunspot_data_structure {
  BYTE SYNC_BYTE1; ///< Synchronisation Byte 1
  BYTE SYNC_BYTE2; ///< Synchronisation Byte 2
  BYTE H_SEQ_LENGTH; ///< Header identification and number of octets following the header octet
  BYTE ORG; ///< Type of telegram
  BYTE DATA_BYTE3; ///< Data Byte 3
  BYTE DATA_BYTE2; ///< Data Byte 2
  BYTE DATA_BYTE1; ///< Data Byte 1
  BYTE DATA_BYTE0; ///< Data Byte 0
  BYTE ID_BYTE3; ///< Transmitter ID Byte 3
  BYTE ID_BYTE2; ///< Transmitter ID Byte 2
  BYTE ID_BYTE1; ///< Transmitter ID Byte 1
  BYTE ID_BYTE0; ///< Transmitter ID Byte 0
  BYTE STATUS; ///< Status field
  BYTE CHECKSUM; ///< Checksum of the packet
} sunspot_data_structure;


/**
 * The default linked lists structure for SunSPOT received messages
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
typedef struct _sensors_queue _sensors_queue;
struct _sensors_queue{
	char sensor[9];
	struct _sensors_queue* next;
};
typedef _sensors_queue sensors_queue;

typedef struct{
	int sock;
	sensors_queue* sensors;
} listen_and_filter_params;

/************************************************ GLOBALS */

/*---------------- sensors */

#define SENSORS_FILE "../drivers/sunspot_driver/sensors_list"
typedef char id_sensor[9];

/*------------------ logs */

#define DEBUG_MODE 1
#define LOG 1

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