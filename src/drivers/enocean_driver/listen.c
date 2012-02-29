#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include "sys/msg.h"
#include "listen.h"
#include "utils.h"
#include "../drv_api.h"

/* Thread's communication */

sem_t to_send;
sem_t to_send_receive;

char* interesting_frame;

/********************************************* PRIVATES FUNCTIONS */

/**
 * Change a "A55A0B051000000043141337306E" char* as
 * we get from EnOcean Sensors into a enocean_data_structure
 * \param the frame
 * \param a enocean_data_structure where to put the information
 */
void parser(char* aFrame, enocean_data_structure* aMessage){
	sscanf(&aFrame[0],"%02X",(unsigned int*) &(aMessage->SYNC_BYTE1));
	sscanf(&aFrame[2],"%02X",(unsigned int*) &(aMessage->SYNC_BYTE2));
	sscanf(&aFrame[4],"%02X",(unsigned int*) &(aMessage->H_SEQ_LENGTH));
	sscanf(&aFrame[6],"%02X",(unsigned int*) &(aMessage->ORG));
	sscanf(&aFrame[8],"%02X",(unsigned int*) &(aMessage->DATA_BYTE3));
	sscanf(&aFrame[10],"%02X",(unsigned int*) &(aMessage->DATA_BYTE2));
	sscanf(&aFrame[12],"%02X",(unsigned int*) &(aMessage->DATA_BYTE1));
	sscanf(&aFrame[14],"%02X",(unsigned int*) &(aMessage->DATA_BYTE0));
	sscanf(&aFrame[16],"%02X",(unsigned int*) &(aMessage->ID_BYTE3));
	sscanf(&aFrame[18],"%02X",(unsigned int*) &(aMessage->ID_BYTE2));
	sscanf(&aFrame[20],"%02X",(unsigned int*) &(aMessage->ID_BYTE1));
	sscanf(&aFrame[22],"%02X",(unsigned int*) &(aMessage->ID_BYTE0));
	sscanf(&aFrame[24],"%02X",(unsigned int*) &(aMessage->STATUS));
	sscanf(&aFrame[26],"%02X",(unsigned int*) &(aMessage->CHECKSUM));
}

void _interpretAndSendRPS(enocean_data_structure* a_RPS_message, int* msgq_id){

	/* Un message pour chaque etat de bouton a mettre a jour */
	struct msg_drv_notify msg;
	struct msg_drv_notify msg2;
	

	unsigned int id; /* Id du capteur */
	
	/* Reconstruction de l'id du capteur a partir des ID_BYTE */
	id = a_RPS_message->ID_BYTE3;
	id = id << 8;
	id += a_RPS_message->ID_BYTE2;
	id = id << 8;
	id += a_RPS_message->ID_BYTE1;
	id = id << 8;
	id += a_RPS_message->ID_BYTE0;

	msg.msg_type = DRV_MSG_TYPE;
	msg.id_sensor = id;

	msg2.msg_type = DRV_MSG_TYPE;
	msg2.id_sensor = id;

	switch (a_RPS_message->DATA_BYTE3)
	{
		/* Bouton gauche en bas */
		case 0x10 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Bouton gauche en bas !!!!\n", id);
			}
			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = -1;

			msg2.flag_value = DRV_FIELD_BUTTON2;
			msg2.value = 0;
		} break;

		/* Bouton gauche en haut */
		case 0x30 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Bouton gauche en haut !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = 1;

			msg2.flag_value = DRV_FIELD_BUTTON2;
			msg2.value = 0;
		} break;

		/* Bouton droite en bas */
		case 0x50 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Bouton droite en bas !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = 0;

			msg2.flag_value = DRV_FIELD_BUTTON2;
			msg2.value = -1;
		} break;

		/* Bouton droit en haut */
		case 0x70 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Bouton droit en haut !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = 0;

			msg2.flag_value = DRV_FIELD_BUTTON2;
			msg2.value = 1;
		} break;

		/* Bouton gauche et droite en haut */
		case 0x37 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Bouton gauche et droite en haut !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = 1;

			msg2.flag_value = DRV_FIELD_BUTTON2;
			msg2.value = 1;
		} break;

		/* Bouton gauche et droite en bas */
		case 0x15 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Bouton gauche et droite en bas !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = -1;

			msg2.flag_value = DRV_FIELD_BUTTON2;
			msg2.value = -1;
		} break;

		/* Bouton gauche en bas et droite en haut */
		case 0x17 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Bouton gauche en bas et droite en haut !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = -1;

			msg2.flag_value = DRV_FIELD_BUTTON2;
			msg2.value = 1;
		} break;

		/* Bouton gauche en haut et droite en bas */
		case 0x35 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Bouton gauche en haut et droite en bas !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = 1;

			msg2.flag_value = DRV_FIELD_BUTTON2;
			msg2.value = -1;
		} break;

		/* Bouton gauche et droite non appuyes */
		case 0x00 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Bouton gauche et droite non appuyes !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = 0;

			msg2.flag_value = DRV_FIELD_BUTTON2;
			msg2.value = 0;
		} break;

		default :
		{
			return;
		}
	}

	msgsnd( *(msgq_id), (const void*) &msg, sizeof(struct msg_drv_notify) - sizeof(long), 0 );
	msgsnd( *(msgq_id), (const void*) &msg2, sizeof(struct msg_drv_notify) - sizeof(long), 0 );
}

