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
static int msgq_id;

/**
Handler déclenché à chaque réception de message
*/
static void (*global_handler)( int, unsigned int, float );

/**
Handle du thread de la collecte de données
*/
static pthread_t collect_thread;

/**
Mutex sur les données pour éviter les accès concurrents ainsi que sur la condition d'arrêt
*/
static pthread_mutex_t data_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t stop_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t table_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t hdler_mutex = PTHREAD_MUTEX_INITIALIZER; 

/**
Condition d'arrêt de la collecte
*/
static int stop_collect;

struct ios_data_handler_value
{
	int fd;
	unsigned int flag_value;
	float value;
};

/**
Routine des threads d'évènements lorsqu'une donnée est modifiée
*/
void* ios_data_handler_evt( void* ptr )
{
	struct ios_data_handler_value* values = (struct ios_data_handler_value*) ptr;

	pthread_mutex_lock( &hdler_mutex );
	(*global_handler)( values->fd, values->flag_value, values->value );
	pthread_mutex_unlock( &hdler_mutex );
}

/**
Routine du thread collectant les données auprès des différents capteurs via la message queue
*/
void* ios_data_collector_callback( void* ptr )
{
	int stop, res;
	struct msg_drv_notify buffer;
	size_t buffer_len;

	fprintf( drv_output, "%s::%s -> Collector Launched.\n", __FILE__, __FUNCTION__ );

	/* Petit calcul de taille */
	buffer_len = sizeof(buffer) - sizeof(long);

	/* La boucle principale */
	stop = 0;
	while( stop == 0 )
	{
		/* Réception des données des périphériques sans blocage */
                memset( &buffer, 0, sizeof(struct msg_drv_notify) );
                res = msgrcv( msgq_id, (void*) &buffer, buffer_len, DRV_MSG_TYPE, IPC_NOWAIT );


		/* Si on a reçu un message, on raffraichit la table et on call le handler, sous condition que le flag est valide */
		if( buffer.flag_value >= 0 && buffer.flag_value < DRV_LAST_VALUE && res > 0 )
		{
			size_t i;
			int fd;
			void (*handler)( unsigned int, float );
			void (*g_handler)( int, unsigned int, float );

			/* On se met en jambe avec une petite recherche du fd pour l'id passé */
			fd = -1;

			pthread_mutex_lock( &table_mutex );
			for( i = 0; i < DEV_MAX_COUNT; i++ )
			{
				if( added_devices[i].id == buffer.id_sensor )
				{
					fd = i;
					handler = added_devices[i].handler;

					break;
				}
			}	
			pthread_mutex_unlock( &table_mutex );

			/* Si on a trouvé le périphérique, on procède */
			if( fd >= 0 )
			{
				pthread_mutex_lock( &data_mutex );
				device_data_matrix[fd][buffer.flag_value] = buffer.value;
				pthread_mutex_unlock( &data_mutex ); 

				pthread_mutex_lock( &hdler_mutex );
				g_handler = global_handler;
				pthread_mutex_unlock( &hdler_mutex );

				/* On call le handler */
				if( handler != NULL )
					(*handler)( buffer.flag_value, buffer.value );
				if( g_handler != NULL )
				{
					pthread_t new_thread;
					struct ios_data_handler_value* values;

					values = (struct ios_data_handler_value*) malloc( sizeof(struct ios_data_handler_value) );

					values->fd = fd;
					values->flag_value = buffer.flag_value;
					values->value = buffer.value;

					pthread_create( &new_thread, NULL, ios_data_handler_evt, values );
				}
			}
		}

		/* On regarde s'il faut s'arrêter */
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
		memset( device_data_matrix[i], 0x00, DRV_LAST_VALUE*sizeof(float) );

		memset( &added_devices[i], 0x00, sizeof(struct ios_device_descriptor) );
	} 

	added_devices_count = 0;

	/* On initialise le handler global */
	global_handler = NULL;

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

		pthread_mutex_lock( &table_mutex );
		added_devices[0].id	 = id;
		added_devices[0].major	 = major;
		added_devices[0].handler = NULL;
		pthread_mutex_unlock( &table_mutex );
	}
	else
	{
		size_t i;
		pthread_mutex_lock( &table_mutex );
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
		pthread_mutex_unlock( &table_mutex );
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

	/* On récupère les données pour éviter de prendre sans arrête le jeton */
	pthread_mutex_lock( &table_mutex );
	id    = added_devices[fd].id;
	major = added_devices[fd].major;
	pthread_mutex_unlock( &table_mutex );

	/* On commence par vérifier que le fd est valide */
	if( id == 0 )
		return IOS_UNKNOWN_DEVICE;

	/* On enlève le périphérique au sein du driver */
	(*drivers_func[major].drv_remove_device)( id );

	/* Puis de notre table */
	pthread_mutex_lock( &table_mutex );
	added_devices[fd].id      = 0;
	added_devices[fd].major   = 0;
	added_devices[fd].handler = 0;
	pthread_mutex_unlock( &table_mutex );

	/* Et enfin les données */
	pthread_mutex_lock( &data_mutex );
	memset( device_data_matrix[fd], 0x00, DRV_LAST_VALUE*sizeof(float) );
	pthread_mutex_unlock( &data_mutex );

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
		int current_major;

		pthread_mutex_lock( &table_mutex );
		current_major = added_devices[i].major;
		pthread_mutex_unlock( &table_mutex );

		if( current_major == major )
			ios_unregister_device( i );
	}

	fprintf( drv_output, "%s::%s -> Major : %d\n", __FILE__, __FUNCTION__, major );
}

