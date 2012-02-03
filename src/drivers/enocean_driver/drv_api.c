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
	sensors_file = fopen(SENSORS_FILE,"rt");
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
	return sensors;
}


/* ---------- Methodes public du pilote ---------- */

/*int main(){
	// false application
	int msgq_id;
	msgq_id = msgget(IPC_PRIVATE, 0600 | IPC_CREAT | IPC_EXCL );

	// run the driver code
	printf("MAIN drv_init\n");
	drv_init("127.0.0.1",1338);
	sleep(5);
	printf("MAIN drv_run\n");
	drv_run(msgq_id);
	sleep(60);
	printf("MAIN drv_stop\n");
	drv_stop();

	// check for the result
	msg_drv_notify buffer;
	msgrcv( msgq_id, (void*) &buffer, sizeof(buffer) - sizeof(long), DRV_MSG_TYPE, 0 );
	printf( "#message  [%d,%d,%d]\t[%d]\n", buffer.id_sensor, buffer.flag_value, buffer.value, msgq_id );

	return 42;
}*/

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
Permet de demander des informations à un capteur
\param id_sensor Identifiant unique du capteur à interroger
id_trame Identifiant de la trame à envoyer
buffer Buffer qui va recevoir les données de retour
max_length Taille maximale du buffer
\return 0 si erreur, ou la taille des données lues
*/
int drv_fetch_data( unsigned int id_sensor, unsigned int id_trame, char* buffer, int max_length );


/**
Permet d'envoyer des données à un capteur (sans retour de sa part)
\param id_sensor Identifiant unique du capteur à contacter
id_trame Identifiant de la trame à envoyer
\return 0 si tout est ok, > 0 si erreur
*/
int drv_send_data( unsigned int id_sensor, char commande )
{
	unsigned int id = 0;
	char* idHexa = (char*)malloc(8);
	char trame [29];
	/* Fabrique les elements principaux de la trame pour calculer le CheckSum */
	unsigned int hSEQ_LEN= 0x6B;
	unsigned int org = 0x05;
	unsigned int dataByte3 = 0;
	unsigned int dataByte2 = 0x00;
	unsigned int dataByte1 = 0x00;
	unsigned int dataByte0 = 0x00;
	unsigned int ID3 = (id_sensor & 0xFF000000);
	unsigned int ID2 = (id_sensor & 0x00FF0000);
	unsigned int ID1 = (id_sensor & 0x0000FF00);
	unsigned int ID0 = (id_sensor & 0x000000FF);
	unsigned int status = 0x30;
	unsigned int checkSum = 0;
	ID3 = ID3>>24;
	ID2 = ID2>>16;
	ID1 = ID1>>8;

	/* On verifie si on allume ou on eteint l'actionneur */
	if (commande  == 0)
	{
		/* Pour activer l'actionneur. */
		dataByte3 = 0x50;
	}
	else
	{
		/* Pour désactiver l'actionneur */
		dataByte3 = 0x70;
	}

	checkSum = (hSEQ_LEN + org + dataByte3 + dataByte2 + dataByte1 + dataByte0 + ID3 + ID2 + ID1 + ID0 + status) % 256;

	printf("%X %X\nDataBytes : %X %X %X %X\nID : %X %X %X %X\nStatus : %X\nCheckSume : %X\n", hSEQ_LEN, org, dataByte3, dataByte2, dataByte1, dataByte0, ID3, ID2, ID1, ID0, status, checkSum);
	
	/* Fabrication de la tramme */
	trame[0] = 'A';
	trame[1] = '5';
	trame[2] = '5';
	trame[3] = 'A';
	trame[4] = '6';
	trame[5] = 'B';
	trame[6] = '0';
	trame[7] = '5';
	if (commande == 0)
	{
		/* Pour activer l'actionneur. */
		trame[8] = '5';
	}
	else
	{
		/* Pour désactiver l'actionneur */
		trame[8] = '7';
	}
	trame[9] = '0';
	trame[10] = '0';
	trame[11] = '0';
	trame[12] = '0'; 
	trame[13] = '0';
	trame[14] = '0';
	trame[15] = '0';
	id = ID3 & 0xF0;
	id = id >> 4;
	snprintf(idHexa,2,"%X", id);
	trame[16] = *idHexa;
	id = ID3 & 0x0F;
	snprintf(idHexa,2,"%X", id);
	trame[17] = *idHexa;
	id = ID2 & 0xF0;
	id = id >> 4;
	snprintf(idHexa,2,"%X", id);
	trame[18] = *idHexa;
	id = ID2 & 0x0F;
	snprintf(idHexa,2,"%X", id);
	trame[19] = *idHexa;
	id = ID1 & 0xF0;
	id = id >> 4;
	snprintf(idHexa,2,"%X", id);
	trame[20] = *idHexa;
	id = ID1 & 0x0F;
	snprintf(idHexa,2,"%X", id);
	trame[21] = *idHexa;
	id = ID0 & 0xF0;
	id = id >> 4;
	snprintf(idHexa,2,"%X", id);
	trame[22] = *idHexa;
	id = ID0 & 0x0F;
	snprintf(idHexa,2,"%X", id);
	trame[23] = *idHexa;
	trame[24] = '3';
	trame[25] = '0';
	id = checkSum & 0xF0;
	id = id >> 4;
	snprintf(idHexa,2,"%X", id);
	trame[26] = *idHexa;
	id = checkSum & 0x0F;
	snprintf(idHexa,2,"%X", id);
	trame[27] = *idHexa;
	free(idHexa);
	printf("\n");
	trame[28]='\0';
	printf("drv_api - trame envoyé: %s\n", trame);
	printf("\n");

	/* Envoyer la trame */
	if(send(sock,trame,28,0)<0){
		int send_error = errno;
		perror("error sending frame to enocean base");
		return(send_error);
	}
	return 0;
}

