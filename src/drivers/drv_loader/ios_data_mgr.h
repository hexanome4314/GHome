#ifndef __IOS_DATA_MGR_H__
#define __IOS_DATA_MGR_H__

#include "drv_manager.h"
#include "ios_error.h"

#define DEV_MAX_COUNT	32

/**
Permet la correspondance entre le descripeur de fichier et l'identifiant du capteur
*/
struct ios_device_descriptor
{
	int	id;	/* Identifiant unique du périphérique */
	int	major;  /* Majeur du driver dont dépend le périphérique */
	void (*handler)( unsigned int, float ); /* Handler à déclencher lorsque les données du périphérique sont modifiées */
} added_devices[DEV_MAX_COUNT];

/**
Compteur sur le nombre de périphériques ajoutés
*/
unsigned int added_devices_count;

/**
Représente la matrice des données. Chaque périphérique possède DRV_LAST_VALUE types de valeur possibles.
*/
float device_data_matrix[DEV_MAX_COUNT][DRV_LAST_VALUE];





/**
Initialise les données et la collecte
\return IOS_OK si tout est ok ou IOS_ERROR le cas échéant
*/
int ios_data_init();

/**
Libère les données et la collecte
*/
void ios_data_release();

/**
Lance la collecte des données au niveau du driver
\param major	Le majeur correspondant au driver
\return IOS_OK si tout est ok, IOS_ERROR ou IOS_INVALID_MAJOR sinon
*/
int ios_data_begin_collect( int major );

/**
Ajoute un nouveau périphérique rattaché à un driver dans la table
\param  major           Le majeur du driver
        id              L'identifiant unique du périphérique
\return Le descripteur si tout est ok, IOS_ID_ALREADY_ADDED si le device existe déjà, ou IOS_INVALID_PORT si le port est douteux
*/
int ios_register_device( int major, int id );

/**
Enlève un périphérique déjà ajouté grâce à son descripteur. Nettoie les données qui lui étaient associées.
\param  fd      Le descripteur du périphérique
\return IOS_OK si tout est ok, IOS_UNKNOWN_DEVICE si le périphérique est introuvable
*/
int ios_unregister_device( int fd );

/**
Enlève les périphériques déjà ajoutés et qui sont associés au majeur passé en paramètre
\param  major	Majeur associé aux périphériques
*/
void ios_unregister_devices( int major );

/**
Attache un handler à ce descripteur qui sera appelé à chaque mise à jour des données
\param	fd	Le descripteur concerné
	handler	Le handler qui sera appelé
*/
void ios_data_handler_attach( int fd, void (*handler)( unsigned int, float ) );

/**
Détache un handler à ce descripteur
\param	fd	Le descripteur concerné
*/
void ios_data_handler_detach( int fd );

/**
Récupère une donnée de la matrice des données
\param	fd	Le descripteur concerné
	field	Le champ à récupérer
	buffer	L'adresse du buffer qui recevra la données
\return IOS_OK si tout est ok, IOS_UNKNOWN_DEVICE ou IOS_INVALID_FIELD sinon
*/
int ios_fetch_data( int fd, unsigned int field, float* buffer );

/**
Envoie une donnée à un périphérique
\param	fd	Le descripteur du périphérique concerné
	data	La donnée à envoyer
\return IOS_OK si tout est ok, IOS_UNKNOWN_DEVICE ou IOS_ERROR
*/
int ios_push_data( int fd, char data );

#endif
