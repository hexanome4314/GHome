#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "ios_api.h"
#include "engine.h"
#include "fields.h"
#include "remote-control.h"
#include "sensor-parse.h"
#include "core.h"

/* ------------------------------------------------------------------- GLOBALS */

static infos_drv drv[MAX_NUMBER_OF_DRIVERS];
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

/* -------------------------------------------- HANDLER TRANSMISSION DONNÉES CAPTEUR */
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
					fprintf(raw_data, ",\n   \"%s\": \"%.2f\"", Fields[f], value);
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
 * libère la mémoire dynamique de la variable global sensor[]
*/
int free_sensor_array()
{
	int loop_index;
	for(loop_index = 0 ; loop_index < MAX_NUMBER_OF_SENSORS ; loop_index++){
		xmlFree(sensor[loop_index].name);
	}
	return 0;
}

/**
 * libère la mémoire dynamique de la variable global drv[]
*/
int free_drv_array()
{
	int loop_index;
	for(loop_index = 0 ; loop_index < MAX_NUMBER_OF_DRIVERS ; loop_index++){
		xmlFree(drv[loop_index].name);
	}
	return 0;
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
	fprint_rules(fd);
	return 1;
}

/**
 * Commande telnet de rechargement des règles
 */
int reload_rules_cmd(int fd, const char* cmd)
{
	ios_detach_global_handler();
	if(reload_rules(sensor) < 0)
	{
		char message[] = "Malformed rule file. Correct it and reload rules again\n";
		write(fd, message, strlen(message));
	}
	else
	{
		fprint_rules(fd);
		ios_attach_global_handler(process_data);
	}
	return 1;
}

/**
 * Commande telnet de rechargement des informations de capteurs
 */
int reload_sensors_cmd(int fd, const char* cmd)
{
	ios_detach_global_handler();
	free_sensor_array();
	free_drv_array();
	if(read_sensors("config/sensors.xml", sensor, drv) < 0)
	{
		char message[] = "Malformed sensor description file. Correct it and reload sensor description file\n";
		write(fd, message, strlen(message));
	}
	else
	{
		fprint_sensors(fd);
		ios_attach_global_handler(process_data);
	}
	return 1;
}

/* ----------------------------------------------------------- FAT FONCTIONS :) */

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
	if ((status = read_sensors("config/sensors.xml", sensor, drv)) < 0)
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
	start_remote_control(1233);
	add_command("stop-server", "Stop the GHome server and cut the connection", &remote_stop_cmd);
	add_command("list-sensors", "Print the list of configured sensors", &list_sensors_cmd);
	add_command("list-rules", "Print the list of applied rules", &list_rules_cmd);
	add_command("reload-rules", "Reload the rules to apply", &reload_rules_cmd);
	add_command("reload-sensors", "Reload the configuration of the sensors", &reload_sensors_cmd);
	/* Association du handler d'arrivée d'informations de capteurs */
	ios_attach_global_handler(process_data);

	/* Attente de commande d'arrêt */
	sem_wait(&stop_sem);

	/* Procedure d'arrêt */
	free_sensor_array();
	free_drv_array();
	sem_destroy(&stop_sem);
	stop_remote_control();
	ios_release();
	printf("Stopping...\n");
	return 0;
}
