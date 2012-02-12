#ifndef __COMMAND_INTERPRET_H__
#define __COMMAND_INTERPRET_H__

/**
Teste si la chaîne de caractère est une commande
\param cmd	La chaîne à tester
\return 1 si l'on est en présence d'une commande, 0 sinon
*/
int is_command( const char* cmd );

/**
Interprète la commande passée en paramètre
\param	fd	Descripteur du VT du client
	cmd	La commande à interpreter
\return	RCTRL_COMMAND_OK si tout est ok, RCTRL_COMMAND_LOGOUT si l'utilisateur doit être déconnecté après l'appel
*/
int command_interpret( int fd, const char* cmd );

#endif
