#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "ios_api.h"
#include "engine.h"
#include "demon.h"

int main(){
	int drv;
	int fd[3];

	ios_init();

	drv = ios_install_driver( "mon_driver.so.1.0.1", "127.0.0.1", 8080 );
	if( drv < 0 )
	{
		ios_release();
		return drv;
	}

	xmlDocPtr capteurs_doc;
	xmlNodePtr node_capteur;
	xmlNodePtr node_infos;
	
	capteurs_doc = xmlParseFile("capteurs.xml");
	node_capteur = capteurs_doc->children;
	{
		printf("LOOP\n");
		int loop_counter = 0;
		printf("node_capteur %s\n",node_capteur->name);
		for( 	node_infos = node_capteur->children;
			node_infos != NULL;
			node_infos = node_infos->next)
		{
			printf("\t%s\n",node_infos->name);
			xmlChar* id = xmlGetProp(node_capteur,(const xmlChar*)"id");
			fd[loop_counter] = ios_add_device( drv,(int) id );
			loop_counter++;
		}
		printf("end LOOP");
	}
	xmlFreeDoc(capteurs_doc);
	return 0;
}
