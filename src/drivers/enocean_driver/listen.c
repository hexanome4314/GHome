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

#include "config.h"
#include "listen.h"
#include "utils.h"
#include "drv_api.h"

/********************************************* Thread communication */

pthread_t filter_thread;
pthread_t interprets_and_sends_thread;
//pthread_mutex_t to_send;
sem_t to_send;
sem_t end_send;

int main()
{
/*
 *  int number_of_sensors;
 *	void* sensors_IDs;
 *  sensors_IDs = read_sensors_list_file(&number_of_sensors);
*/
	int sock;
	int loop_counter1;
	int loop_counter2;
	char* char_buffer;
	char* interesting_frame;

	void (*p_function_to_interpret_and_send);
	p_function_to_interpret_and_send=&interpretAndSend;

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
	sem_init(&end_send,0,1); /* 1 as the init value in order not to block in the first loop */
	pthread_create(&interprets_and_sends_thread,NULL,p_function_to_interpret_and_send,NULL);

	for(loop_counter1=0 ; loop_counter1<10 ; loop_counter1++){

		printf("listen - I'm listening\n");

		/* get a new frame */
		if(recv(sock, char_buffer, 29,MSG_WAITALL)==-1)
			perror("listen - recv fail");
		printf("listen - recv: %s \n",char_buffer);

		/* filter */
		/*for(loop_counter2=0 ; loop_counter2<NUMBER_OF_CAPTEURS ; loop_counter2++){
			if(){

			}
		}*/
	}
	shutdown(sock,2);
	return 1;
}

void _interpretAndSendRPS(enocean_data_structure* a_RPS_message){

}

void interpretAndSend(){

	enocean_data_structure* message;

	for(;;){
		sem_wait(&to_send);

		/* create a new message */
	//	message = malloc(sizeof(enocean_data_structure));
	//	parser(char_buffer,enocean_buffer);

		/* send the message */
		sem_wait(&end_send);
		//message = enocean_buffer;
		sem_post(&to_send);

		if (message!=NULL){
			switch (message->ORG){
			case 0x05: if(LOG) printf("read - It's an RPS!\n");
					   _interpretAndSendRPS(message);
					   break;
			case 0x06: if(LOG) printf("read - It's an 1BS!\n");
					   break;
			case 0x07: if(LOG) printf("read - It's an 4BS!\n");
					   break;
			default: if(LOG) printf("read - Don't know what this f42king message is about\n");
			}
			free(message);
			message = NULL;
		}else{
			printf("Tango Charlie :\n");
			sleep(5);
		}
	}
}

/**
 * change a "A55A0B05100000000021CC31306E" char* as
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
