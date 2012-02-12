#include <stdio.h>

#include "connection.h"

main()
{
	if( start_remote_control( 1100 ) == 0 )
		sleep( 15 );

	printf( "Closing....\n" );
	stop_remote_control();
}
