#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int (*drv_init) (const char*, int);

int main()
{
	void* handle;

	handle = (void*) dlopen( "./mon_driver/libdriver.so.1.0.1", RTLD_LAZY );
	if( !handle )
	{
		fprintf( stderr, "Unknown file !\n" );
		return 1;
	}

	*(void**) (&drv_init) = dlsym( handle, "drv_init" );
	if( drv_init )
		(*drv_init)( "127.0.0.1", 1337 );

	dlclose( handle );

	return 0;
}
