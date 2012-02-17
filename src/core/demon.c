#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <time.h>

#include "XMLParser.h"
#include "ios_api.h"
//#include "engine.h"
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
	/* avec reconstruction totale du fichier */
		int i = 0;
	int j = 0;
	FILE * raw_data = fopen("raw_data.json", "w");
	char * entete = (char*)malloc(65535);
	entete = "{\n\"raw_data\": {\n\0";
	
	//free(entete);

	/* Entete du fichier */
	j = 0;
	while (entete[j] != '\0')
	{
		putc(entete[j], raw_data);
		printf("Ecriture entete en cours : char %c !!!!\n", entete[j]);
		j++;	
	}
	printf("Entete ecrit !!!!\n");
	/* Info des capteurs installes. */
	for (i = 0; i < MAX_NUMBER_OF_SENSORS; i++)
	{
		printf("Capteur trouve !!!!\n");

		if (sensor[i].name != 0)
		{
			printf("Capteur trouve !!!!\n");
			char * capt = (char*)malloc(65535);
			float but1 = 0; 
			float but2 = 0; 
			float but3 = 0;
			float but4 = 0;
			float but5 = 0;
			float but6 = 0;
			float but7 = 0;
			float but8 = 0;
			float temp = 0;
			float humi = 0;
			float lumi = 0;
			float volt = 0;
			ios_read(sensor[i].fd, DRV_FIELD_BUTTON1, &but1);
			ios_read(sensor[i].fd, DRV_FIELD_BUTTON2, &but2);
			ios_read(sensor[i].fd, DRV_FIELD_BUTTON3, &but3);
			ios_read(sensor[i].fd, DRV_FIELD_BUTTON4, &but4);
			ios_read(sensor[i].fd, DRV_FIELD_BUTTON5, &but5);
			ios_read(sensor[i].fd, DRV_FIELD_BUTTON6, &but6);
			ios_read(sensor[i].fd, DRV_FIELD_BUTTON7, &but7);
			ios_read(sensor[i].fd, DRV_FIELD_BUTTON8, &but8);
			ios_read(sensor[i].fd, DRV_FIELD_TEMPERATURE, &temp);
			ios_read(sensor[i].fd, DRV_FIELD_HUMIDITY, &humi);
			ios_read(sensor[i].fd, DRV_FIELD_LIGHTING, &lumi);
			ios_read(sensor[i].fd, DRV_FIELD_VOLTAGE, &volt);
			sprintf(capt,"%s{\n\"Bouton1\":\"%f\";\n\"Bouton2\":\"%f\";\n\"Bouton3\":\"%f\";\n\"Bouton4\":\"%f\";\n\"Bouton5\":\"%f\";\n\"Bouton6\":\"%f\";\n\"Bouton7\":\"%f\";\n\"Bouton8\":\"%f\";\n\"Temperature\":\"%f\";\n\"Humidite\":\"%f\";\n\"Luminosite\":\"%f\";\n\"Voltage\":\"%f\";\n}", sensor[i].name, but1, but2, but3, but4, but5, but6, but7, but8,temp, humi, lumi, volt);
		
			/* Ecriture des donnees dans le fichier */
			j = 0;
			while (capt[j] != '\0')
			{
				putc(capt[j], raw_data);
				j++;
				printf("Ecriture des infos en cours !!!!\n");
			}
			printf("Infos capteurs ecrites !!!!\n");

			//free(capt);
		}
	}

	/* Fin du fichier. */
	putc('}',raw_data);
	putc('\n',raw_data);

	fclose(raw_data);
	printf("Fermeture du fichier !!!!\n");

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

/**
 * Donne le file descriptor d'un capteur installé à partir de son nom
 * \param name le nom du capteur
 * \return le fd ou -1 si le capteur n'est pas installé
 */
int get_fd_by_name(char* name){
	int i = 0;
	for(i=0; i<MAX_NUMBER_OF_SENSORS; i++)
	{
		if(strcmp((char *)sensor[i].name, name) != 0)
			return sensor[i].fd;
	}
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
				printf("\tname = %s\n", (char*)sensor[capteur_counter].name);
				xmlChar* id = xmlGetProp(capteur_node,(const xmlChar*)"id");
				int id_int = 0;
				id_int = atoi((const char*)id);
				sensor[capteur_counter].fd = ios_add_device( drv[driver_counter],(int)id);
				//printf("added on fd :%d\n", sensor[capteur_counter]);
				sensor[capteur_counter].id = (int)id;
				xmlFree(id);
			}
			xmlFree(etat);
			capteur_counter++;
		}
		driver_counter++;
	}
	xmlFreeDoc(capteurs_doc);

	int i=0;
	for(i=0; i<MAX_NUMBER_OF_SENSORS; i++)
	{
		printf("fd: %d, name: %s\n", sensor[i].fd, sensor[i].name);
	}
	return 0;
}

void json_writer_loop(){

}
