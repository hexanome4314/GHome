#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <time.h>

#include "ios_api.h"
#include "engine.h"
#include "fiels.h"
#include "core.h"

static int drv[MAX_NUMBER_OF_DRIVERS];
static infos_sensor sensor[MAX_NUMBER_OF_SENSORS];

// Association id champ => nom
static char* Fields[] = {
#define X(define, str) str,
FIELDS
#undef X
};


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
	printf("Got some data from %d on field %d : %f\n", device, field, val);
	/* appeler l'ia pour effectuer les actions necessaires */
	apply_actions(device, field, val);
	/* écrire dans raw_data.json les valeurs */
	/* avec reconstruction totale du fichier */
	int i = 0;
	int j = 0;
	FILE * raw_data = fopen("raw_data.json", "w");
	char * entete = (char*)malloc(65535);
	entete = "{\n    \"raw_data\": [\0";

	/* Entete du fichier */
	j = 0;
	while (entete[j] != '\0')
	{
		putc(entete[j], raw_data);
		j++;	
	}

	/* Info des capteurs installes. */
	int nbCapteur = 0; /* Pour gerer des elements d'ecriture du fichier. */
	for (i = 0; i < MAX_NUMBER_OF_SENSORS; i++)
	{
		if (sensor[i].name != 0)
		{
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
			sprintf(capt,"{\n\"name\": \"%s\",\n\"temperature\": \"%i\",\n\"humidity\": \"%i\",\n\"brightness\": \"%i\",\n\"voltage\": \"%i\"\n", sensor[i].name, (int)temp, (int)humi, (int)lumi, (int)volt);

			/* Ecriture des donnees dans le fichier */
			nbCapteur++;
			if (nbCapteur > 2)
			{
				putc('}', raw_data);
				putc(',', raw_data);
				putc('\n', raw_data);
			}
			else if (nbCapteur == 2)
			{
				putc(' ', raw_data);
				putc(' ', raw_data);
				putc(' ', raw_data);
				putc(' ', raw_data);
				putc('}', raw_data);
				putc(',', raw_data);
				putc('\n', raw_data);
			}

			/* Ecriture des donnees dans le fichier */
			j = 0;
			while (capt[j] != '\0')
			{
				putc(capt[j], raw_data);
				j++;
			}
		}
	}

	/* Fin du fichier. */
	putc('}',raw_data);
	putc(']',raw_data);
	putc('\n',raw_data);
	putc('}',raw_data);

	fclose(raw_data);
}

void print_sensors(){
	int i;
	printf("Configured sensors : \n");
	for(i=0; i<MAX_NUMBER_OF_SENSORS; i++)
		if(sensor[i].name)
			printf("Sensor : %s\n -FD : %d\n -ID : %.8X\n",
			       sensor[i].name, sensor[i].fd, sensor[i].id);
}

int main(){

	xmlDocPtr capteurs_doc;
	xmlNodePtr drivers_node;
	xmlNodePtr driver_node;
	xmlNodePtr capteur_node;
	
	capteurs_doc = xmlParseFile("config/sensors.xml");
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
				int id_int;
				sscanf(id, "%X", &id_int);
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
	print_sensors();
	init_engine("config/rules.xml", sensor);
	ios_attach_global_handler(process_data);
	sleep(5);
}

void json_writer_loop(){

}
