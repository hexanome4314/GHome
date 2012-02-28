#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "ios_api.h"
#include "engine.h"
#include "fields.h"
#include "core.h"
#include "remote-control.h"

/* ------------------------------------------------------------------- GLOBALS */

static int drv[MAX_NUMBER_OF_DRIVERS];
static infos_sensor sensor[MAX_NUMBER_OF_SENSORS]; /* variable globale fourni à l'IA */
static sem_t stop_sem; // Semaphore arret application

// Association id champ => nom
static char* Fields[] = {
#define X(define, str) str,
FIELDS
#undef X
};


/**
 * Handler d'arrêt de l'application
 * \param signum Signal interrompant l'application
 */
void stop_handler(int signum)
{
	sem_post(&stop_sem);
}


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


/* --------------------------------------------------- INFORMATIONS POUR DES HUMAINS */

/**
 * Écrit une chaîne descriptive des capteurs dans un flux specifique
 * \param output le flux de sortie
 */
void fprint_sensors(int output)
{
	int i;
	char msg[1024];
	sprintf(msg,"Configured sensors : \n");
	write(output, msg, strlen(msg));
	for(i=0; i<MAX_NUMBER_OF_SENSORS; i++)
	{
		if(sensor[i].name)
		{
			sprintf(msg, "Sensor \"%s\"\n├─ FD : %d\n└─ ID : %.8X\n",
				sensor[i].name, sensor[i].fd, sensor[i].id);
			write(output, msg, strlen(msg));
		}
	}
}

/** Affiche sur la sortie standard les infos sur les capteurs */
void print_sensors(){
	fprint_sensors(stdout->_fileno);
}


/* -------------------------------------------------------------- TELNET */

/**
 * Commande telnet d'arrêt
 */
int remote_stop_cmd(int fd, const char* cmd)
{
	char* msg = "GHome is stopping. Goodbye !\n";
	write(fd, msg, strlen(msg));
	sem_post(&stop_sem);
	return 0;
}

/**
 * Commande telnet de listage de capteurs
 */
int list_sensors_cmd(int fd, const char* cmd)
{
	fprint_sensors(fd);
	return 1;
}

/**
 * Commande telnet de listage de règles
 */
int list_rules_cmd(int fd, const char* cmd)
{
//	fprint_rules(fd);
	return 1;
}

/* ----------------------------------------------------------- FAT FONCTIONS :) */

/**
 *   
*/
void process_data(int device, unsigned int field, float val)
{
	printf("Got some data from %d on field %d : %f\n", device, field, val);
	/* appeler l'ia pour effectuer les actions necessaires */
	apply_actions(device, field, val);
	/* écrire dans raw_data.json les valeurs */
	/* avec reconstruction totale du fichier */
	int i = 0;
	FILE * raw_data = fopen("raw_data.json", "w");
	fprintf(raw_data, "{\n \"raw_data\": [");
	/* Info des capteurs installes. */
	int nbCapteur = 0; /* Pour gerer des elements d'ecriture du fichier. */
	for (i = 0; i < MAX_NUMBER_OF_SENSORS; i++)
	{
		if (sensor[i].name != 0)
		{
			int f;
			/* On indique que ce capteur utilise ce champ, et que ce champ
			   doit être écrit */
			if (sensor[i].fd == device)
				sensor[i].used_fields |= (1 << field);
			if (nbCapteur != 0)
				putc(',', raw_data);
			fprintf(raw_data,"\n {\n   \"name\": \"%s\"", sensor[i].name);
			for (f = 0; f < DRV_LAST_VALUE; f++)
			{
			/* On n'écrit que si le capteur utilise le champ */
				if (sensor[i].used_fields & (1 << f))
				{
					float value = 0;
					ios_read(sensor[i].fd, f, &value);
					fprintf(raw_data, ",\n   \"%s\": \"%f\"", Fields[f], value);
				}
				
			}
			fprintf(raw_data, "\n }");
			nbCapteur++;
		}
	}
	fprintf(raw_data, "\n]\n}");
	fclose(raw_data);
}

/**
 * Charge les drivers et ajoute les capteurs selon un fichier de
 * configuration xml
 * \param path chemin du fichier de configuration
 * \return <0 si erreur
 */
int init_sensors(const char* path)
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

/**
 * libère la mémoire dynamique de la variable global sensor[]
*/
int free_sensor_array()
{
	int loop_index;
	for(loop_index = 0 ; ((loop_index < MAX_NUM_OF_SENSOR) && (sensor[loop_index].name != NULL)) ; loop_index++){
		if(xmlFree(sensor[loop_index].name) != 0)
			perror("free_sensor_tab - xmlFree");		
	}
	return 0;
}

int main()
{
	sem_init(&stop_sem, 0, 0);
	int status;
	
	/* Handler d'arrêt */
	struct sigaction stop_action;
	stop_action.sa_handler=&stop_handler;
	sigemptyset(&stop_action.sa_mask);
	stop_action.sa_flags = 0;
	if((sigaction(SIGINT, &stop_action, NULL) < 0) ||
	   (sigaction(SIGTERM, &stop_action, NULL) < 0))
	{
		perror("Internal Error\n");
		return -1;
	}
	/* Chargement du fichier de configuration des capteurs */
	if ((status = init_sensors("config/sensors.xml")) < 0)
	{
		perror("Initialisation error\n");
		return status;
	}
	print_sensors();
	/* Chargement du fichier de configuration du moteur de règles */
	status = init_engine("config/rules.xml", sensor);
	if (status < 0)
	{
		printf("Rule loading error\n");
		return status;
	}
	
	/* Lancement du contrôle telnet */
	start_remote_control(1235);
	add_command("stop-server", "Stop the GHome server and cut the connection", &remote_stop_cmd);
	add_command("list-sensors", "Print the list of configured sensors", &list_sensors_cmd);
	add_command("list-rules", "Print the list of applied rules", &list_rules_cmd);

	/* Association du handler d'arrivée d'informations de capteurs */
	ios_attach_global_handler(process_data);

	/* Attente de commande d'arrêt */
	sem_wait(&stop_sem);

	/* Procedure d'arrêt */
	sem_destroy(&stop_sem);
	stop_remote_control();
	ios_release();
	printf("Stopping...\n");
	return 0;
}
