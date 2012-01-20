#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

int (*drv_init) (const char*, int);
int (*drv_add_sensor) (unsigned int, unsigned char*);
int (*drv_remove_sensor) (unsigned int);
int (*drv_run) ( pthread_mutex_t );
void (*drv_stop) ();

int main()
{
	void* handle;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


	handle = (void*) dlopen( "./mon_driver/libdriver.so.1.0.1", RTLD_LAZY );
	if( !handle )
	{
		fprintf( stderr, "Unknown file !\n" );
		return 1;
	}

	*(void**) (&drv_init) = dlsym( handle, "drv_init" );
	*(void**) (&drv_add_sensor) = dlsym( handle, "drv_add_sensor" );
	*(void**) (&drv_remove_sensor) = dlsym( handle, "drv_remove_sensor" );
	*(void**) (&drv_run) = dlsym( handle, "drv_run" );
        *(void**) (&drv_stop) = dlsym( handle, "drv_stop" );

	if( drv_init && drv_add_sensor && drv_remove_sensor && drv_run && drv_stop )
	{
		unsigned char u;
		int i;

		(*drv_init)( "127.0.0.1", 1337 );
		(*drv_run)( mutex );

		(*drv_remove_sensor)( 7 );	
		(*drv_add_sensor)( 7, &u );

		i = 0;
		while( i < 10 )
		{
			i++;

			pthread_mutex_lock( &mutex );
			printf( "Value of sensor #7 is : %d\n", u );
			pthread_mutex_unlock( &mutex );
			sleep( 1 );
		}

		(*drv_stop)();
	}

	dlclose( handle );

	return 0;
}
