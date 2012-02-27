#include <stdio.h>

#include "remote-actionner.h"

main()
{
	if( start_remote_actionner( 1100 ) == 0 )
		sleep( 35 );

	printf( "Closing....\n" );
	stop_remote_actionner();
}
