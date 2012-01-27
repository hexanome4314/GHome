#ifndef __DRV_MANAGER_H__
#define __DRV_MANAGER_H__

#include "drv_loader.h"


#define DRV_PATH        "drivers/"
#define DRV_MAX_COUNT	8

/**
Liste les drivers pouvant être chargés
\param	buffer		Adresse de la chaîne de caractères recevant les informations (doit être désallouée !)
	max_size	Nombre de chaînes dans le buffer
*/
void drv_list_drivers( char*** buffer, size_t* max_size );

/**
Liste les drivers chargés en mémoire
\param 	buffer		Adresse de la chaîne de caractères recevant les informations (doit $etre désallouée !)
	max_size	Nombre de chaînes dans le buffer
*/
void drv_list_plugged_drivers( char*** buffer, size_t* max_size );

/**
Branche un nouveau driver et le charge en mémoire
\param	filename	Nom du driver à brancher
	ip_address	Adresse ip du concentrateur
	port		Port de connexion
\return	Le master du driver ou < 0 si une erreur est survenue
*/
int drv_plug( const char* filename, const char* ip_address, unsigned int port );

/**
Débranche un driver qui était au préalable branché
\param	master	Le master du driver qui a été renvoyé par drv_plug
*/
void drv_unplug( int master );

/**
Initialise le gestionnaire
\return DRV_OK si tout est ok, < 0 sinon
*/
int drv_manager_init();

/**
Ferme proprement le gestionnaire
*/
void drv_manager_release();

#endif
