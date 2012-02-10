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


#endif // __CONNECTION_H__
