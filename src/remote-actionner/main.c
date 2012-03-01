#include <stdio.h>

#include "remote-actionner.h"

int lol( int i, const char* p ) {

	printf( "lol ! :: %s\n", p );

	return 1;
}

main()
{

	if( start_remote_actionner( 1100 ) == 0 )
	{
		add_actionner_command( "lol", lol );	
		sleep( 35 );
	}

	printf( "Closing....\n" );
	stop_remote_actionner();
}
