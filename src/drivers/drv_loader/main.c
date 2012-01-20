#include <stdlib.h>
#include <stdio.h>

#include "drv_loader.h"

int main()
{
	int msgq_id;
	int i;
	struct drv_func_ptr func;

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
			msgrcv( msgq_id, (void*) &buffer, sizeof(struct msg_drv_notify), DRV_MSG_TYPE, 0 );

			printf( "[%d,%d,%d,%x]\n", buffer.id_sensor, buffer.flag_value, buffer.value, &buffer );
		}

	(*func.drv_stop)();

	drv_unload( &func );

	msgctl( msgq_id, IPC_RMID, NULL );

	return 0;
}
