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

xmlNodePtr next_XML_ELEMENT_NODE(xmlNodePtr node){
	node = node->next;
	while(node != NULL && node->type != XML_ELEMENT_NODE){
		node = node->next;
	}
	return node;
}

int main(){

	xmlDocPtr capteurs_doc;
	xmlNodePtr dtd_node;
	xmlNodePtr drivers_node;
	xmlNodePtr driver_node;
	xmlNodePtr capteur_node;
	
	capteurs_doc = xmlParseFile("drivers.xml");
	perror("xmlParseFile");
	/* the first node of the file is the DTD node */
	dtd_node = capteurs_doc->children;

	/* get the number of drivers and of capteurs */
	int drv = 1;
	int fd[9];

	/* init, start drivers and add all the capteurs */
	ios_init();
	int driver_counter = 0;	
	drivers_node = dtd_node->next;
	for( 	driver_node = drivers_node->children->next;
		driver_node != NULL;
		driver_node = next_XML_ELEMENT_NODE(driver_node))
	{
		printf("CORE/ driver - %s,%i\n",driver_node->name,driver_node->type);
		xmlChar* drv_so_name = xmlGetProp(driver_node,(const xmlChar*)"so");
		drv = ios_install_driver( (char*)drv_so_name, "127.0.0.1", 8080 );
		xmlFree(drv_so_name);
		if( drv < 0 )
		{	
			ios_release();
			return drv;
		}
		int capteur_counter = 0;
		for( 	capteur_node = driver_node->children->next;
			capteur_node != NULL;
			capteur_node = next_XML_ELEMENT_NODE(capteur_node))
		{
			printf("CORE/\tcapteur - %s,%i\n",capteur_node->name,capteur_node->type);
			xmlChar* id = xmlGetProp(drivers_node,(const xmlChar*)"id");
			fd[capteur_counter] = ios_add_device( drv,(int) id );
			capteur_counter++;
		}
		driver_counter++;
	}
	xmlFreeDoc(capteurs_doc);
	return 0;
}

