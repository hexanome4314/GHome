#ifndef __DRV_LOADER_H__
#define __DRV_LOADER_H__

#include <dlfcn.h>
#include <unistd.h>

#define __DRVLOADER	/* Permet d'éviter la déclaration des fonctions et d'avoir des erreurs */
#include "../drv_api.h"

#include "drv_error.h"

/**
Structure contenant les pointeurs vers les fonctions de la librairie ainsi que le handle lui même de la librairie
*/
struct drv_func_ptr
{
	char filename[256];
	void* handle;
	int (*drv_init) (const char*, int);
	int (*drv_add_sensor) (unsigned int);
	int (*drv_remove_sensor) (unsigned int);
	int (*drv_run) (int);
	void (*drv_stop) ();
	void (*drv_get_info) (char*, int);
};

/**
Charge un driver, représenté par une librairie externe, en mémoire
\param	filename	Nom du fichier à charger
	drv_func	Structure des foncteurs
\return	0 Si tout est ok, > 0 sinon
*/
int  drv_load( const char* filename, struct drv_func_ptr* drv_func );

/**
Décharge un driver de la mémoire
\param	drv_func	Structure des foncteurs contenant le handle
*/
void drv_unload( struct drv_func_ptr* drv_func );

#endif
