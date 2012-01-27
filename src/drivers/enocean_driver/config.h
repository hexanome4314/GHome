#ifndef CONFIG_H_
#define CONFIG_H_

/****************************************** sensors */

#define SENSORS_FILE "sensors_list"
typedef char id_capteur[9];

/********************************************* logs */

#define DEBUG_MODE 1
#define LOG 1

/****************************************** network */

#define IP_BORNE_ENOCEAN "127.0.0.1"
#define PORT_BORNE_ENOCEAN 1338
/*
#define IP_BORNE_ENOCEAN "134.214.105.28"
#define PORT_BORNE_ENOCEAN 5000
*/


#endif /* CONFIG_H_ */