void _interpretAndSend1BS(enocean_data_structure* a_RPS_message, int* msgq_id){

	struct msg_drv_notify msg; /* Message pour l'etat du capteur */
	unsigned int id; /* Id du capteur */

	/* Reconstruction de l'id du capteur a partir des ID_BYTE */
	id = a_RPS_message->ID_BYTE3;
	id = id << 8;
	id += a_RPS_message->ID_BYTE2;
	id = id << 8;
	id += a_RPS_message->ID_BYTE1;
	id = id << 8;
	id += a_RPS_message->ID_BYTE0;
	msg.id_sensor=id;
	msg.msg_type = DRV_MSG_TYPE;

	switch (a_RPS_message->DATA_BYTE0)
	{
		/* Contact ouvert */
		case 0x08 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Contact ouvert !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = 0;
		} break;

		case 0x09 :
		{
			if (LOG)
			{
				printf("listen - Capteur : %X Contact ferme !!!!\n", id);
			}

			msg.flag_value = DRV_FIELD_BUTTON1;
			msg.value = 1;
		} break;

		default :
		{
			return;
		}
	}

	msgsnd( *(msgq_id), (const void*) &msg, sizeof(struct msg_drv_notify) - sizeof(long), 0 );
}

void _interpretAndSend4BS(enocean_data_structure* a_RPS_message, int* msgq_id){
	struct msg_drv_notify msgTemp; /* Message pour la temperature. */
	struct msg_drv_notify msg2;/* Message pour la luminosite. */
	struct msg_drv_notify msg3;/* Message pour le voltage. */
	unsigned int id; /* Id du capteur */
	int resp; /* Reponse pour l'envoie du message */

	/* Reconstruction de l'id du capteur a partir des ID_BYTE */
	id = a_RPS_message->ID_BYTE3;
	id = id << 8;
	id += a_RPS_message->ID_BYTE2;
	id = id << 8;
	id += a_RPS_message->ID_BYTE1;
	id = id << 8;
	id += a_RPS_message->ID_BYTE0;

	// Info du capteur de type temperature.
	msgTemp.flag_value = DRV_FIELD_TEMPERATURE;
	msg2.flag_value = DRV_FIELD_LIGHTING;
	msg3.flag_value = DRV_FIELD_VOLTAGE;

	// On calcule la temperature du capteur.
	msgTemp.value = a_RPS_message->DATA_BYTE1 * 40/255;
	msgTemp.id_sensor = id;

	msg2.value = a_RPS_message->DATA_BYTE2 * 510/255;
	msg2.id_sensor = id;

	msg3.value = a_RPS_message->DATA_BYTE3 * 5.1/255;
	msg3.id_sensor = id;

	if (LOG)
	{
		printf("listen - Capteur : %X Temperature : %f !!!!\n", id, msgTemp.value);
		printf("listen - Capteur : %X Luminosite : %f !!!!\n", id, msg2.value);
		printf("listen - Capteur : %X Voltage : %f !!!!\n", id, msg3.value);
	}

	resp = msgsnd( (msgqnum_t)msgq_id, (const void*) &msgTemp, sizeof(struct msg_drv_notify) - sizeof(long), 0 );
	resp = msgsnd( (msgqnum_t)msgq_id, (const void*) &msg2, sizeof(struct msg_drv_notify) - sizeof(long), 0 );
	resp = msgsnd( (msgqnum_t)msgq_id, (const void*) &msg3, sizeof(struct msg_drv_notify) - sizeof(long), 0 );
}

