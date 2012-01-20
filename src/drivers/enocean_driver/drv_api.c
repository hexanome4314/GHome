/*********************************************************************************************************
*
* 	 _/_/_/ _/ _/_/_/ _/_/_/ _/ _/
* 	_/ _/ _/ _/ _/ _/ _/ _/ _/_/ _/_/_/ _/_/ _/_/
*      _/_/_/ _/ _/ _/ _/ _/_/ _/_/_/_/ _/ _/ _/ _/ _/ _/_/_/_/
*     _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/ _/
*    _/ _/_/_/_/ _/_/_/ _/_/_/ _/ _/ _/_/ _/ _/ _/ _/_/_/
*
*
* __ __ _ ___ _______ ____ _ ___
* | | | || | | || | | | | | | |
* | |_| || |_| ||___ | | | | |_| |
* | || | ___| | | | | |
* | ||___ ||___ | | | |___ |
* | _ | | | ___| | | | | |
* |__| |__| |___||_______| |___| |___|
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
	
		if (drv_rechercheCapteur(0x12) == -1)
		{
			printf("Capteur bien supprime !!!!\n");
		}
		else
		{
			printf("Retrait capteur ne marche pas !!!!\n");
		}
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
int drv_send_data( unsigned int id_sensor, unsigned int id_trame );
