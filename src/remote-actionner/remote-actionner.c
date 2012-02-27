#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>	
#include <string.h>

#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "remote-actionner.h"
#include "remote_act_error.h"
#include "command_interp.h"

#include <stdio.h>

/**
Thread qui permet d'accepter les nouvelles connexions
*/
static pthread_t accept_thread;

/**
Thread du client
*/
static pthread_t client_thread;

/**
Descripteur du socket serveur
*/
static int fd_sock;




/**
Petit hack qui permet de quitter le read()
*/
void client_alarm_handler( int sig ) { }

/**
Routine du thread qui écoute l'unique client
*/
void* client_callback( void* ptr )
{
	int current_sock;
	char buffer[255];
	int res;
	register int stop = 0;

	/* Hack qui permet de quitter le read */
	signal( SIGALRM, client_alarm_handler );

	/* Récupération du fd du sock via notre super hack ! */
	current_sock = (long) ptr;

	/* Le client doit envoyer la commande en premier (laquelle doit terminer par \r\n)*/
	res = read( current_sock, buffer, 255 );

	/* On récupère la commande (-2 pour enlever le \r\n) */
	buffer[res-2] = 0;

	/* On check que c'est bien une commande sinon on pète une erreur */
	if( is_command( buffer ) == RACT_UNKNOWN_COMMAND )
	{
		print( current_sock, "UNKNOWN_COMMAND\r\n" );
		close( current_sock );
		return NULL;
	}

	/* Et on interprete */
	res = command_interpret( current_sock, buffer );
	switch( res )
	{
		case RACT_COMMAND_OK :
			print( current_sock, "OK\r\n" );
			break;
		case RACT_COMMAND_PONG :
			print( current_sock, "PONG\r\n" );
			break;
		case RACT_COMMAND_ERROR :
		default :
			print( current_sock, "ERROR\r\n" );		
			break;
	}

	/* Déconnexion du client */
	close( current_sock );
	return NULL;
}

/**
Routine du thread qui accepte les nouvelles connexions
*/
void* accept_callback( void* ptr )
{
	long new_sock;
	int old_sock;
	int client_len;
	struct sockaddr_in client_addr;


	client_len = sizeof(client_addr);
	while( 1 )
	{

		/* En attente de connexion */
     		new_sock = accept( fd_sock, (struct sockaddr *) &client_addr, &client_len );	

		/* Si le socket server a été fermé, on quitte */
		if( new_sock < 0 )
		{
			pthread_exit( 0 );
		}

		/* Sinon, on lance un nouveau thread pour traiter la requête */
		/* (1 requête = 1 thread) */

		/* /!\ Attention, ceci est moche : passage de l'entier du sock en le faisant passer pour une adresse, les 2 sont des entiers et puis ça permet de simplifier le tout =) */
		pthread_create( &client_thread, NULL, client_callback, (void*) new_sock );
	}
}

/**
Démarre l'actionneur
\param  port    Port sur lequel écouter
\return RACT_OK si tout est ok
*/
int start_remote_actionner( unsigned int port )
{
	static struct sockaddr_in server_addr;
	int res;

	/* Ouverture d'un socket en mode flux */
	fd_sock = socket( AF_INET, SOCK_STREAM, 0 );
	if( fd_sock < 0 )
	{
		return RACT_CANNOT_OPEN_SOCKET;
	}

	/* Autorise les connexions sur le port passé en paramètre par n'importe qui */
	bzero( (char*) &server_addr, sizeof(server_addr) );

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons( port );

	if( bind( fd_sock, (struct sockaddr *) &server_addr, sizeof(server_addr) ) < 0) 
	{
              	return RACT_CANNOT_BIND_SOCKET;
	}

	/* Autorise un certain nombre de connexions */
	listen( fd_sock, MAX_REMOTE_CONNECTIONS_ALLOWED );

	/* Création des nouveaux threads */
	res = pthread_create( &accept_thread, NULL, accept_callback, NULL );

	if( res != 0 )
		return RACT_CANNOT_LAUNCH_ACCEPT_THREAD;

	/* Initialisation des commandes */
	init_command();

	/* Tout est ok */
	return RACT_OK;
}

/**
Arrête l'actionneur
*/
void stop_remote_actionner()
{
	/* Fermeture du socket d'écoute et du thread d'acceptation des connexions */
	close( fd_sock );

	pthread_cancel( accept_thread );

	release_command();
}
