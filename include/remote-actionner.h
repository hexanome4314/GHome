#ifndef __REMOTE_ACT_H__
#define __REMOTE_ACT_H__

#define MAX_ACTIONNER_CONNECTIONS_ALLOWED	100

/**
Démarre l'actionneur distant
\param	port	Port sur lequel écouter
\return	RACT_OK si tout est ok
*/
int start_remote_actionner( unsigned int port );

/**
Arrête l'actionneur
*/
void stop_remote_actionner();

#ifndef __COMMAND_INTERPRET_H__

/**
Ajoute une nouvelle commande avec son handler
\param  command_name    Chaine par laquelle la commande doit commencer
        handler         Le handler associé à la commande
\return RACT_OK si tout est ok, RACT_CANNOT_ADD_COMMAND sinon
*/
int add_actionner_command( char* command_name, int (*handler)( int, const char* ) );

#endif // __COMMAND_INTERPRET_H__

#endif // __REMOTE_ACT_H__
