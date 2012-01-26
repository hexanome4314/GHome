/*
 * ecoute.h
 *
 *  Created on: 13 janv. 2012
 *      Author: Raphael CLAUDE
 */

#ifndef LISTEN_H_
#define LISTEN_H_

/************************************************** TYPES */

/**
 * \brief The default structure for EnOcean packets
 *
 * Data structure for RPS, 1BS and 4BS packages.
 *
 **/

#include <stdint.h>
#define BYTE uint8_t
typedef struct enocean_data_structure {
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
} enocean_data_structure;


/**
 * The default linked lists structure for EnOcean received messages
 *
 **/
typedef struct _enocean_data_structure_queue _enocean_data_structure_queue;
struct _enocean_data_structure_queue{
	enocean_data_structure* enocean_data_structure;
	struct _enocean_data_structure_queue* previous;
};
typedef _enocean_data_structure_queue enocean_data_structure_queue;

/**
 * linked lists structure for sensors id.
 */
typedef struct _sensors_queue _sensors_queue;
struct _sensors_queue{
	char sensor[9];
	struct _sensors_queue* next;
};
typedef _sensors_queue sensors_queue;
/************************************************ GLOBALS */

/*---------------- sensors */

#define SENSORS_FILE "sensors_list"
typedef char id_sensor[9];

/*------------------ logs */

#define DEBUG_MODE 1
#define LOG 1

/*--------------- network */

#define IP_BORNE_ENOCEAN "127.0.0.1"
#define PORT_BORNE_ENOCEAN 1338
/*
#define IP_BORNE_ENOCEAN "134.214.105.28"
#define PORT_BORNE_ENOCEAN 5000
*/


/********************************************** FUNCTIONS */

void parser(char* uneTrame, enocean_data_structure* unMessage);
void interpretAndSend();


#endif /* LISTEN_H_ */
