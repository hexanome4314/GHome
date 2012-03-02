/*********************************************************************************************************
*
* 	 _/_/_/ _/ _/_/_/ _/_/_/ _/ _/
* 	_/ _/ _/ _/ _/ _/ _/ _/ _/_/ _/_/_/ _/_/ _/_/
*      _/_/_/ _/ _/ _/ _/ _/_/ _/_/_/_/ _/ _/ _/ _/ _/ _/_/_/_/
*     _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/
*    _/ _/_/_/_/ _/_/_/ _/_/_/ _/ _/ _/_/ _/ _/ _/ _/_/_/
*
*
*  __   __   ___   _  _______   __   ___   _
* |  | |  | |   | | ||       | |  | |   | | |
* |  |_|  | |   |_| ||___    | |  | |   |_| |
* |       | |       | ___|   | |  | |       |
* |       | |    ___||___    | |  | |    ___|
* |   _   | |   |     ___|   | |  | |   |
* |__| |__| |___|    |_______| |__| |___|
*
* API implementee par les drivers et utilisee par le gestionnaire de drivers
**********************************************************************************************************/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "drv_api.h"
#include "listen.h"
#include "utils.h"

#define NB_DEV_MAX 256
#define NB_DATA_BYTES 4

#define OK 0
#define ERROR -1


/* ---------- Types persos ---------- */
typedef struct
{
unsigned char* dataBytes; /* Donnees du capteur */
unsigned int idDev; /* id physique du capteur */
} devData;

/* ---------- Variable du pilote ---------- */

sensors_queue* sensors; /* liste chainée des capteurs */
int nbDev; /* Nombre de capteurs installes */
int sock;
int message_box;
pthread_t filter_thread = 0;
pthread_t interprets_and_sends_thread = 0;

/* ---------- Methodes public du pilote ---------- */

/**
 * Met en place le context pour le lancement des deux threads du driver
 * \param remote_addr ip de la base
 * \param remote_port port de la base
 */
int drv_init( const char* remote_addr, int remote_port )
{
	/* init the communication with the sensors base */
	sock = connect_to(inet_addr(remote_addr), remote_port, 0);
	if(sock == -1){
		perror("listen - Start driver fail due to socket connection");
		return 1;
	}else{ /* politeness */
		send(sock,"Hi from Hx4314's driver!",25,0);
	}
	sensors = malloc(sizeof(sensors_queue));
	sensors->next = NULL;
	/* Initialization of semaphores used to synchronize the two threads */
	initialisation_for_listener();
	return 0;
}


/**
Fonction appelée par le gestionnaire de drivers pour activer l'écoute (après l'initialisation)
\return 0 si tout est ok, > 0 si erreur
*/
int drv_run(int msgq_id)
{
	message_box = msgq_id;
	/* start first thread */
	void (*p_function_to_receive_and_filter);
	p_function_to_receive_and_filter = &listenAndFilter;
	listen_and_filter_params* params = (listen_and_filter_params*) malloc(sizeof(listen_and_filter_params));
	params->sock= sock;
	params->sensors = sensors;
	pthread_create(&filter_thread, NULL, p_function_to_receive_and_filter, params);

	/* start second thread */
	int* msgid = (int*)malloc(sizeof(int));
	*(msgid) = msgq_id;
    void (*p_function_to_interpret_and_send);
	p_function_to_interpret_and_send = &interpretAndSend;
	pthread_create(&interprets_and_sends_thread, NULL, p_function_to_interpret_and_send, &message_box);

	return 0;
}

/**
Fonction appelée par le gestionnaire de drivers juste avant de décharger la librairie de la mémoire. L'écoute se stoppe et les ressources sont libérées
*/
void drv_stop( void )
{
    printf("dying...");
	/* stop the two thread */
    if(filter_thread != 0)
    {
        pthread_cancel(filter_thread);
    }
    if(interprets_and_sends_thread != 0)
    {
        pthread_cancel(interprets_and_sends_thread);
    }
    
	/* free the allocated memory */
	sensors_queue* last_sensors = sensors;
	while(last_sensors != NULL){
		last_sensors = sensors->next;
		free(sensors);
		sensors = last_sensors;
	}
}

/**
 * \param id_sensor l'id du capteur qui doit correspondre avec l'id réel
*/
int drv_add_sensor( unsigned int id_sensor){
	sensors_queue* new_sensor = malloc(sizeof(sensors_queue));
	sprintf(new_sensor->sensor, "%.8X", id_sensor);
	new_sensor->next = sensors->next;
	sensors->next = new_sensor;
	if(DEBUG_MODE)
		printf("drv_api - DEBUG drv_add_sensor %s\n",new_sensor->sensor);
	return 0;
}

/**
Fonction appelée par le gestionnaire de drivers pour supprimer un capteur en cours d'écoute.
\param id_sensor Identifiant unique du capteur qui ne doit plus être écouté
*/
void drv_remove_sensor( unsigned int id_sensor ){

	sensors_queue* current_sensor = sensors;
	char* char_id_sensor = "00000000";
	sprintf(char_id_sensor, "%u",id_sensor);
	/* regular case */
	while(current_sensor != NULL && current_sensor->next != NULL){
		if(current_sensor->next->sensor == char_id_sensor){
			sensors_queue* to_free = current_sensor->next;
			current_sensor->next = current_sensor->next->next;
			free(to_free);
		}
	}
	return;
}

/**
Permet de demander des informations à un capteur
\param id_sensor Identifiant unique du capteur à interroger
id_trame Identifiant de la trame à envoyer
buffer Buffer qui va recevoir les données de retour
max_length Taille maximale du buffer
\return 0 si erreur, ou la taille des données lues
*/
int drv_fetch_data( unsigned int id_sensor, unsigned int id_trame, char* buffer, int max_length )
{
    return 0;
}


/**
Permet d'envoyer des données à un capteur (sans retour de sa part)
\param id_sensor Identifiant unique du capteur à contacter
id_trame Identifiant de la trame à envoyer
\return 0 si tout est ok, > 0 si erreur
*/
int drv_send_data( unsigned int id_sensor, char commande )
{
	return 0;
}

/**
Retourne les informations concernant le driver (nom, version, ...)
\param  buffer		Chaîne de caractère qui reçoit les données
	max_length	Taille du buffer max
*/
void drv_get_info( char* buffer, int max_length )
{
    strcpy( buffer, "Driver SunSpot, v15.0" );
}