/********************************************* PUBLICS FUNCTIONS */

void initialisation_for_listener(){

	sem_init(&to_send,0,0);
	sem_init(&to_send_receive,0,1); /* 1 as the init value in order not to block in the first loop */
}
/**
 * First thread: Receive the flow filter frame we are interested in
 * and give them to second thread
 * \param socket from which arrive the flow of frame
 */

int listenAndFilter(listen_and_filter_params* params)
{
	/* make the params local */
	int sock = params->sock;
	sensors_queue* sensors = params->sensors;
	free(params);

	/* useful stuffs for the main loop */
	char* char_buffer;
	char_buffer = (char*)malloc(28);
	memset(char_buffer,0,sizeof(char_buffer));

	for(;;){ /* begin RECEIVE&FILTER THREAD LOOP */

		//printf("listen ---------------- I'm listening\n");

		/* LOOP get a new frame */
		if(recv(sock, char_buffer, 28,MSG_WAITALL)==-1)
			perror("listen - recv fail");
		printf("listen - recv: %s \n",char_buffer);
		if(SIMULATION){
			char backslash_n;
			recv(sock, &backslash_n,1,MSG_WAITALL);
			printf("listen - SIMULATION backslash_n %c ",backslash_n);
		}

		/* LOOP filter */
		sensors_queue* p_sensor;
		while(sensors == NULL)
		{
			sleep(1);
			printf("listen - la liste géré par le driver enocean est vide\n");
		}
		for(p_sensor = sensors ; (sensors_queue*)p_sensor->next != NULL ; p_sensor = p_sensor->next){
			
			if(   (char)p_sensor->sensor[0] == (char) char_buffer[16]
			   && (char)p_sensor->sensor[1] == (char) char_buffer[17]
			   && (char)p_sensor->sensor[2] == (char) char_buffer[18]
			   && (char)p_sensor->sensor[3] == (char) char_buffer[19]
        	   	   && (char)p_sensor->sensor[4] == (char) char_buffer[20]
		           && (char)p_sensor->sensor[5] == (char) char_buffer[21]
		           && (char)p_sensor->sensor[6] == (char) char_buffer[22]
			   && (char)p_sensor->sensor[7] == (char) char_buffer[23]
			){
				/* this message is from one of our sensors ! */
				sem_wait(&to_send_receive); /* sychronisation with the interpret&send thread */
				printf("listen - message from one of ours sensors! \n");
				interesting_frame = char_buffer;
				sem_post(&to_send);
				char_buffer = (char*)malloc(28);
				memset(char_buffer,0,sizeof(char_buffer));
				break; /* no need to go throw the end of the list */
			}
		}
		if(p_sensor->next == NULL && interesting_frame != char_buffer){
			printf("listen - message i don't care about \n");
		}
	} /* end RECEIVE&FILTER THREAD LOOP */
}
/**
 * Second thread: interpret the frame and sem post
 * \param the semaphore where to put
 */
void interpretAndSend(int* msgq_id){

	enocean_data_structure* message;

	for(;;){ /* begin INTERPRET&SEND THREAD LOOP */

		/* get the new message */
		message = malloc(sizeof(enocean_data_structure));
		char* buffer = NULL;
		sem_wait(&to_send);
		{
			buffer = interesting_frame;
			interesting_frame = NULL;
		}
		sem_post(&to_send_receive);
		parser(buffer,message);
		free(buffer);

		/* send the message */
		if (message!=NULL){
			switch (message->ORG){
			case 0x05: if(LOG) printf("interpret&send - It's an RPS!\n");
					   _interpretAndSendRPS(message, msgq_id);
					   break;
			case 0x06: if(LOG) printf("interpret&send - It's an 1BS!\n");
					   _interpretAndSend1BS(message, msgq_id);
					   break;
			case 0x07: if(LOG) printf("interpret&send - It's an 4BS!\n");
			           _interpretAndSend4BS(message, msgq_id);
					   break;
			case 0x58: if(LOG) printf("interpret&send - It's an acquitment!\n");
			if (message->H_SEQ_LENGTH == 0x8B)
			{
				printf("interpret&send - Acquitement recu !\n");
			}
			break;
			default: if(LOG) printf("interpret&send - Don't know what this f42king message is about\n");
			}
			free(message);
			message = NULL;
		}else{
			printf("interpret&send - Tango Charlie :\n");
			sleep(5);
		}
	}/* end INTERPRET&SEND THREAD LOOP */
}