/**
Attache un handler à ce descripteur qui sera appelé à chaque mise à jour des données
\param  fd      Le descripteur concerné
        handler Le handler qui sera appelé
*/
void ios_data_handler_attach( int fd, void (*handler)( unsigned int, float ) )
{
	int id;

	pthread_mutex_lock( &table_mutex );
	id = added_devices[fd].id;
	pthread_mutex_unlock( &table_mutex );

	/* Vérification que le fd est valide */
	if( id == 0 )
		return;

	/* Met en place le handler */
	pthread_mutex_lock( &table_mutex );
	added_devices[fd].handler = handler;
	pthread_mutex_unlock( &table_mutex );
}

/**
Détache un handler à ce descripteur
\param  fd      Le descripteur concerné
*/
void ios_data_handler_detach( int fd )
{
	pthread_mutex_lock( &table_mutex );
	added_devices[fd].handler = NULL;
	pthread_mutex_unlock( &table_mutex );
}

/**
Attache un handler activé dès qu'une donnée est mise à jour (s'il y en a déjà un, il est détaché)
\param  handler Foncteur sur la fonction à exécuter
*/
void ios_data_global_handler_attach( void (*handler)( int, unsigned int, float ) )
{
	pthread_mutex_lock( &hdler_mutex );
	global_handler = handler;
	pthread_mutex_unlock( &hdler_mutex );
}

/**
Détache le handler général
*/
void ios_data_global_handler_detach()
{
	pthread_mutex_lock( &hdler_mutex );
	global_handler = NULL;
	pthread_mutex_unlock( &hdler_mutex );
}

/**
Récupère une donnée de la matrice des données
\param  fd      Le descripteur concerné
        field   Le champ à récupérer
        buffer  L'adresse du buffer qui recevra la données
\return IOS_OK si tout est ok, IOS_UNKNOWN_DEVICE ou IOS_INVALID_FIELD sinon
*/
int ios_fetch_data( int fd, unsigned int field, float* buffer )
{
	int id;

	/* Vérification que le champ est valide car sinon... SEGFAULT ! */
	if( field >= DRV_LAST_VALUE )
		return IOS_INVALID_FIELD;

	/* Vérficiation que le descripteur est valide */
	pthread_mutex_lock( &table_mutex );
	id = added_devices[fd].id;
	pthread_mutex_unlock( &table_mutex );

	if( id == 0 )
		return IOS_UNKNOWN_DEVICE;

	/* On continue que si le buffer est valide */
	if( buffer != NULL )
	{
		pthread_mutex_lock( &data_mutex );
		*buffer = device_data_matrix[fd][field];
		pthread_mutex_unlock( &data_mutex );
	}

	return IOS_OK;
}

/**
Envoie une donnée à un périphérique
\param  fd      Le descripteur du périphérique concerné
        data    La donnée à envoyer
\return IOS_OK si tout est ok, IOS_UNKNOWN_DEVICE ou IOS_ERROR
*/
int ios_push_data( int fd, char data )
{
	int res;
	int major;
	int id;

	/* Vérification que le descripteur est valide */
	pthread_mutex_lock( &table_mutex );
	id = added_devices[fd].id;
	major = added_devices[fd].major;
	pthread_mutex_unlock( &table_mutex );

	if( id == 0 )
		return IOS_UNKNOWN_DEVICE;

	/* On s'embête pas et on appelle la fonction du driver ! */
	res = (*drivers_func[major].drv_send_data)( id, data );	
	if( res > 0 )
		return IOS_ERROR;

	return IOS_OK;
}
