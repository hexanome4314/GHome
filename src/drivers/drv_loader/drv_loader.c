#include <string.h>
#include "drv_loader.h"

/*
Chargement d'une librairie dynamique
*/
int drv_load( const char* filename, struct drv_func_ptr* func_ptr )
{
	/* Chargement de la librairie */
	func_ptr->handle = (void*) dlopen( filename, RTLD_LAZY );
	if( !func_ptr->handle )
	{
		return DRV_FILE_NOT_FOUND;
	}

	/* Sauvegarde le chemin du driver */
	strcpy( func_ptr->filename, filename );

	/* Chargement des symboles */
	*(void**) (&func_ptr->drv_init) = dlsym( func_ptr->handle, "drv_init" );
        *(void**) (&func_ptr->drv_add_sensor) = dlsym( func_ptr->handle, "drv_add_sensor" );
        *(void**) (&func_ptr->drv_remove_sensor) = dlsym( func_ptr->handle, "drv_remove_sensor" );
        *(void**) (&func_ptr->drv_run) = dlsym( func_ptr->handle, "drv_run" );
        *(void**) (&func_ptr->drv_stop) = dlsym( func_ptr->handle, "drv_stop" );
	*(void**) (&func_ptr->drv_get_info) = dlsym( func_ptr->handle, "drv_get_info" );

	/* Petite vérification pour voir que tous les symboles sont bien chargés */
        if( ! (func_ptr->drv_init && func_ptr->drv_add_sensor && func_ptr->drv_remove_sensor && func_ptr->drv_run && func_ptr->drv_stop && func_ptr->drv_get_info) )
		return DRV_SYMBOL_NOT_FOUND;

	return DRV_OK;
}


/*
	Décharge la librairie
*/
void drv_unload( struct drv_func_ptr* func_ptr )
{
	dlclose( func_ptr->handle );
}
