#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ios_api.h"
#include "drv_manager.h"

void h1( unsigned int field, float val )
{
	printf( "Handler called ! [%d,%f]\n", field, val );
}

int main()
{
	int major;
	int fd1, fd2, fd3;

	float   value1, value2, value3;
	size_t i;

	ios_init();

	major = ios_install_driver( "mon_driver.so.1.0.1", "127.0.0.1", 8080 );
	printf( "Result is : %d\n", major );

	if( major < 0 )
	{
		ios_release();
		return;
	}

	fd1 = ios_add_device( major, 7 );
	printf( "Result is : %d\n", fd1 );

	fd2 = ios_add_device( major, 3 );
	printf( "Result is : %d\n", fd2 );

	fd3 = ios_add_device( major, 2 );

	/* ios_attach_handler( fd1, h1 ); */

	for( i = 0; i < 5; i++ )
	{
		sleep( 2 );

		ios_read( fd1, DRV_FIELD_TEMPERATURE, &value1 );
		ios_read( fd2, DRV_FIELD_TEMPERATURE, &value2 );
		ios_read( fd3, DRV_FIELD_TEMPERATURE, &value3 );

		printf( "Temp1 : %f / Temp2 : %f / Temp3 : %f\n", value1, value2, value3 );

		ios_write( fd2, 10 );
	}

	ios_remove_device( fd1 );
	ios_remove_device( fd2 );

	sleep( 1 );

	ios_uninstall_driver( major );

	ios_release();

	return 0;
}
