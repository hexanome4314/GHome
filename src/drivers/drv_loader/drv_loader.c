#include <stdio.h>
#include <string.h>
#include "drv_manager.h"
#include "drv_loader.h"
#include "drv_verbose.h"

/*
Chargement d'une librairie dynamique
*/
int drv_load( const char* filename, struct drv_func_ptr* func_ptr )
{
	char true_filename[256];
	sprintf( true_filename, "%s%s", DRV_PATH, filename );
	fprintf( drv_output, "%s::%s -> Load driver %s\n", __FILE__, __FUNCTION__, true_filename );
	
	/* Chargement de la librairie */
	func_ptr->handle = (void*) dlopen( true_filename, RTLD_LAZY );
	if( !func_ptr->handle )
	{
		return DRV_FILE_NOT_FOUND;
	}

	/* Sauvegarde le chemin du driver */
	strcpy( func_ptr->filename, filename );

	/* Chargement des symboles */
	*(void**) (&func_ptr->drv_init) = dlsym( func_ptr->handle, "drv_init" );
        *(void**) (&func_ptr->drv_add_device) = dlsym( func_ptr->handle, "drv_add_sensor" );
        *(void**) (&func_ptr->drv_remove_device) = dlsym( func_ptr->handle, "drv_remove_sensor" );
        *(void**) (&func_ptr->drv_run) = dlsym( func_ptr->handle, "drv_run" );
        *(void**) (&func_ptr->drv_stop) = dlsym( func_ptr->handle, "drv_stop" );
	*(void**) (&func_ptr->drv_get_info) = dlsym( func_ptr->handle, "drv_get_info" );
	*(void**) (&func_ptr->drv_send_data) = dlsym( func_ptr->handle, "drv_send_data" );

	/* Petite vérification pour voir que tous les symboles sont bien chargés */
        if( ! (func_ptr->drv_init && func_ptr->drv_add_device && func_ptr->drv_remove_device && func_ptr->drv_run && func_ptr->drv_stop && func_ptr->drv_get_info && func_ptr->drv_send_data) )
	{
		fprintf( drv_output, "%s::%s -> Missings functions in %s\n", __FILE__, __FUNCTION__, true_filename );
		return DRV_SYMBOL_NOT_FOUND;
	}

	return DRV_OK;
}


/*
	Décharge la librairie
*/
void drv_unload( struct drv_func_ptr* func_ptr )
{
	fprintf( drv_output, "%s::%s -> Unload %s\n", __FILE__, __FUNCTION__, func_ptr->filename );

	dlclose( func_ptr->handle );
}
