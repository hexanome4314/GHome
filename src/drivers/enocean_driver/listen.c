#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "listen.h"
#include "utils.h"
#include "drv_api.h"

/********************************************* Thread communication */

pthread_t filter_thread;
pthread_t interprets_and_sends_thread;

sem_t to_send;
sem_t to_send_receive;

char* interesting_frame;

int main()
{

	/* init the communication with the sensors base */
	int sock;
	sock = connect_to(inet_addr(IP_BORNE_ENOCEAN), PORT_BORNE_ENOCEAN, 0);
	if(sock == -1){
		perror("listen - Start driver fail due to socket connection");
		return -1;
	}else{ /* politeness */
		send(sock,"Hi from Hx4314's driver!",25,0);
	}

    /* get the sensors_id from "sensors_file"*/
	int number_of_sensors;
	sensors_queue* sensors = read_sensors_list_file(&number_of_sensors);

	/* creation of a thread for the interpretation and message sending stuff */
	sem_init(&to_send,0,0);
	sem_init(&to_send_receive,0,1); /* 1 as the init value in order not to block in the first loop */
	void (*p_function_to_interpret_and_send);
	p_function_to_interpret_and_send = &interpretAndSend;
	pthread_create(&interprets_and_sends_thread,NULL,p_function_to_interpret_and_send,NULL);

	/* useful stuffs for the main loop */
	int loop_counter1;
	char* char_buffer;
	char_buffer = (char*)malloc(28);
	printf( "malloc 0x%x \n", char_buffer );
	memset(char_buffer,0,sizeof(char_buffer));

	for(loop_counter1=0 ; loop_counter1<10 ; loop_counter1++){ /* begin RECEIVE&FILTER THREAD LOOP */

		//printf("listen ---------------- I'm listening\n");

		/* LOOP get a new frame */
		if(recv(sock, char_buffer, 28,MSG_WAITALL)==-1)
			perror("listen - recv fail \n");
		printf("listen - recv: %s \n",char_buffer);
		if(SIMULATION){
			char backslash_n;
			recv(sock, &backslash_n,1,MSG_WAITALL);
			printf("listen - backslash_n %c \n",backslash_n);
		}

		/* LOOP filter */
		printf("FILTRE ");
		sensors_queue* p_sensor;
		for(p_sensor = sensors ; (sensors_queue*)p_sensor->next != NULL ; p_sensor = p_sensor->next){
			/*printf("%c%c %c%c %c%c %c%c == %c%c %c%c %c%c %c%c ?\n",
					p_sensor->sensor[0],p_sensor->sensor[1],p_sensor->sensor[2],p_sensor->sensor[3],
					p_sensor->sensor[4],p_sensor->sensor[5],p_sensor->sensor[6],p_sensor->sensor[7],
					char_buffer[16],char_buffer[17],char_buffer[18],char_buffer[19],
					char_buffer[20],char_buffer[21],char_buffer[22],char_buffer[23]);
			*/
			if(   (char)p_sensor->sensor[0] == (char) char_buffer[16]
			   && (char)p_sensor->sensor[1] == (char) char_buffer[17]
			   && (char)p_sensor->sensor[2] == (char) char_buffer[18]
			   && (char)p_sensor->sensor[3] == (char) char_buffer[19]
        	   && (char)p_sensor->sensor[4] == (char) char_buffer[20]
		       && (char)p_sensor->sensor[5] == (char) char_buffer[21]
               && (char)p_sensor->sensor[6] == (char) char_buffer[22]
		       && (char)p_sensor->sensor[7] == (char) char_buffer[23]
			){
				sem_wait(&to_send_receive);
				/*if(sem_wait(&to_send_receive) == 0){
					int val;
					sem_getvalue(&to_send,&val);
					printf("listen - SEM GET to_send_receive %d\n",val);
				}else{
					printf("error during SEM GET to_send_receive %d",errno);
				}*/
				printf("listen - message from one of ours sensors! \n");
				interesting_frame = char_buffer;
				sem_post(&to_send);
				char_buffer = (char*)malloc(28);
				printf( "malloc 0x%x \n", char_buffer );
				memset(char_buffer,0,sizeof(char_buffer));

				/*if(sem_post(&to_send) == 0){
					int val;
					sem_getvalue(&to_send,&val);
					printf("listen - SEM PUT to_send %d\n",val);
				}else{
					printf("error during SEM PUT to_send %d",errno);
				}*/
				break;
			}
		}
		if(p_sensor->next == NULL && interesting_frame != char_buffer){
			printf("listen - message i don't care about \n");
		}
		printf("FIN FILTRE \n");
	} /* end RECEIVE&FILTER THREAD LOOP */
	shutdown(sock,2);
	return 1;
}

void _interpretAndSendRPS(enocean_data_structure* a_RPS_message){

}

void _interpretAndSend1BS(enocean_data_structure* a_RPS_message){

}

void _interpretAndSend4BS(enocean_data_structure* a_RPS_message){

}


void interpretAndSend(){

	enocean_data_structure* message;

	for(;;){ /* begin INTERPRET&SEND THREAD LOOP */
		//printf("interpret&send --------------------- start of a loop ! \n");
		/* get the new message */
		message = malloc(sizeof(enocean_data_structure));
		char* buffer = NULL;
		sem_wait(&to_send);
		/*if(sem_wait(&to_send) == 0){
			int val;
			sem_getvalue(&to_send,&val);
			printf("interpret&send - SEM GET to_send %d\n",val);
		}else{
			printf("error during SEM GET to_send %d",errno);
		}*/
		buffer = interesting_frame;
		interesting_frame = NULL;
		sem_post(&to_send_receive);
		/*if(sem_post(&to_send_receive) == 0){
			int val;
			sem_getvalue(&to_send,&val);
			printf("interpret&send - SEM PUT to_send_receive %d\n",val);
		}else{
			printf("error during SEM PUT to_send_receive %d",errno);
		}*/
		parser(buffer,message);
		printf( "free 0x%x \n", buffer );
		free(buffer);

		/* send the message */
		if (message!=NULL){
			switch (message->ORG){
			case 0x05: if(LOG) printf("interpret&send - It's an RPS!\n");
					   _interpretAndSendRPS(message);
					   break;
			case 0x06: if(LOG) printf("interpret&send - It's an 1BS!\n");
					   _interpretAndSend1BS(message);
					   break;
			case 0x07: if(LOG) printf("interpret&send - It's an 4BS!\n");
			           _interpretAndSend4BS(message);
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

/**
 * change a "A55A0B051000000043141337306E" char* as
 * we get from EnOcean Sensors into a enocean_data_structure
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

