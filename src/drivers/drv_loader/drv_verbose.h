#ifndef __DRV_VERBOSE_H__
#define __DRV_VERBOSE_H__

#include <stdio.h>
#include <fcntl.h>

#define DRV_DEFAULT_OUTPUT	fopen( "/dev/null", "w+" )

/**
Flux associé à la sortie "standard"
*/
FILE*	drv_output;

/**
Initialise le module
*/
void drv_verbose_init();

/**
Libère le module
*/
void drv_verbose_release();

/**
Défini la nouvelle sortie standard associée 
\param	fd	La nouvelle sortie standard
*/
void drv_set_verbose( FILE* fd );

/**
Remet le mode verbeux à son paramétrage initial
*/
void drv_reset_verbose();

#endif
