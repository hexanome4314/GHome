#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#include "listen.h"
#include "utils.h"

#define IP_BORNE_ENOCEAN "127.0.0.1"
#define PORT_BORNE_ENOCEAN 1338

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
	sscanf(&aFrame[20],"%02X",(unsigned int*) &(aMessage->STATUS));
	sscanf(&aFrame[22],"%02X",(unsigned int*) &(aMessage->CHECKSUM));
}

int main()
{
	int sock;
	int loop = 1;
	char* buffer;
	enocean_data_structure message;
	buffer = malloc(29);

	sock = connect_to(inet_addr(IP_BORNE_ENOCEAN), PORT_BORNE_ENOCEAN, 0);

	{ // politeness
		char hi[26] = "Hi from Hx4314's driver!/0";
		send(sock,hi,sizeof(hi),0);
	}


	// as long as you answer 1 you get a new message
	while(loop == 1){
		memset(buffer,0,sizeof(buffer));
		printf("listen - I'm listening\n");
		recv(sock, buffer, 28,MSG_WAITALL);
		perror(errno);
		buffer[29] = '\0';
		printf("listen - recv : %s \n",buffer);
		parser(buffer, &message);
		scanf("%d",&loop);
	}

	shutdown(sock,2);
	return 1;
}
