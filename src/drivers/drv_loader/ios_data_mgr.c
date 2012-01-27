#include <pthread.h>
#include <string.h>

#include "ios_data_mgr.h"
#include "drv_verbose.h"

/**
Défini dans drv_manager.c
*/
extern struct drv_func_ptr drivers_func[DRV_MAX_COUNT];

/**
La boîte aux lettres utilisées pour les échanges entre les drivers et l'ios
*/
int msgq_id;

/**
Handle du thread de la collecte de données
*/
static pthread_t collect_thread;

/**
Mutex sur les données pour éviter les accès concurrents ainsi que sur la condition d'arrêt
*/
static pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t stop_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
Condition d'arrêt de la collecte
*/
static int stop_collect;


/**
Routine du thread collectant les données auprès des différents capteurs via la message queue
*/
void* ios_data_collector_callback( void* ptr )
{
	int stop = 0;
	fprintf( drv_output, "%s::%s -> Collector Launched.\n", __FILE__, __FUNCTION__ );
	while( stop == 0 )
	{
		usleep( 5 );

		pthread_mutex_lock( &stop_mutex );
		stop = stop_collect;
		pthread_mutex_unlock( &stop_mutex );
	}

	fprintf( drv_output, "%s::%s -> Collector exiting.\n", __FILE__, __FUNCTION__ );
	
}

/**
Initialise les données et la collecte
\return IOS_OK si tout est ok ou IOS_ERROR le cas échéant
*/
int ios_data_init()
{
	register int i;
	int res;

	/* On commence par initialiser la message queue */
	msgq_id = msgget( IPC_PRIVATE, 0600 | IPC_CREAT | IPC_EXCL );

	if( msgq_id == -1 )
		return IOS_ERROR;

	/* Initialise la matrice des données ainsi que la table de correspondance */
	for( i = 0; i < DRV_MAX_COUNT; i++ )
	{
		memset( device_data_matrix[i], 0x00, DRV_LAST_VALUE );

		memset( &added_devices[i], 0x00, sizeof(struct ios_device_descriptor) );
	} 

	added_devices_count = 0;

	/* Lance le thread de collecte des données */
	stop_collect = 0;
	res = pthread_create( &collect_thread, NULL, ios_data_collector_callback, NULL );


	return IOS_OK;
}

/**
Libère les données et la collecte
*/
void ios_data_release()
{
	/* Arrête la collecte */
	pthread_mutex_lock( &stop_mutex );
	stop_collect = 1;
	pthread_mutex_unlock( &stop_mutex );

	pthread_join( collect_thread, NULL );

	/* Libère la message queue */
	msgctl( msgq_id, IPC_RMID, NULL );	
}
		
/**
Lance la collecte des données au niveau du driver
\param major    Le majeur correspondant au driver
\return IOS_OK si tout est ok, IOS_INVALID_MAJOR ou IOS_ERROR sinon.
*/
int ios_data_begin_collect( int major )
{
	int err;

	/* On vérifie que le major est correct */
	if( drivers_func[major].handle == NULL )
		return IOS_INVALID_MAJOR;

	/* Puis on lance */
	err = (*drivers_func[major].drv_run)( msgq_id );
	if( err > 0 )
		return IOS_ERROR;

	return IOS_OK;
}

/**
Ajoute un nouveau périphérique rattaché à un driver dans la table
\param  major           Le majeur du driver
        id              L'identifiant unique du périphérique
\return Le descripteur si tout est ok, IOS_ID_ALREADY_ADDED si le device existe déjà ou encore IOS_INVALID_DEVICE_ID
*/
int ios_register_device( int major, int id )
{
	int fd;

	/* On vérifie que le major est correct */
	if( drivers_func[major].handle == NULL )
		return IOS_INVALID_MAJOR;

	/* Vérification que l'on peut accueillir un nouveau rejeton */
	if( added_devices_count + 1 >= DEV_MAX_COUNT )
		return IOS_TOO_MANY_DEVICES_PLUGGED;

	if( id <= 0 )
		return IOS_INVALID_DEVICE_ID;

	/* Si aucun périphérique n'a encore été ajouté, on assigne directement le 0 au lieu de faire le parcours */
	if( added_devices_count == 0 )
	{
		fd = 0;

		added_devices[0].id	 = id;
		added_devices[0].major	 = major;
		added_devices[0].handler = NULL;
	}
	else
	{
		size_t i;
		for( i = 0; i < DRV_MAX_COUNT; i++ )
		{
			if( added_devices[i].id == 0 )
			{
				added_devices[i].id	 = id;
				added_devices[i].major   = major;
				added_devices[i].handler = NULL;

				fd = i;

				break;
			}
		}
	}


	added_devices_count++;

	/* Il ne reste plus qu'à ajouter le périphérique au sein du driver */
	(*drivers_func[major].drv_add_device)( id );

	fprintf( drv_output, "%s::%s -> Add %d, and assign fd #%d : %d\n", __FILE__, __FUNCTION__, id, fd, major );

	return fd;
}

/**
Enlève un périphérique déjà ajouté grâce à son descripteur. Nettoie les données qui lui étaient associées.
\param  fd      Le descripteur du périphérique
\return IOS_OK si tout est ok, IOS_UNKNOWN_DEVICE si le périphérique est introuvable
*/
int ios_unregister_device( int fd )
{
	size_t i;
	int id, major;

	/* On commence par vérifier que le fd est valide */
	if( added_devices[fd].id == 0 )
		return IOS_UNKNOWN_DEVICE;

	id    = added_devices[fd].id;
	major = added_devices[fd].major;

	/* On enlève le périphérique au sein du driver */
	(*drivers_func[major].drv_remove_device)( id );

	/* Puis de notre table */
	added_devices[fd].id      = 0;
	added_devices[fd].major   = 0;
	added_devices[fd].handler = 0;

	/* Et enfin les données */
	for( i = 0; i < DEV_MAX_COUNT; i++ )
		memset( device_data_matrix[i], 0x00, DRV_LAST_VALUE );

	added_devices_count--;

	fprintf( drv_output, "%s::%s -> File desc : %d\n", __FILE__, __FUNCTION__, fd );

	return IOS_OK;
}

/**
Enlève les périphériques déjà ajoutés et qui sont associés au majeur passé en paramètre
\param  major   Majeur associé aux périphériques
*/
void ios_unregister_devices( int major )
{
	size_t i;

	/* Pas nécessaire s'il n'y a plus de périphériques */
	if( added_devices_count == 0 )
		return;

	/* On vérifie que le major est correct */
	if( drivers_func[major].handle == NULL )
		return;

	/* On enlève tous les périphériques du major concerné */
	for( i = 0; i < DEV_MAX_COUNT; i++ )
	{
		if( added_devices[i].major == major )
			ios_unregister_device( i );
	}

	fprintf( drv_output, "%s::%s -> Major : %d\n", __FILE__, __FUNCTION__, major );
}

/**
Attache un handler à ce descripteur qui sera appelé à chaque mise à jour des données
\param  fd      Le descripteur concerné
        handler Le handler qui sera appelé
*/
void ios_data_handler_attach( int fd, void (*handler)( int, char ) )
{
	/* Vérification que le fd est valide */
	if( added_devices[fd].id == 0 )
		return;

	/* Met en place le handler */
	added_devices[fd].handler = handler;
}

/**
Détache un handler à ce descripteur
\param  fd      Le descripteur concerné
*/
void ios_data_handler_detach( int fd )
{
	added_devices[fd].handler = NULL;
}

