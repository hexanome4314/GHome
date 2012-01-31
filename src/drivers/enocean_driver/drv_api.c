/*********************************************************************************************************
*
* 	 _/_/_/ _/ _/_/_/ _/_/_/ _/ _/
* 	_/ _/ _/ _/ _/ _/ _/ _/ _/_/ _/_/_/ _/_/ _/_/
*      _/_/_/ _/ _/ _/ _/ _/_/ _/_/_/_/ _/ _/ _/ _/ _/ _/_/_/_/
*     _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/
*    _/ _/_/_/_/ _/_/_/ _/_/_/ _/ _/ _/_/ _/ _/ _/ _/_/_/
*
*
*  __   __   ___   _  _______   __   ___   _
* |  | |  | |   | | ||       | |  | |   | | |
* |  |_|  | |   |_| ||___    | |  | |   |_| |
* |       | |       | ___|   | |  | |       |
* |       | |    ___||___    | |  | |    ___|
* |   _   | |   |     ___|   | |  | |   |
* |__| |__| |___|    |_______| |__| |___|
*
* API implementee par les drivers et utilisee par le gestionnaire de drivers
**********************************************************************************************************/

#include "drv_api.h"
#include <stdio.h>
#include <stdlib.h>

#define NB_DEV_MAX 256
#define NB_DATA_BYTES 4

#define OK 0
#define ERROR -1


/* ---------- Types persos ---------- */
typedef struct
{
unsigned char* dataBytes; /* Donnees du capteur */
unsigned int idDev; /* id physique du capteur */
} devData;

/* ---------- Variable du pilote ---------- */
devData tabDevData[NB_DEV_MAX]; /* Tableau des donnees des capteurs installes */
int nbDev; /* Nombre de capteur installes */

/* ---------- Main pour les tests du pilote ---------- */
int test()
{
	unsigned char* mem = 0;
	int erreur = 0;
	
	drv_init(0,0);
	printf("Test OK pour init driver\n");
	
	
	erreur = drv_add_sensor(0x12, mem);
	
	if (erreur == 0)
	{
		printf("Test ajout d'un capteur\n");
	}
	else
	{
		printf("Erreur d'ajout de capteur !!!!\n");
	}
	
	printf("Id Capteur : %i\n", tabDevData[0].idDev);
	
	erreur = drv_remove_sensor(0x12);
	
	if (erreur > -1)
	{
		printf("Capteur retire !!!!\n");
	/*
		if (drv_rechercheCapteur(0x12) == -1)
		{
			printf("Capteur bien supprime !!!!\n");
		}
		else
		{
			printf("Retrait capteur ne marche pas !!!!\n");
		}
	*/
	}
	else
	{
		printf("Erreur de retrait de capteur !!!!\n");
	}
	
	getchar();
	
	return 0;
}

/* ---------- Methodes privees du pilote ---------- */

/**
Recherche la premiere place libre dans le tableau des donnees des capteurs
\return valeur de la place pour une place libre trouvee, -1 sinon.
*/
int recherchePlace()
{
	int i = 0;
	for (i = 0; i < NB_DEV_MAX; i++)
	{
		if (tabDevData[i].dataBytes == 0)
		{
			return i;
		}
	}
	
	return -1;
}

/**
Recherche la premiere place libre dans le tableau des donnees des capteurs
\param idCapteur
\return valeur de la place du capteur recherche, -1 si le capteur n'a pas été trouve.
*/
int drv_rechercheCapteur(unsigned int idCapteur)
{
	/* On parcourt tout le tableau des capteurs */
	int i = 0;
	for (i = 0; i < NB_DEV_MAX; i++)
	{
		// On prend un capteur qui est installe */
		if (tabDevData[i].dataBytes != 0)
		{
		/* On verifie si le capteur recherche est et on retourne la position trouve */
			if (tabDevData[i].idDev == idCapteur)
			{
				return i;
			}
		}
	}
	
	return -1;
}

