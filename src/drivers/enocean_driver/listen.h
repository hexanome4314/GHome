/*
 * ecoute.h
 *
 *  Created on: 13 janv. 2012
 *      Author: Raphael CLAUDE
 */

#ifndef LISTEN_H_
#define LISTEN_H_

#include <stdint.h>
#define BYTE uint8_t

/**
 * \brief The default structure for EnOcean packets
 *
 * Data structure for RPS, 1BS and 4BS packages.
 *
 **/
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

void parser(char* uneTrame, enocean_data_structure* unMessage);

#endif LISTEN_H_
