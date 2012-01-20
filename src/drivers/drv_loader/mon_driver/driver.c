/***********************************
* Driver de test
* Pulse chaque seconde un nouveau message
* Author : Sébastien M.
***********************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../../drv_api.h"

#define MAX_SENSOR 32
#define SEED	   5

struct SENSOR_MAP
{
	unsigned int 	_id;
	unsigned char*	_ptr;
};

static pthread_t the_thread;
static pthread_mutex_t the_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t memory_mutex;
static short must_continue;

struct SENSOR_MAP added_sensors[MAX_SENSOR];
unsigned int sensor_count;

void add_sensor( unsigned int id, unsigned char* ptr )
{
	int i = 0;

	pthread_mutex_lock( &the_mutex );
	for( i = 0; i < MAX_SENSOR; i++ )
	{
		if( added_sensors[i]._id == 0 )
		{
			added_sensors[i]._id = id;
			added_sensors[i]._ptr = ptr;
			sensor_count++;

			break;
		}
	}
	pthread_mutex_unlock( &the_mutex );

	if( i == MAX_SENSOR )
		printf( "Cannot add new sensor : limit reached.\n" );
	else
		printf( "\tAdded at #%d\n", i );
}

unsigned int sensor_exists( unsigned int id )
{
	int i;

	pthread_mutex_lock( &the_mutex );
        for( i = 0; i < MAX_SENSOR; i++ )
        {
                if( added_sensors[i]._id == id )
                        break;
        }
        pthread_mutex_unlock( &the_mutex );

	return i;
}

void remove_sensor( unsigned int id )
{
	int res = sensor_exists( id );

	if( res >= 0 && res < MAX_SENSOR )
	{
		pthread_mutex_lock( &the_mutex );
		added_sensors[res]._id = 0;
		added_sensors[res]._ptr = 0;
		sensor_count--;
	        pthread_mutex_unlock( &the_mutex );
	}
	else
		printf( "No sensor found.\n" );
}


/**
* La callback qui pulse
*/
void* callback( void* ptr )
{
	int buffer;

	srand( SEED );

	pthread_mutex_lock( &the_mutex );
	must_continue = 1;
	buffer = 1;
	pthread_mutex_unlock( &the_mutex );

	while( buffer == 1 )
	{
		int index;
		int id = (rand() % MAX_SENSOR);

		if( id == 0 ) continue;

		/* On verifie que l'id est bien dans la liste des capteurs surveillés */
		index = sensor_exists( id );

		if( index >= 0 && index < MAX_SENSOR )
		{
			unsigned char* ptr_addr;

			pthread_mutex_lock( &the_mutex );
			ptr_addr = added_sensors[index]._ptr;
			pthread_mutex_unlock( &the_mutex );

			pthread_mutex_lock( &memory_mutex );
			*ptr_addr = 100 + rand()%5;
			pthread_mutex_unlock( &memory_mutex );
		}

		pthread_mutex_lock( &the_mutex );
		buffer = must_continue;
		pthread_mutex_unlock( &the_mutex );
	}
}

/**
* Initialisation, ne fait quasiment rien
*/
int drv_init( const char* addr, int port )
{
	int i;

	printf( "Connexion à %s:%d\n", addr, port );

	/* Initialise la liste des capteurs ajoutés */
	for( i = 0; i < MAX_SENSOR; i++ )
	{
		added_sensors[i]._id  = 0;
		added_sensors[i]._ptr = 0;
	}

	sensor_count = 0;

	return 0;
}

int drv_run( pthread_mutex_t mem_mtx )
{
	int ret;

	memory_mutex = mem_mtx;
	ret = pthread_create( &the_thread, NULL, callback, NULL );

	return 0;
}

void drv_stop( void )
{
	printf( "Wait\n" );

	pthread_mutex_lock( &the_mutex );
	must_continue = 0;
	pthread_mutex_unlock( &the_mutex );

	pthread_join(the_thread, NULL);

	printf( "Stop\n" );
}

int drv_add_sensor( unsigned int id_sensor, unsigned char* mem_ptr )
{
	printf( "Adding sensor #%d, mapped at 0x%x\n", id_sensor, (unsigned int) mem_ptr );

	add_sensor( id_sensor, mem_ptr );

	return 0;
}

void drv_remove_sensor( unsigned int id_sensor )
{
	printf( "Removing sensor #%d\n", id_sensor );

	remove_sensor( id_sensor );
}

int drv_fetch_data( unsigned int id_sensor, unsigned int id_trame, char* buffer, int max_length )
{
	return 0;
}


int drv_send_data( unsigned int id_sensor, unsigned int id_trame )
{
	return 0;
}