/**
Libere la place du tableau des donnees des capteurs donnee en parametre
\param idCapteur : id du capteur a enlever (indice du tableau a enlever).
\return OK si la place a ete liberee, ERROR sinon (place non trouvee ou < 0 ou > NB_DEV_MAX.
*/
int libererPlace(unsigned int idCapteur)
{
	if ( (idCapteur < NB_DEV_MAX) && (idCapteur >= 0) )
	{
		tabDevData[idCapteur].dataBytes = 0;
		tabDevData[idCapteur].idDev = 0;
		
		return OK;
	}
	else
	{
		return ERROR;
	}
}

/**
Ajoute les donnees à un capteur deja installe dans tabDevData
\param idCapteur: id physique du capteur en hexadecimal
\return OK si la donnee a ete ajout, -1 si le capteur n'a pas été trouve.
*/
int drv_add_data(unsigned int idCapteur, unsigned char* dataBytes)
{
	int place = 0;
	place = drv_rechercheCapteur(idCapteur);
	if (place > -1)
	{

		return OK;
	}
	else
	{
		return ERROR;
	}
}
/* ---------- Methodes public du pilote ---------- */


/**
*/


int drv_init( const char* remote_addr, int remote_port )
{
	/* Initialisation des données des capteur */
	int i;
	for (i = 0; i < NB_DEV_MAX; i++)
	{
		tabDevData[i].dataBytes = 0;
		tabDevData[i].idDev = 0;
	}
	nbDev = 0;
	
	/* Lancer la connexion */
	/* Connexion pas bonne */
	/*
	if ()
	{
		return 1;
	}int place = 0;
place = drv_rechercheCapteur(id_sensor);
if (place < 0)
{
	return ERROR;
}
else
{
	libererPlace(place);
	nbDev--;
	
	return OK;
}
	*/
	return 0;
}


/**
Fonction appelée par le gestionnaire de drivers pour activer l'écoute (après l'initialisation)
\param mem_sem Semaphore protegeant les accès concurrents à la mémoire
\return 0 si tout est ok, > 0 si erreur
*/
int drv_run( sem_t mem_sem );

/**
Fonction appelée par le gestionnaire de drivers juste avant de décharger la librairie de la mémoire. L'écoute se stoppe et les ressources sont libérées
*/
void drv_stop( void );

/**
*/
int drv_add_sensor( unsigned int id_sensor, unsigned char* mem_ptr )
{
	int place = 0; /* Place où mettre le capteur */
	
	/* Recherche place libre dans le tableau des donnees des capteurs */
	place = recherchePlace();
	if (place < 0)
	{
		return ERROR;
	}
	else
	{
		tabDevData[place].idDev = id_sensor;
		tabDevData[place].dataBytes = mem_ptr;
		nbDev++;
		
		return place;
	}
}

/**
Fonction appelée par le gestionnaire de drivers pour supprimer un capteur en cours d'écoute.
\param id_sensor Identifiant unique du capteur qui ne doit plus être écouté
*/
int drv_remove_sensor( unsigned int id_sensor )
{
	int place = 0;
	place = drv_rechercheCapteur(id_sensor);
	if (place < 0)
	{
		return ERROR;
	}
	else
	{
		libererPlace(place);
		nbDev--;
		
		return OK;
	}
}

/**
Permet de demander des informations à un capteur
\param id_sensor Identifiant unique du capteur à interroger
id_trame Identifiant de la trame à envoyer
buffer Buffer qui va recevoir les données de retour
max_length Taille maximale du buffer
\return 0 si erreur, ou la taille des données lues
*/
int drv_fetch_data( unsigned int id_sensor, unsigned int id_trame, char* buffer, int max_length );


