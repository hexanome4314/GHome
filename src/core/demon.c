#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <time.h>

#include "XMLParser.h"
#include "ios_api.h"
#include "engine.h"
#include "demon.h"

int drv[MAX_NUMBER_OF_DRIVERS];
infos_sensor sensor[MAX_NUMBER_OF_SENSORS];


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

void process_data(int device, unsigned int field, float val)
{
	/* appeler l'ia pour effectuer les actions necessaires */

	/* écrire dans raw_data.json les valeurs */
}
/**
 * Donne le file descriptor d'un capteur installé à partir de son id
 * Si le capteur n'est pas installé retourne -1
*/
int get_fd(int id){
	int loop_counter = 0;
	while(id != MAX_NUMBER_OF_SENSORS && sensor[loop_counter].id != id){
		loop_counter++;
	}
	if(sensor[loop_counter].id == id)
		return sensor[loop_counter].fd;
	else
		return -1;
}

int init_demon(){

	xmlDocPtr capteurs_doc;
	xmlNodePtr drivers_node;
	xmlNodePtr driver_node;
	xmlNodePtr capteur_node;
	
	capteurs_doc = xmlParseFile("drivers.xml");
	perror("xmlParseFile");
	/* the first xml element node of the file */
	drivers_node = next_XML_ELEMENT_NODE(capteurs_doc->children);

	/* init, install drivers and add all the capteurs */
	ios_init();
	int driver_counter = 0;	
	int capteur_counter = 0;
	for( 	driver_node = children_XML_ELEMENT_NODE(drivers_node);
		driver_node != NULL;
		driver_node = next_XML_ELEMENT_NODE(driver_node))
	{
		/* install a driverX/ */
		xmlChar* drv_so_name = xmlGetProp(driver_node,(const xmlChar*)"so");
		drv[driver_counter] = ios_install_driver( (char*)drv_so_name, "127.0.0.1", 8080 );
		xmlFree(drv_so_name);
		if( drv[driver_counter] < 0 )
		{	
			ios_release();
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
				printf("\tname = %s\n", sensor[capteur_counter].name);
				xmlChar* id = xmlGetProp(drivers_node,(const xmlChar*)"id");
				sensor[capteur_counter].fd = ios_add_device( drv[driver_counter],(int)id);
				sensor[capteur_counter].id = (int)id;
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

int main(){
	init_demon();
	for(;;){
		sleep(5);
		puts("alive");
	}
}

void json_writer_loop(){

}
