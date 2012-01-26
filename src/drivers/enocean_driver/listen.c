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

    /* get the sensors_id from "sensors_file"*/
	int number_of_sensors;
	char** sensors = read_sensors_list_file(&number_of_sensors);

	int sock;
	int loop_counter1;
	int loop_counter2;
	char* char_buffer;

	char_buffer = malloc(29);
	memset(char_buffer,0,sizeof(char_buffer));

	sock = connect_to(inet_addr(IP_BORNE_ENOCEAN), PORT_BORNE_ENOCEAN, 0);

	if(sock == -1){
		perror("listen - Start driver fail due to socket connection");
		return -1;
	}else{ /* politeness */
		send(sock,"Hi from Hx4314's driver!",25,0);
	}

	/* creation of a thread for the interpretation and message sending stuff */
	sem_init(&to_send,0,0);
	sem_init(&to_send_receive,0,1); /* 1 as the init value in order not to block in the first loop */
	void (*p_function_to_interpret_and_send);
	p_function_to_interpret_and_send = &interpretAndSend;
	pthread_create(&interprets_and_sends_thread,NULL,p_function_to_interpret_and_send,NULL);


	for(loop_counter1=0 ; loop_counter1<10 ; loop_counter1++){

		printf("listen - I'm listening\n");

		/* get a new frame */
		if(recv(sock, char_buffer, 29,MSG_WAITALL)==-1)
			perror("listen - recv fail");
		printf("listen - recv: %s \n",char_buffer);

		/* filter */
		for(loop_counter2=0 ; loop_counter2<number_of_sensors ; loop_counter2++){
			if(   (char)sensors[loop_counter2][0] == (char) char_buffer[16]
			   && (char)sensors[loop_counter2][1] == (char) char_buffer[17]
			   && (char)sensors[loop_counter2][2] == (char) char_buffer[18]
			   && (char)sensors[loop_counter2][3] == (char) char_buffer[19]
        	   && (char)sensors[loop_counter2][4] == (char) char_buffer[20]
		       && (char)sensors[loop_counter2][5] == (char) char_buffer[21]
               && (char)sensors[loop_counter2][6] == (char) char_buffer[22]
		       && (char)sensors[loop_counter2][7] == (char) char_buffer[23]
			){
				printf("listen - message from one of ours sensors! \n");
				sem_wait(&to_send_receive);
				interesting_frame = char_buffer;
				sem_post(&to_send);
				break;
			}
		}
	}
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

	for(;;){

		/* get the new message */
		message = malloc(sizeof(enocean_data_structure));
		char* buffer;
		sem_wait(&to_send);
		buffer = interesting_frame;
		sem_post(&to_send_receive);
		parser(buffer,message);
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
	}
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