/**
Permet d'envoyer des données à un capteur (sans retour de sa part)
\param id_sensor Identifiant unique du capteur à contacter
id_trame Identifiant de la trame à envoyer
\return 0 si tout est ok, > 0 si erreur
*/
int drv_send_data( unsigned int id_sensor, unsigned int id_trame )
{
	unsigned int id = 0;
	char* idHexa = (char*)malloc(8);
	char trame [28];
	/* Fabrique les elements principaux de la tramme pour calculer le CheckSum */
	unsigned int byte1 = 0xA5;
	unsigned int byte2 = 0x5A;
	unsigned int hSEQ_LEN= 0x6B;
	unsigned int org = 0x05;
	unsigned int dataByte3 = 0;
	unsigned int dataByte2 = 0x00;
	unsigned int dataByte1 = 0x00;
	unsigned int dataByte0 = 0x00;
	unsigned int ID3 = (id_sensor & 0xFF000000);
	unsigned int ID2 = (id_sensor & 0x00FF0000);
	unsigned int ID1 = (id_sensor & 0x0000FF00);
	unsigned int ID0 = (id_sensor & 0x000000FF);
	unsigned int status = 0x30;
	unsigned int checkSum = 0;
	ID3 = ID3>>24;
	ID2 = ID2>>16;
	ID1 = ID1>>8;

	/* On verifie si on allume ou on eteint l'actionneur */
	if (id_trame == 0)
	{
		/* Pour activer l'actionneur. */
		dataByte3 = 0x50;
	}
	else
	{
		/* Pour désactiver l'actionneur */
		dataByte3 = 0x70;
	}

	checkSum = (hSEQ_LEN + org + dataByte3 + dataByte2 + dataByte1 + dataByte0 + ID3 + ID2 + ID1 + ID0 + status) % 256;

	printf("%X %X\nDataBytes : %X %X %X %X\nID : %X %X %X %X\nStatus : %X\nCheckSume : %X\n", hSEQ_LEN, org, dataByte3, dataByte2, dataByte1, dataByte0, ID3, ID2, ID1, ID0, status, checkSum);
	
	/* Fabrication de la tramme */
	trame[0] = 'A';
	trame[1] = '5';
	trame[2] = '5';
	trame[3] = 'A';
	trame[4] = '6';
	trame[5] = 'B';
	trame[6] = '0';
	trame[7] = '5';
	if (id_trame == 0)
	{
		/* Pour activer l'actionneur. */
		trame[8] = '5';
	}
	else
	{
		/* Pour désactiver l'actionneur */
		trame[8] = '7';
	}
	trame[9] = '0';
	trame[10] = '0';
	trame[11] = '0';
	trame[12] = '0'; 
	trame[13] = '0';
	trame[14] = '0';
	trame[15] = '0';
	id = ID3 & 0xF0;
	id = id >> 4;
	itoa(id, idHexa, 16);
	trame[16] = *idHexa;
	id = ID3 & 0x0F;
	itoa(id, idHexa, 16);
	trame[17] = *idHexa;
	id = ID2 & 0xF0;
	id = id >> 4;
	itoa(id, idHexa, 16);
	trame[18] = *idHexa;
	id = ID2 & 0x0F;
	itoa(id, idHexa, 16);
	trame[19] = *idHexa;
	id = ID1 & 0xF0;
	id = id >> 4;
	itoa(id, idHexa, 16);
	trame[20] = *idHexa;
	id = ID1 & 0x0F;
	itoa(id, idHexa, 16);
	trame[21] = *idHexa;
	id = ID0 & 0xF0;
	id = id >> 4;
	itoa(id, idHexa, 16);
	trame[22] = *idHexa;
	id = ID0 & 0x0F;
	itoa(id, idHexa, 16);
	trame[23] = *idHexa;
	trame[24] = '3';
	trame[25] = '0';
	id = checkSum & 0xF0;
	id = id >> 4;
	itoa(id, idHexa, 16);
	trame[26] = *idHexa;
	id = checkSum & 0x0F;
	itoa(id, idHexa, 16);
	trame[27] = *idHexa;
	free(idHexa);
	printf("\n");
	printf(trame);
	printf("\n");

	/* Envoyer la trame */
	/* Il faut voir comment qu'on fait pk il faut la socket ici ^^ */
	/* ----------------	*/

	/* Erreur d'envoie */
	if (0)
	{
		return 1;
	}
	
	return 0;
}

/* Test */
/*
int main()
{
	char* c = (char*)malloc(8);
	itoa(32, c, 16);
	printf(c);
	drv_send_data(0xFF9F1E04, 0);
	drv_send_data(0xFF9F1E04, 1);

	getchar();

}
*/

