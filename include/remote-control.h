#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#define MAX_REMOTE_CONNECTIONS_ALLOWED	1

/**
Démarre le mode télécommande
\param	port	Port sur lequel écouter
\return	0 si tout est ok
*/
int start_remote_control( unsigned int port );

/**
Arrête le mode télécommande
*/
void stop_remote_control();

#ifndef __COMMAND_INTERPRET_H__

/**
Ajoute une nouvelle commande avec son handler
\param  command_name    Chaine par laquelle la commande doit commencer
        command_desc    Chaine décrivant la commande
        handler         Le handler associé à la commande
\return RCTRL_OK si tout est ok, RCTRL_CANNOT_ADD_COMMAND sinon
*/
int add_command( char* command_name, char* command_desc, int (*handler)( int, const char* ) );

#endif // __COMMAND_INTERPRET_H__

#endif // __CONNECTION_H__
