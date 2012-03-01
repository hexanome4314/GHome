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

void interpret(sunspot_data_structure* message, int* msgq_id){
	struct msg_drv_notify msgTemp;/* Message pour la temperature. */
	struct msg_drv_notify msgLumi;/* Message pour la luminosite. */

	// Info du capteur de type luminosite
	msgLumi.flag_value = DRV_FIELD_BRIGHTNESS;
	msgLumi.msg_type = DRV_MSG_TYPE;
	msgLumi.value = message->light;
	msgLumi.id_sensor = message->address;
    msgsnd( *msgq_id, (const void*) &msgLumi, sizeof(struct msg_drv_notify) - sizeof(long), 0 );    
    if(LOG)
    {
        printf("Capteur : %X Luminosite : %i !!!!\n", message->address, (int) msgLumi.value);
    }    
    
   	// Info du capteur de type temperature
	msgTemp.flag_value = DRV_FIELD_TEMPERATURE;
	msgTemp.msg_type = DRV_MSG_TYPE;
	msgTemp.value = message->temperature;
	msgTemp.id_sensor = message->address;
    msgsnd( *msgq_id, (const void*) &msgTemp, sizeof(struct msg_drv_notify) - sizeof(long), 0 );
    if(LOG)
    {
        printf("Capteur : %X Temperature : %f !!!!\n", message->address, msgTemp.value);	
    }
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
	char_buffer = (char*)malloc(MESSAGE_LEN+1);
	memset(char_buffer,0,sizeof(char_buffer));

	for(;;){ /* begin RECEIVE&FILTER THREAD LOOP */

		//printf("listen ---------------- I'm listening\n");

		/* LOOP get a new frame */
		if(recv(sock, char_buffer, MESSAGE_LEN, MSG_WAITALL)==-1)
			perror("listen - recv fail");
		printf("\nlisten - recv: %s \n",char_buffer);
		/*if(SIMULATION){
			char backslash_n;
			recv(sock, &backslash_n, 1, MSG_WAITALL);
			printf("listen - SIMULATION backslash_n %c ",backslash_n);
		}*/

		/* LOOP filter */
		sensors_queue* p_sensor = sensors->next;
		if(sensors == NULL)
		{
			printf("listen - la liste géré par le driver sunspot est vide\n");
		}
        else
        {            
            while(p_sensor != NULL)
            {
                if(strlen(char_buffer) >= SENSORNAME_LEN && memcmp(p_sensor->sensor, char_buffer, SENSORNAME_LEN))
                {
                    sem_wait(&to_send_receive);
                    printf("listen - message from one of ours sensors! \n");
                    interesting_frame = char_buffer;
                    sem_post(&to_send);
                    char_buffer = (char*)malloc(MESSAGE_LEN);
                    memset(char_buffer, 0, sizeof(char_buffer));
                    break; /* no need to go throw the end of the list */
                }
                p_sensor = p_sensor->next;
            }
            if(p_sensor == NULL && interesting_frame != char_buffer){
                printf("listen - message i don't care about \n");
            }
        }
	} /* end RECEIVE&FILTER THREAD LOOP */
}
/**
 * Second thread: interpret the frame and sem post
 * \param the semaphore where to put
 */
void interpretAndSend(int* msgq_id)
{
	sunspot_data_structure* message;

	for(;;){ /* begin INTERPRET&SEND THREAD LOOP */

		/* get the new message */
		message = malloc(sizeof(sunspot_data_structure));
		char* buffer = NULL;
		sem_wait(&to_send);
		{
			buffer = interesting_frame;
			interesting_frame = NULL;
		}
		sem_post(&to_send_receive);
        sscanf(buffer, "%x %d %e", (int *) &(message->address), (int *) &(message->light), (float *) &(message->temperature));
		free(buffer);

		/* send the message */
		if(message!=NULL)
        {
            interpret(message, msgq_id);
			free(message);
			message = NULL;
		}
        else
        {
			printf("interpret&send - Tango Charlie :\n");
			sleep(5);
		}
	}/* end INTERPRET&SEND THREAD LOOP */
}
