#include <stdlib.h>
#include <string.h>

#include "command_interp.h"
#include "remote_ctrl_error.h"
#include "console.h"

/**
Liste des commandes (liste chaînée)
*/
static struct command_struct* command_list;
static struct command_struct* last_command;

/**
Handler pour la commande "quit"
*/
int quit_handler( int fd, const char* command )
{
	print( fd, "Goodbye.\n" );
	return RCTRL_COMMAND_LOGOUT;
}

/**
Handler pour la commande "help"
*/
int help_handler( int fd, const char* command )
{
	struct command_struct* current;

	print( fd, "Liste des commandes disponibles :\n" );

	current = command_list;
	while( current != NULL )
	{
		print( fd, "\t%s\t%s\n", current->command_name, current->command_desc );

		current = current->_next;
	}

	return RCTRL_OK;
}



/**
Initialise les commandes
*/
void init_command()
{
	/* Ajout de help et quit */
	struct command_struct* new_cmd;

	/* quit */
	new_cmd = (struct command_struct*) malloc( sizeof(struct command_struct) );
	
	strcpy( new_cmd->command_name, "help" );
	strcpy( new_cmd->command_desc, "Affiche cet aide." );
	new_cmd->handler = help_handler;

	command_list = new_cmd;

	/* help */
	new_cmd = (struct command_struct*) malloc( sizeof(struct command_struct) );
	
	strcpy( new_cmd->command_name, "quit" );
	strcpy( new_cmd->command_desc, "Quitte la console." );
	new_cmd->handler = quit_handler;
	new_cmd->_next   = NULL;
	
	command_list->_next = new_cmd;


	/* Garde en mémoire la dernière commande ajoutée */
	last_command = new_cmd;
}

/**
Libère les commandes de la mémoire
*/
void release_command()
{
	struct command_struct* current;

	current = command_list;
	while( current != NULL )
	{
		struct command_struct* buf;

		buf = current->_next;

		free( current );

		current = buf;
	}

	command_list = NULL;
	last_command = NULL;
}


/**
Ajoute une nouvelle commande avec son handler
\param  command_name    Chaine par laquelle la commande doit commencer
        command_desc    Chaine décrivant la commande
        handler         Le handler associé à la commande
\return RCTRL_OK si tout est ok, RCTRL_CANNOT_ADD_COMMAND sinon
*/
int add_command( char* command_name, char* command_desc, int (*handler)( int, const char* ) )
{
	struct command_struct* new_cmd;

	/* Allocation et création du maillon */
	new_cmd = (struct command_struct*) malloc( sizeof(struct command_struct) );

	strcpy( new_cmd->command_name, command_name );
	strcpy( new_cmd->command_desc, command_desc );
	new_cmd->handler = handler;
	new_cmd->_next   = NULL;

	/* Chainage */
	last_command->_next = new_cmd;
	/* Le dernier élément est le nouvel élément */
	last_command = new_cmd;
}


/**
Teste si la chaîne de caractère est une commande
\param cmd      La chaîne à tester
\return RCTRL_OK si l'on est en présence d'une commande, RCTRL_UNKNWON_COMMAND sinon
*/
int is_command( const char* cmd )
{
	struct command_struct* current;

	current = command_list;
	while( current != NULL )
	{
		if( strstr( cmd, current->command_name ) != NULL )
			return RCTRL_OK;

		current = current->_next;
	}

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
	struct command_struct* current;

        current = command_list;
        while( current != NULL )
        {
                if( strstr( cmd, current->command_name ) != NULL )
                        return (*current->handler)( fd, cmd );

		current = current->_next;
        }


	return RCTRL_COMMAND_OK;
}


