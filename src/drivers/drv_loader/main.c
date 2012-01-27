#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "drv_manager.h"

int main()
{
	int msgq_id;
	unsigned int i;
	struct drv_func_ptr func;
	char** buf;
	size_t len;

	drv_manager_init();

	drv_set_verbose( stdout );

	drv_plug( "mon_driver/libdriver.so.1.0.1" );

	drv_list_plugged_drivers( &buf, &len );
	i = 0;
	while( i < len )
	{
		printf( "%d - %s\n", i, buf[i] );
		
		i++;
	}


	drv_manager_release();

	return 0;

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

	return 0;
}
