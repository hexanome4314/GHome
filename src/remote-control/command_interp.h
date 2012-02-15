#ifndef __COMMAND_INTERPRET_H__
#define __COMMAND_INTERPRET_H__

/**
Structure d'une commande enregistrée
*/
struct command_struct
{
	char	command_name[32];
	char	command_desc[128];
	int (*handler)( int, const char* );

	struct command_struct* _next;
};


/**
Initialise les commandes
*/
void init_command();

/**
Libère les commandes de la mémoire
*/
void release_command();

/**
Ajoute une nouvelle commande avec son handler
\param	command_name	Chaine par laquelle la commande doit commencer
	command_desc	Chaine décrivant la commande
	handler		Le handler associé à la commande
\return	RCTRL_OK si tout est ok, RCTRL_CANNOT_ADD_COMMAND sinon
*/
int add_command( char* command_name, char* command_desc, int (*handler)( int, const char* ) );

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
