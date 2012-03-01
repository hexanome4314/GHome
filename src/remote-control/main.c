#include <stdio.h>

#include "remote-control.h"

main()
{
	if( start_remote_control( 1100, "pass" ) == 0 )
		sleep( 25 );

	printf( "Closing....\n" );
	stop_remote_control();
}
