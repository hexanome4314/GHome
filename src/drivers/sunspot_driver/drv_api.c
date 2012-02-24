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

#include "../drv_api.h"
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
pthread_t filter_thread;
pthread_t interprets_and_sends_thread;

/* ---------- Methodes privees du pilote ---------- */

/**
Lit le fichier sensors_file pour y récuperer automatiquement la *
liste de nos capteurs
/param a_number_of_sensor un pointeur vers l'entier dans lequel ecrire le nombre de capteur lu
 */

sensors_queue* read_sensors_list_file(int* a_number_of_sensor){

	FILE* sensors_file;
	int loop_counter1;
	int loop_counter2;
	sensors_file = fopen(SENSORS_FILE,"r");
       
    if(sensors_file == NULL)
    {
        perror("Impossible d'ouvrir le fichier");
        return NULL;
    }
        
	fseek(sensors_file,0L,SEEK_END);
	int size = ftell(sensors_file);
	fseek(sensors_file,0L,SEEK_SET);
	*(a_number_of_sensor) = size/9;

	sensors_queue* sensors;
	sensors_queue* current_sensor;

	/* init for loop */
	current_sensor = (sensors_queue*)malloc(sizeof(sensors_queue));
	for(loop_counter2=0 ; loop_counter2 < 8 ; loop_counter2++){
				current_sensor->sensor[loop_counter2] = fgetc(sensors_file);
	}
	fgetc(sensors_file);
	current_sensor-> next = NULL;
	sensors = current_sensor;

	/* loop */
 	for(loop_counter1=1 ; loop_counter1 < *(a_number_of_sensor) ; loop_counter1++){
 		sensors_queue* new_sensor = (sensors_queue*)malloc(sizeof(sensors_queue));
 		for(loop_counter2=0 ; loop_counter2 < 8 ; loop_counter2++){
			new_sensor->sensor[loop_counter2] = fgetc(sensors_file);
		}
 		fgetc(sensors_file);
 		new_sensor->next = NULL;
 		current_sensor->next = new_sensor;
		current_sensor = new_sensor;
	}
        
        fclose(sensors_file);
	return sensors;
}


/* ---------- Methodes public du pilote ---------- */

int main(){
	// false application
	int msgq_id;
	msgq_id = msgget(IPC_PRIVATE, 0600 | IPC_CREAT | IPC_EXCL );

	// run the driver code
	printf("MAIN drv_init\n");
	drv_init(IP_BORNE_SUNSPOT, PORT_BORNE_SUNSPOT);
	sleep(5);
	printf("MAIN drv_run\n");
	drv_run(msgq_id);
	sleep(60);
	printf("MAIN drv_stop\n");
	drv_stop();

	// check for the result
	struct msg_drv_notify buffer;
	msgrcv( msgq_id, (void*) &buffer, sizeof(buffer) - sizeof(long), DRV_MSG_TYPE, 0 );
        printf( "#message  [%i,%i,%f]\t[%i]\n", buffer.id_sensor, buffer.flag_value, buffer.value, msgq_id );

	return 42;
}

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
            return -1;
    }else{ /* politeness */
            send(sock,"Hi from Hx4314's driver!",25,0);
    }

    /* get the sensors_id from "sensors_file"*/
    sensors = read_sensors_list_file(&nbDev);
    /* Initialization of semaphores used to synchronize the two threads */

    initialisation_for_listener();

    return 0;
}


/**
Fonction appelée par le gestionnaire de drivers pour activer l'écoute (après l'initialisation)
\return 0 si tout est ok, > 0 si erreur
*/
int drv_run(int msgq_id){

	message_box = msgq_id;
	/* start first thread */
	void (*p_function_to_receive_and_filter);
	p_function_to_receive_and_filter = &listenAndFilter;
	listen_and_filter_params* params = (listen_and_filter_params*) malloc(sizeof(listen_and_filter_params));
	params->sock= sock;
	params->sensors = sensors;
	pthread_create(&filter_thread,NULL,p_function_to_receive_and_filter,params);

	/* start second thread */
	int* msgid = (int*)malloc(sizeof(int));
	*(msgid) = msgq_id;
    void (*p_function_to_interpret_and_send);
	p_function_to_interpret_and_send = &interpretAndSend;
	pthread_create(&interprets_and_sends_thread,NULL,p_function_to_interpret_and_send,&message_box);

	return 0;
}

/**
Fonction appelée par le gestionnaire de drivers juste avant de décharger la librairie de la mémoire. L'écoute se stoppe et les ressources sont libérées
*/
void drv_stop( void ){

	/* stop the two thread */
	pthread_cancel(filter_thread);
	pthread_cancel(interprets_and_sends_thread);

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
	new_sensor->next = sensors;
	sensors = new_sensor;
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
	/* initialisation case */
	if(current_sensor->sensor == char_id_sensor){
		sensors = sensors->next;
		free(sensors);
	}
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
Permet d'envoyer des données à un capteur (sans retour de sa part)
\param id_sensor Identifiant unique du capteur à contacter
id_trame Identifiant de la trame à envoyer
\return 0 si tout est ok, > 0 si erreur
*/
int drv_send_data( unsigned int id_sensor, char commande )
{
    return 42;
}


/**
Retourne les informations concernant le driver (nom, version, ...)
\param  buffer		Chaîne de caractère qui reçoit les données
	max_length	Taille du buffer max
*/
void drv_get_info( char* buffer, int max_length )
{
    strcpy( buffer, "Driver SunSpot, v1.0" );
}