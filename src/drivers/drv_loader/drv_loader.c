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
		return FILE_NOT_FOUND;
	}

	/* Chargement des symboles */
	*(void**) (&func_ptr->drv_init) = dlsym( func_ptr->handle, "drv_init" );
        *(void**) (&func_ptr->drv_add_sensor) = dlsym( func_ptr->handle, "drv_add_sensor" );
        *(void**) (&func_ptr->drv_remove_sensor) = dlsym( func_ptr->handle, "drv_remove_sensor" );
        *(void**) (&func_ptr->drv_run) = dlsym( func_ptr->handle, "drv_run" );
        *(void**) (&func_ptr->drv_stop) = dlsym( func_ptr->handle, "drv_stop" );
	*(void**) (&func_ptr->drv_get_info) = dlsym( func_ptr->handle, "drv_get_info" );

	/* Petite vérification pour voir que tous les symboles sont bien chargés */
        if( ! (func_ptr->drv_init && func_ptr->drv_add_sensor && func_ptr->drv_remove_sensor && func_ptr->drv_run && func_ptr->drv_stop && func_ptr->drv_get_info) )
		return SYMBOL_NOT_FOUND;

	return OK;
}


/*
	Décharge la librairie
*/
void drv_unload( struct drv_func_ptr* func_ptr )
{
	dlclose( func_ptr->handle );
}
