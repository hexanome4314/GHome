#include <string.h>

#include "command_interp.h"
#include "remote_ctrl_error.h"
#include "console.h"

/**
Teste si la chaîne de caractère est une commande
\param cmd      La chaîne à tester
\return RCTRL_OK si l'on est en présence d'une commande, RCTRL_UNKNWON_COMMAND sinon
*/
int is_command( const char* cmd )
{
	if( strcmp( cmd, "quit" ) == 0 )
		return RCTRL_OK;

	return RCTRL_UNKNOWN_COMMAND;
}

/**
Interprète la commande passée en paramètre
\param  fd      Descripteur du VT du client
        cmd     La commande à interpreter
\return RCTRL_COMMAND_OK si tout est ok, RCTRL_COMMAND_LOGOUT si l'utilisateur doit être déconnecté après l'appel
*/
int command_interpret( int fd, const char* cmd )
{
	if( strcmp( cmd, "quit" ) == 0 )
	{
		print( fd, "Goodbye.\n" );
		return RCTRL_COMMAND_LOGOUT;
	}

	return RCTRL_COMMAND_OK;
}


