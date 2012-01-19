#include <stdio.h>
#include <stdlib.h>

#include "../../drv_api.h"

int drv_init( const char* addr, int port )
{
	printf( "Connexion à %s:%d\n", addr, port );

	return 0;
}

int drv_run( sem_t mem_sem )
{
	printf( "Run\n" );

	return 0;
}

void drv_stop( void )
{
	printf( "Stop\n" );
}

int drv_add_sensor( unsigned int id_sensor, unsigned char* mem_ptr )
{
	printf( "Adding sensor #%d, mapped at 0x%x\n", id_sensor, (int) mem_ptr );

	return 0;
}

void drv_remove_sensor( unsigned int id_sensor )
{
	printf( "Removing sensor #%d\n" );
}

int drv_fetch_data( unsigned int id_sensor, unsigned int id_trame, char* buffer, int max_length )
{
	return 0;
}


int drv_send_data( unsigned int id_sensor, unsigned int id_trame )
{
	return 0;
}
