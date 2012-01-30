#ifndef __IOS_API_H__
#define __IOS_API_H__

#include "drv_api.h"

/**
Initialise l'ios avant utilisation
\return IOS_OK si tout est ok
*/
int ios_init();

/**
Libère l'ios de la mémoire
*/
void ios_release();

/**
Charge un nouveau driver afin d'exploiter ses fonctionnalités
\param	driver_name	Le nom du fichier .so à charger
	ip_address	L'adresse IP du concentrateur/du capteur associé à cet id
	port		Le port sur lequel se connecter au périphérique distant
\return	Le majeur si tout est ok ou une erreur (IOS_UNKNOWN_DRIVER, IOS_INVALID_DRIVER, IOS_TOO_MANY_DRIVER_PLUGGED), ou IOS_INVALID_PORT si le port est douteux
*/
int ios_install_driver( const char* driver_name, const char* ip_address, unsigned int port );

/**
Désinstalle un driver chargé en mémoire en cloturant tous les fd si nécessaire
\param	major	Le majeur retourné lors de l'installation
*/
void ios_uninstall_driver( int major );

/**
Ajoute un nouveau périphérique rattaché à un driver  en lui associant un nom
\param	major		Le majeur du driver
	id		L'identifiant unique du périphérique
\return Le descripteur si tout est ok, IOS_ID_ALREADY_ADDED si le device existe déjà
*/
int ios_add_device( int major, int id );

/**
Enlève un périphérique déjà ajouté grâce à son nom
\param	fd	Le descripteur du périphérique
\return	IOS_OK si tout est ok, IOS_UNKNOWN_DEVICE si le périphérique est introuvable
*/
int ios_remove_device( int fd );

/**
Lit une donne en particulier provenant d'un périphérique
\param	fd		Descripteur de fichier
	drv_field	Données à retourner
	buffer		Buffer recevant les données
\return IOS_OK si tout est ok, IOS_INVALID_FIELD si le champ est invalide, IOS_UNKNOWN_FD si le descripteur est inconnu ou IOS_ERROR si l'erreur est critique
*/
int ios_read( int fd, int drv_field, char* buffer );

/**
Envoie des données au périphérique
\param	fd	Descripteur de fichier
	data	Les données à envoyer
\return IOS_OK si tout est ok, IOS_UNKNOWN_FD si le descripteur est inconnu, IOS_ERROR sinon
*/
int ios_write( int fd, char* data );

/**
Attache un handler au périphérique dès qu'une donnée est mise à jour (s'il y en a déjà un, il est détaché)
\param	fd	Descripteur de fichier
	handler	Foncteur sur la fonction à exécuter
*/
void ios_attach_handler( int fd, void (*handler)( int, char ) );

/**
Détache le handler du périphérique
\param	fd	Descripteur de fichier
*/
void ios_detach_handler( int fd );

#endif
