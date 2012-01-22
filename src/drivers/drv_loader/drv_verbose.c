#include "drv_verbose.h"

/**
Initialise le module
*/
void drv_verbose_init()
{
	drv_output = DRV_DEFAULT_OUTPUT;
}

/**
Libère le module
*/
void drv_verbose_release()
{
	close( drv_output );
}

/**
Défini la nouvelle sortie standard associée 
\param  fd      La nouvelle sortie standard
*/
void drv_set_verbose( FILE* fd )
{
	close( drv_output );
	
	drv_output = fd;
}

/**
Remet le mode verbeux à son paramétrage initial
*/
void drv_reset_verbose()
{
	close( drv_output );

	drv_output = DRV_DEFAULT_OUTPUT;
}

