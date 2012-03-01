#include <stdlib.h>
#include <string.h>

#include "command_interp.h"
#include "remote_act_error.h"
#include "console.h"

/**
Liste des commandes (liste chaînée)
*/
static struct command_struct* command_list;
static struct command_struct* last_command;

/**
Handler pour la commande "quit"
*/
int ping_handler( int fd, const char* command )
{
	return RACT_COMMAND_PONG;
}


/**
Initialise les commandes
*/
void act_init_command()
{
	/* Ajout de help et quit */
	struct command_struct* new_cmd;

	/* quit */
	new_cmd = (struct command_struct*) malloc( sizeof(struct command_struct) );
	
	strcpy( new_cmd->command_name, "PING" );
	new_cmd->handler = ping_handler;
	new_cmd->_next   = NULL;
	
        command_list = new_cmd;


	/* Garde en mémoire la dernière commande ajoutée */
	last_command = new_cmd;
}

/**
Libère les commandes de la mémoire
*/
void act_release_command()
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
        handler         Le handler associé à la commande
\return RACT_OK si tout est ok, RACT_CANNOT_ADD_COMMAND sinon
*/
int add_actionner_command( char* command_name, int (*handler)( int, const char* ) )
{
	struct command_struct* new_cmd;

	/* Allocation et création du maillon */
	new_cmd = (struct command_struct*) malloc( sizeof(struct command_struct) );

	strcpy( new_cmd->command_name, command_name );
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
\return RACT_OK si l'on est en présence d'une commande, RACT_UNKNWON_COMMAND sinon
*/
int act_is_command( const char* cmd )
{
	struct command_struct* current;

	current = command_list;
	while( current != NULL )
	{
		if( strstr( cmd, current->command_name ) != NULL )
			return RACT_OK;

		current = current->_next;
	}

	return RACT_UNKNOWN_COMMAND;
}

/**
Interprète la commande passée en paramètre
\param  fd      Descripteur du VT du client
        cmd     La commande à interpreter
\return RACT_COMMAND_OK si tout est ok, RACT_COMMAND_LOGOUT si l'utilisateur doit être déconnecté après l'appel
*/
int act_command_interpret( int fd, const char* cmd )
{
	struct command_struct* current;

        current = command_list;
        while( current != NULL )
        {
                if( strstr( cmd, current->command_name ) != NULL )
                        return (*current->handler)( fd, cmd );

		current = current->_next;
        }


	return RACT_COMMAND_OK;
}


