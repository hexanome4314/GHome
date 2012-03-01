#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include "drv_loader.h"
#include "drv_manager.h"
#include "drv_verbose.h"

/**
Tableau représentant tous les drivers chargés en mémoire
*/
struct drv_func_ptr drivers_func[DRV_MAX_COUNT];

/**
Compteur du nombre de drivers branchés
*/
static unsigned int plugged_driver_count;


/**
Fonction privée
Filtre permettant à scandir de ne récpuérer que les fichiers réguliers !
*/
int drv_filter( const struct dirent* current )
{
	return (current->d_type == DT_REG) ? 1 : 0;	
}

/*
Liste l'ensemble des drivers présents dans le répertoire DRV_PATH
*/
void drv_list_drivers( char*** buffer, size_t* max_size )
{
	struct dirent** filename_list;
	size_t tmp;

	/* Si le pointeur est nul, on évite de s'ennuyer et on le fait pointer sur un tampon */
	if( max_size == NULL )
		max_size = &tmp;

	/* Récupération de toutes les bonnes infos */
    	*max_size = scandir( DRV_PATH, &filename_list, drv_filter, alphasort);

	if( *max_size > 0 )
	{
		size_t i;
		
		/* On s'occupe de l'allocation mémoire */
		*buffer = (char**) malloc( *max_size * sizeof(char*) );

		/* On fait les pirouette pour remplir notre matrice tout en libérant la mémoire allouée par scandir ! :-) */
		i = 0;
        	while( i < *max_size )
		{
			(*buffer)[i] = (char*) malloc( strlen( filename_list[i]->d_name ) );
			strcpy( (*buffer)[i], filename_list[i]->d_name );

			free( filename_list[i] );

			i++;
		}

		free( filename_list );
	}
}

/**
Liste les drivers chargés en mémoire
\param  buffer          Adresse de la chaîne de caractères recevant les informations (doit $etre désallouée !)
        max_size        Nombre de chaînes dans le buffer
*/
void drv_list_plugged_drivers( char*** buffer, size_t* max_size )
{
	size_t i;

	*max_size = plugged_driver_count;

	if( plugged_driver_count == 0 )
		return;

	*buffer = (char**) malloc( plugged_driver_count * sizeof(char*) );
	
	for( i = 0; i < DRV_MAX_COUNT; i++ )
	{
		if( drivers_func[i].handle != NULL )
		{
			char info[256];
			(*drivers_func[i].drv_get_info)( info, 256 );

			(*buffer)[i] = (char*) malloc( strlen( info ) );
			strcpy( (*buffer)[i], info );
		}
	}
}

/**
Branche un nouveau driver et le charge en mémoire
\param  filename        Nom du driver à brancher
        ip_address      Adresse ip du concentrateur
        port            Port de connexion
\return Le master du driver ou < 0 si une erreur est survenue
*/
int drv_plug( const char* filename, const char* ip_address, unsigned int port )
{
	int res;
	size_t i;
	char buffer[256];

	/* Dans un premier temps, on récupère le premier master libre ! */
	i = 0;
	while( drivers_func[i].handle != NULL && i < DRV_MAX_COUNT ) i++;

	if( i == DRV_MAX_COUNT )
		return DRV_TOO_MANY_DRIVER_PLUGGED;

	/* On charge notre driver */
	res = drv_load( filename, &drivers_func[i] );

	if( res != DRV_OK )
		return res;

	/* On affice les infos pour la verbosité */	
	(*drivers_func[i].drv_get_info)( buffer, 256 );
	fprintf( drv_output, "New driver %s plugged. [%s]\n", filename, buffer );
		

	plugged_driver_count++;

	/* On initialise le driver */
	res = (*drivers_func[i].drv_init)( ip_address, port );

	if( res > 0 )
		return DRV_UNABLE_TO_INIT_DRIVER;

	return i;
}

/**
Débranche un driver qui était au préalable branché
\param  master  Le master du driver qui a été renvoyé par drv_plug
*/
void drv_unplug( int master )
{
	/* Vérification que le master existe */
	if( drivers_func[master].handle == NULL )
	{
		fprintf( drv_output, "No driver #%d.\n", master );
		return;
	}

	/* On demande au driver de s'arrêter */
	(*drivers_func[master].drv_stop)();

	/* On le décharge de la mémoire */
	drv_unload( &drivers_func[master] );
	drivers_func[master].handle = NULL;

	plugged_driver_count--;
}

/**
Initialise le gestionnaire
\return OK si tout est ok, < 0 si une erreur est survenue
*/
int drv_manager_init()
{
	size_t i;

	plugged_driver_count = 0;

	for( i = 0; i < DRV_MAX_COUNT; i++ )
		drivers_func[i].handle = NULL;


	drv_verbose_init();

	FILE* log_fd = fopen("./log/GHome_ios.log", "w+");
	drv_set_verbose(log_fd);

	return DRV_OK;
}

/**
Ferme proprement le gestionnaire
*/
void drv_manager_release()
{
	/* On ferme les drivers qui seraient toujours ouverts... */
	if( plugged_driver_count > 0 )
	{
		size_t i;
		for( i = 0; i < DRV_MAX_COUNT; i++ )
		{
			if( drivers_func[i].handle != NULL )
			{
				fprintf( drv_output, "Closing driver #%d... [%s]\n", (int) i, drivers_func[i].filename );
				drv_unplug( i );

				/* S'il n'en reste plus, autant se stopper là */
				if( plugged_driver_count == 0 )
					break;
			}
		}
	}

	drv_verbose_release();
}

