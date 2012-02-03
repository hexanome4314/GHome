#include <stdio.h>
#include "ios_api.h"
#include "ios_data_mgr.h"
#include "ios_error.h"
#include "drv_manager.h"

#include "drv_verbose.h"


/**
Initialise l'ios avant utilisation
\return IOS_OK si tout est ok
*/
int ios_init()
{
	int err;

	drv_manager_init();
	drv_set_verbose( stdout );

	err = ios_data_init();

	if( err == IOS_ERROR )
		return IOS_ERROR;

	return IOS_OK;	
}

/**
Libère l'ios de la mémoire
*/
void ios_release()
{
	ios_data_release();

	drv_manager_release();
}


/**
Charge un nouveau driver afin d'exploiter ses fonctionnalités
\param  driver_name     Le nom du fichier .so à charger
        ip_address      L'adresse IP du concentrateur/du capteur associé à cet id
        port            Le port sur lequel se connecter au périphérique distant
\return Le majeur si tout est ok ou une erreur < 0 sinon
*/
int ios_install_driver( const char* driver_name, const char* ip_address, unsigned int port )
{
	register int err;
	int major;

	/* Vérification que le port est valide */
	if( port > 65535 )
		return IOS_INVALID_PORT;

	/* On plug ! */
	err = drv_plug( driver_name, ip_address, port );

	if( err < 0 )
	{
		switch( err )
		{
			case DRV_TOO_MANY_DRIVER_PLUGGED:
				return IOS_TOO_MANY_DRIVER_PLUGGED;
			case DRV_SYMBOL_NOT_FOUND:
				return IOS_INVALID_DRIVER;
			case DRV_FILE_NOT_FOUND:
				return IOS_UNKNOWN_DRIVER;		
			case DRV_UNABLE_TO_INIT_DRIVER:
				return IOS_UNABLE_TO_INIT_DRIVER;
			default:
				return IOS_ERROR;
		}
	}

	major = err;

	/* On lance la routine du driver */
	err = ios_data_begin_collect( major );

	/* Si une erreur survient, on désinstalle le driver et on retourne l'erreur */
	if( err != IOS_OK )
	{
		ios_uninstall_driver( major );

		return err; /* IOS_INVALID_HANDLE / IOS_ERROR */
	}

	return major;
}

/**
Désinstalle un driver chargé en mémoire en cloturant tous les fd si nécessaire
\param  major   Le majeur retourné lors de l'installation
*/
void ios_uninstall_driver( int major )
{

	/* Fermeture des descripteurs liés à ce driver */
	ios_unregister_devices( major );

	/* Ferme le driver */
	drv_unplug( major );
}

/**
Ajoute un nouveau périphérique rattaché à un driver  en lui associant un nom
\param  major           Le majeur du driver
        id              L'identifiant unique du périphérique
\return Le descripteur si tout est ok, IOS_ID_ALREADY_ADDED si le device existe déjà
*/
int ios_add_device( int major, int id )
{
	return ios_register_device( major, id );
}

/**
Enlève un périphérique déjà ajouté grâce à son nom
\param  fd      Le descripteur du périphérique
\return IOS_OK si tout est ok, IOS_UNKNOWN_DEVICE si le périphérique est introuvable
*/
int ios_remove_device( int fd )
{
	return ios_unregister_device( fd );
}

/**
Attache un handler au périphérique dès qu'une donnée est mise à jour (s'il y en a déjà un, il est détaché)
\param  fd      Descripteur de fichier
        handler Foncteur sur la fonction à exécuter
*/
void ios_attach_handler( int fd, void (*handler)( unsigned int, float ) )
{
	ios_data_handler_attach( fd, handler );
}

/**
Détache le handler du périphérique
\param  fd      Descripteur de fichier
*/
void ios_detach_handler( int fd )
{
	ios_data_handler_detach( fd );
}

/**
Lit une donne en particulier provenant d'un périphérique
\param  fd              Descripteur de fichier
        drv_field       Données à retourner
        buffer          Buffer recevant les données
\return IOS_OK si tout est ok, IOS_INVALID_FIELD si le champ est invalide, IOS_UNKNOWN_DEVICE si le descripteur est inconnu ou IOS_ERROR si l'erreur est critique
*/
int ios_read( int fd, unsigned int drv_field, float* buffer )
{
	return ios_fetch_data( fd, drv_field, buffer );
}

/**
Envoie des données au périphérique
\param  fd      Descripteur de fichier
        data    Les données à envoyer
\return IOS_OK si tout est ok, IOS_UNKNOWN_FD si le descripteur est inconnu, IOS_ERROR sinon
*/
int ios_write( int fd, char data )
{
	return ios_push_data( fd, data );
}

