#ifndef __IOS_API_H__
#define __IOS_API_H__

/**
Charge un nouveau driver afin d'exploiter ses fonctionnalités
\param	driver_name	Le nom du fichier .so à charger
\return	Le majeur si tout est ok ou une erreur < 0 sinon
*/
int ios_install_driver( const char* driver_name );

/**
Désinstalle un driver chargé en mémoire en cloturant tous les fd si nécessaire
\param	major	Le majeur retourné lors de l'installation
\return IOS_OK si tout est ok, ou IOS_UNKNOWN_DRIVER si le driver est inconnu
*/
int ios_uninstall_driver( int major );

/**
Ajoute un nouveau périphérique rattaché à un driver  en lui associant un nom
\param	major		Le majeur du driver
	id		L'identifiant unique du périphérique
	device_name	Le nom qui sera associé au périphérique
\return IOS_OK si tout est ok, IOS_ID_ALREADY_ADDED si le device existe déjà, ou IOS_NAME_ALREADY_ASSIGNED si le nom est déjà pris
*/
int ios_add_device( int major, int id, const char* device_name );

/**
Enlève un périphérique déjà ajouté grâce à son nom
\param	device_name	Le nom associé au périphérique
\return	IOS_OK si tout est ok, IOS_UNKNOWN_DEVICE si le périphérique est introuvable
*/
int ios_remove_device( const char* device_name );

/**
Ouvre un périphérique
\param	device_name	Le nom associé au périphérique
\return	Le descripteur de fichier du périphérique ou IOS_UNKNOWN_DEVICE si le périphérique est introuvable ou IOS_ERROR si l'erreur est critique
*/
int ios_open( const char* device_name );

/**
Ferme un périphérique
\param	fd	Le descripteur de fichier
\return IOS_OK si tout est ok, ou IOS_UNKNOWN_FD si le descripteur est inconnu
*/
int ios_close( int fd );

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
\return IOS_OK si tout est ok, IOS_UNKNOWN FD ou IOS_ERROR
*/
int ios_attach_handler( int fd, void (*handler)( int, char ) );

/**
Détache le handler du périphérique
\param	fd	Descripteur de fichier
*/
void ios_detach_handler( int fd );

#endif
