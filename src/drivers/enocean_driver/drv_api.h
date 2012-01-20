/*********************************************************************************************************
*                                                                                             
*     _/_/_/    _/        _/_/_/          _/_/_/  _/    _/                                     
*    _/    _/  _/        _/    _/      _/        _/    _/    _/_/    _/_/_/  _/_/      _/_/    
*   _/_/_/    _/        _/    _/      _/  _/_/  _/_/_/_/  _/    _/  _/    _/    _/  _/_/_/_/   
*  _/        _/        _/    _/      _/    _/  _/    _/  _/    _/  _/    _/    _/  _/          
* _/        _/_/_/_/  _/_/_/          _/_/_/  _/    _/    _/_/    _/    _/    _/    _/_/_/     
*                                                                                              
*                                                                                              
*			  __   __  _   ___  _______  ____   _   ___ 
*			|  | |  || | |   ||       ||    | | | |   |
*			|  |_|  || |_|   ||___    | |   | | |_|   |
*			|       ||       | ___|   | |   | |       |
*			|       ||___    ||___    | |   | |___    |
*			|   _   |    |   | ___|   | |   |     |   |
*			|__| |__|    |___||_______| |___|     |___|
*
* API implémentée par les drivers et utilisée par le gestionnaire de drivers
**********************************************************************************************************/

#ifndef __DRV_API_H__
#define __DRV_API_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/*
Définition des valeurs admissibles par l'IOS
*/
#define DRV_FIELD_BUTTON1	0
#define DRV_FIELD_BUTTON2	1
#define DRV_FIELD_BUTTON3	2
#define DRV_FIELD_BUTTON4	3
#define DRV_FIELD_BUTTON5	4
#define DRV_FIELD_BUTTON6	5
#define DRV_FIELD_BUTTON7	6
#define DRV_FIELD_BUTTON8	7
#define DRV_FIELD_TEMPERATURE	8
#define DRV_FIELD_HUMIDITY	9
#define DRV_FIELD_LIGHTING	10
#define DRV_FIELD_VOLTAGE	11
#define DRV_LAST_VALUE		12

/*
Types des messages de la boîte aux lettres
*/
#define DRV_MSG_TYPE		0xAA

/**
Structure des messages contenus dans la boîte aux lettres
*/
struct msg_drv_notify
{
	long msg_type;
	unsigned int id_sensor;
	unsigned int flag_value;
	char value;
};

#ifndef __DRVLOADER

/**
Fonction appelée juste après le chargement de la librairie en mémoire pour initialiser le capteur en spécifiant les données de connexion.
\param:	remote_addr	Adresse IP ou DNS de l'hôte contacter
	remote_port	Port de l'hôte à contacter
\return 0 si tout est ok, > 0 si erreur
*/

int drv_init( const char* remote_addr, int remote_port );


/**
Fonction appelée par le gestionnaire de drivers pour activer l'écoute (après l'initialisation)
\param	msgq_id		Identifiant de la boîte aux lettres à utiliser pour notifier le manager du changement de valeur
\return 0 si tout est ok, > 0 si erreur
*/
int drv_run( int msgq_id );

/**
Fonction appelée par le gestionnaire de drivers juste avant de décharger la librairie de la mémoire. L'écoute se stoppe et les ressources sont libérées
*/
void drv_stop( void );

/**
Fonction appelée par le gestionnaire de drivers pour ajouter un capteur à écouter. Peut survenir n'importe quand.
\param	id_sensor	Identifiant unique du capteur qui doit être écouté
\return 0 si tout est ok, > 0 si erreur
*/
int drv_add_sensor( unsigned int id_sensor );

/**
Fonction appelée par le gestionnaire de drivers pour supprimer un capteur en cours d'écoute.
\param	id_sensor	Identifiant unique du capteur qui ne doit plus être écouté
*/
int drv_remove_sensor( unsigned int id_sensor );

/**
Permet de demander des informations à un capteur
\param	id_sensor	Identifiant unique du capteur à interroger
	id_trame	Identifiant de la trame à envoyer
	buffer		Buffer qui va recevoir les données de retour
	max_length	Taille maximale du buffer
\return 0 si erreur, ou la taille des données lues
*/
int drv_fetch_data( unsigned int id_sensor, unsigned int id_trame, char* buffer, int max_length );


/**
Permet d'envoyer des données à un capteur (sans retour de sa part)
\param	id_sensor	Identifiant unique du capteur à contacter
	id_trame	Identifiant de la trame à envoyer
\return 0 si tout est ok, > 0 si erreur
*/
int drv_send_data( unsigned int id_sensor, unsigned int id_trame );

/**
Retourne les informations concernant le driver (nom, version, ...)
\param  buffer		Chaîne de caractère qui reçoit les données
	max_length	Taille du buffer max
*/
void drv_get_info( char* buffer, int max_length );

#endif // __DRVLOADER
#endif
