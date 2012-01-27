#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ios_api.h"
#include "drv_manager.h"

int main()
{
	int major;
	int fd1, fd2, fd3;

	char** buf;
	size_t len;

	ios_init();

	major = ios_install_driver( "libdriver.so.1.0.1", "127.0.0.1", 8080 );
	printf( "Result is : %d\n", major );

	fd1 = ios_add_device( major, 7 );
	printf( "Result is : %d\n", fd1 );

	fd2 = ios_add_device( major, 3 );
	printf( "Result is : %d\n", fd2 );

	fd3 = ios_add_device( major, 2 );

	sleep( 1 );

	ios_remove_device( fd1 );
	ios_remove_device( fd2 );

	ios_uninstall_driver( major );

	ios_release();

	return 0;

	/*drv_plug( "mon_driver/libdriver.so.1.0.1" );

	drv_list_plugged_drivers( &buf, &len );
	i = 0;
	while( i < len )
	{
		printf( "%d - %s\n", i, buf[i] );
		
		i++;
	}
	

	msgq_id = msgget(IPC_PRIVATE, 0600 | IPC_CREAT | IPC_EXCL );

	memset( &func, 0, sizeof(func) );
	drv_load( "mon_driver/libdriver.so.1.0.1", &func );

	(*func.drv_init)( "127.0.0.1", 1337 );
	(*func.drv_run)( msgq_id );

	(*func.drv_remove_sensor)( 7 );	
	(*func.drv_add_sensor)( 7 );

	i = 0;
	while( i < 100 )
	{
		struct msg_drv_notify buffer;

		i++;

		memset( &buffer, 0, sizeof(struct msg_drv_notify) );
		msgrcv( msgq_id, (void*) &buffer, sizeof(buffer) - sizeof(long), DRV_MSG_TYPE, 0 );

		printf( "#%d  [%d,%d,%d]\t[%d]\n", i, buffer.id_sensor, buffer.flag_value, buffer.value, msgq_id );
	}

	(*func.drv_stop)();

	drv_unload( &func );

	msgctl( msgq_id, IPC_RMID, NULL );
	*/

}
