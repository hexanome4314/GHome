#ifndef __COMMAND_INTERPRET_H__
#define __COMMAND_INTERPRET_H__

/**
Structure d'une commande enregistrée
*/
struct command_struct
{
	char	command_name[32];
	int (*handler)( int, const char* );

	struct command_struct* _next;
};


/**
Initialise les commandes
*/
void act_init_command();

/**
Libère les commandes de la mémoire
*/
void act_release_command();

/**
Ajoute une nouvelle commande avec son handler
\param	command_name	Chaine par laquelle la commande doit commencer
	handler		Le handler associé à la commande (il doit retourner RACT_COMMAND_OK ou RACT_COMMAND_ERROR)
\return	RACT_OK si tout est ok, RACT_CANNOT_ADD_COMMAND sinon
*/
int add_actionner_command( char* command_name, int (*handler)( int, const char* ) );

/**
Teste si la chaîne de caractère est une commande
\param cmd	La chaîne à tester
\return 1 si l'on est en présence d'une commande, 0 sinon
*/
int act_is_command( const char* cmd );

/**
Interprète la commande passée en paramètre
\param	fd	Descripteur du VT du client
	cmd	La commande à interpreter
\return	RACT_COMMAND_OK si tout est ok, RACT_COMMAND_LOGOUT si l'utilisateur doit être déconnecté après l'appel
*/
int act_command_interpret( int fd, const char* cmd );

#endif
