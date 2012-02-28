#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "sensor-parse.h"
#include "ios_api.h"

/* ------------------------------------------------------- OUTILS POUR xmllib2 */
 
/* Those blabla_XML_ELEMENT_NODE allow to write readable xml files
 * by removing the fake TEXT_ELEMENT_NODE created by \n and \t */

/**
 * it return the first :father children which is a real XML_ELEMENT_NODE 
 * \param father the xmlNodePtr of the node your intrested in finding it's children
*/
xmlNodePtr children_XML_ELEMENT_NODE(xmlNodePtr father){
	xmlNodePtr child = father->children;
	while(child != NULL && child->type != XML_ELEMENT_NODE){
		child = child->next;
	}
	return child;
}

/**
 * it return the first :node successor which is a real XML_ELEMENT_NODE 
 * \param node the xmlNodePtr of the node your intrested in finding it's next node
*/
xmlNodePtr next_XML_ELEMENT_NODE(xmlNodePtr node){
	node = node->next;
	while(node != NULL && node->type != XML_ELEMENT_NODE){
		node = node->next;
	}
	return node;
}

int read_sensors(const char* path, infos_sensor* sensor, int* drv)
{
	xmlDocPtr capteurs_doc;
	xmlNodePtr drivers_node;
	xmlNodePtr driver_node;
	xmlNodePtr capteur_node;
	
	capteurs_doc = xmlParseFile(path);
	if (capteurs_doc == NULL)
	{
		perror("xmlParseFile");
		return -1;
	}
	/* the first xml element node of the file */
	drivers_node = next_XML_ELEMENT_NODE(capteurs_doc->children);

	/* init, install drivers and add all the capteurs */
	ios_init();
	int i;
	for(i=0; i < MAX_NUMBER_OF_SENSORS ; i++)
	{
		sensor[i].fd=0;
		sensor[i].id=0;
		sensor[i].name=NULL;
		sensor[i].used_fields=0;
	}
	int driver_counter = 0;	
	int capteur_counter = 0;
	for( 	driver_node = children_XML_ELEMENT_NODE(drivers_node);
		driver_node != NULL;
		driver_node = next_XML_ELEMENT_NODE(driver_node))
	{
		/* install a driverX/ */
		xmlChar* drv_so_name = xmlGetProp(driver_node,(const xmlChar*)"so");
		xmlChar* drv_ip = xmlGetProp(driver_node,(const xmlChar*)"ip");
		xmlChar* drv_port = xmlGetProp(driver_node,(const xmlChar*)"port");
		unsigned int port;
		sscanf((char*) drv_port, "%u", &port);
		printf("DEBUG unsigned int = %u\n", port); 
		drv[driver_counter] = ios_install_driver((char*) drv_so_name, (char*) drv_ip, port);
		xmlFree(drv_so_name);
		xmlFree(drv_ip);
		xmlFree(drv_port);
		if( drv[driver_counter] < 0 )
		{	
			ios_release();
			perror("Driver loading error");
			return drv[driver_counter];
		}
		for( 	capteur_node = children_XML_ELEMENT_NODE(driver_node);
			capteur_node != NULL;
			capteur_node = next_XML_ELEMENT_NODE(capteur_node))
		{
			/* install a driverX/capteurY/ */
			xmlChar* etat = xmlGetProp(capteur_node,(const xmlChar*)"etat");
			if(etat[0] == 'O' && etat[1] == 'N'){
				sensor[capteur_counter].name = xmlNodeGetContent(capteur_node);
				printf("\tname = %s\n", (char*)sensor[capteur_counter].name);
				xmlChar* id = xmlGetProp(capteur_node,(const xmlChar*)"id");
				int id_int;
				sscanf((char*)id, "%X", &id_int);
				sensor[capteur_counter].fd = ios_add_device( drv[driver_counter],id_int);
				sensor[capteur_counter].id = id_int;
				xmlFree(id);
			}
			xmlFree(etat);
			capteur_counter++;
		}
		driver_counter++;
	}
	xmlFreeDoc(capteurs_doc);
	return 0;
}
