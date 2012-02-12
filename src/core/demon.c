#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "XMLParser.h"
#include "ios_api.h"
#include "engine.h"
#include "demon.h"


int main(){

	xmlDocPtr capteurs_doc;
	xmlNodePtr document_node;
	xmlNodePtr driver_node;
	xmlNodePtr capteur_node;
	
	capteurs_doc = xmlParseFile("drivers.xml");
	perror("xmlParseFile");
	document_node = capteurs_doc->children;

	/* get the number of drivers and of capteurs */
	int drv;
	int fd[9];

	/* init, start drivers and add all the capteurs */
	ios_init();
	int driver_counter = 0;	
	for( 	driver_node = document_node->children;
		driver_node != NULL;
		driver_node = driver_node->next)
	{
		//drv = ios_install_driver( "mon_driver.so.1.0.1", "127.0.0.1", 8080 );
		if( drv < 0 )
		{	
			ios_release();
			return drv;
		}
		int capteur_counter = 0;
		printf("%s,%i\n",driver_node->name,driver_node->type);
		for( 	capteur_node = document_node->children;
			capteur_node != NULL;
			capteur_node = capteur_node->next)
		{
			printf("\t%s\n",capteur_node->name);
			//xmlChar* id = xmlGetProp(document_node,(const xmlChar*)"id");
			//fd[capteur_counter] = ios_add_device( drv,(int) id );
			capteur_counter++;
		}
		printf("end LOOP");
		drv[driver_counter]++;
	}
	if(driver_node == NULL)
		printf("no child\n ");
	xmlFreeDoc(capteurs_doc);
	return 0;
}

