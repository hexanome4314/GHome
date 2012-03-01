#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>	
#include <string.h>

#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "remote-control.h"
#include "remote_ctrl_error.h"
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
Mutex sur le descripteur de la socket cliente
*/
static pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
Descripteur de la socket cliente en cours
*/
static int fd_client_sock;
static int current_user_logged;

/**
Descripteur du socket serveur
*/
static int fd_sock;

/**
Mot de passe permettant d'accèder à la console
*/
static char telnet_password_access[255];




/**
Petit hack qui permet de quitter le read()
*/
void client_alarm_handler( int sig )
{
}

/**
Affiche le MOTD à chaque nouvelle connexion
*/
void print_motd( int fd )
{
	print( fd, 
		"***************************************************************************\n"
		"      ___           ___           ___           ___           ___     \n"
		"     /\\  \\         /\\__\\         /\\  \\         /\\__\\         /\\  \\    \n"
		"    /::\\  \\       /:/  /        /::\\  \\       /::|  |       /::\\  \\   \n"
		"   /:/\\:\\  \\     /:/__/        /:/\\:\\  \\     /:|:|  |      /:/\\:\\  \\  \n"
		"  /:/  \\:\\  \\   /::\\  \\ ___   /:/  \\:\\  \\   /:/|:|__|__   /::\\~\\:\\  \\ \n"
		" /:/__/_\\:\\__\\ /:/\\:\\  /\\__\\ /:/__/ \\:\\__\\ /:/ |::::\\__\\ /:/\\:\\ \\:\\__\\\n"
		" \\:\\  /\\ \\/__/ \\/__\\:\\/:/  / \\:\\  \\ /:/  / \\/__/~~/:/  / \\:\\~\\:\\ \\/__/\n"
		"  \\:\\ \\:\\__\\        \\::/  /   \\:\\  /:/  /        /:/  /   \\:\\ \\:\\__\\  \n"
		"   \\:\\/:/  /        /:/  /     \\:\\/:/  /        /:/  /     \\:\\ \\/__/  \n"
		"    \\::/  /        /:/  /       \\::/  /        /:/  /       \\:\\__\\    \n"
		"     \\/__/         \\/__/         \\/__/         \\/__/         \\/__/    \n"
		"***************************************************************************\n"
	);
}

/**
Routine du thread qui écoute l'unique client
*/
void* client_callback( void* ptr )
{
	int current_sock;
	int new_sock;
	int is_logged;
	char buffer[255];
	int res;
	register int stop = 0;

	/* Hack qui permet de quitter le read */
	signal( SIGALRM, client_alarm_handler );

	/* Récupération du fd courant pour afficher le motd */
	pthread_mutex_lock( &client_mutex );
	current_sock = fd_client_sock;
	pthread_mutex_unlock( &client_mutex );

	print_motd( current_sock );
	
	/* Tant que l'utilisateur ne demande pas de quitter */
	while( stop == 0 )
	{
		size_t i;

		/* Actualisation du fd courant */
		pthread_mutex_lock( &client_mutex );
		current_sock = fd_client_sock;
		is_logged = current_user_logged;
		pthread_mutex_unlock( &client_mutex );

		/* L'utilisateur n'est pas authentifié */
		if( is_logged == 0 ) 
		{
			print( current_sock, "Enter password: " );
		}
		/* Affichage du prompt et on attend une entrée de l'utilisateur */
		else
		{
			print( current_sock, "$> " );
		}

		res = read( current_sock, buffer, 255 );

		/* On vérifie que le fd n'a pas changé, sinon c'est qu'on a un nouveau client */
		pthread_mutex_lock( &client_mutex );
		new_sock = fd_client_sock;
		is_logged = current_user_logged;
		pthread_mutex_unlock( &client_mutex );

		if( new_sock != current_sock )
		{
			/* On affiche donc le motd au nouveau client */
			print_motd( new_sock );
			continue;
		}

		/* Sinon on récupère la commande (-2 pour enlever le \r\n) */
		buffer[res-2] = 0;

		/* L'utilisateur n'est pas authentifié */
		if( is_logged == 0 )
		{
			if( strcmp( buffer, telnet_password_access ) == 0 )
			{
				pthread_mutex_lock( &client_mutex );
				current_user_logged = 1;
				pthread_mutex_unlock( &client_mutex );

				print( current_sock, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );
				print_motd( current_sock );
			}
			else
			{
				print( current_sock, "\aYou failed.\n" );
				stop = 1;
			}
			
		}
		else
		{
			/* On check que c'est bien une commande sinon on pète une erreur */
			if( is_command( buffer ) == RCTRL_UNKNOWN_COMMAND )
			{
				print( current_sock, "Unrecognized command : \"%s\".\n", buffer );
				continue;
			}

			/* Et on interprete */
			res = command_interpret( current_sock, buffer );
			if( res == RCTRL_COMMAND_LOGOUT )
				stop = 1;
		}
	}

	/* Déconnexion du client */
	pthread_mutex_lock( &client_mutex );
	close( fd_client_sock );
	fd_client_sock = -1;
	pthread_mutex_unlock( &client_mutex );
}

/**
Routine du thread qui accepte les nouvelles connexions
*/
void* accept_callback( void* ptr )
{
	int new_sock;
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

		/* Sinon, on vérifie si un client est déjà connecté */
		pthread_mutex_lock( &client_mutex );
		old_sock = fd_client_sock;
		current_user_logged = 0;
		pthread_mutex_unlock( &client_mutex );

		if( old_sock <= 0 )
		{
			int res;

			/* Ce n'est pas le cas, donc on conserve le descripteur et on lance le thread */
			/* A noter que pas de client = pas d'environnement multithread donc ici, on peut ne pas utiliser de mutex */
			fd_client_sock = new_sock;
			res = pthread_create( &client_thread, NULL, client_callback, NULL );
		}
		else
		{
			/* Permutation avec le nouveau client, le thread gère la transition */
			pthread_mutex_lock( &client_mutex );
			fd_client_sock = new_sock;
			pthread_mutex_unlock( &client_mutex );

			/* Il existe, et là, couic */
			print( old_sock, "\nYou're gonna be kicked off.\n" );
			pthread_kill( client_thread, SIGALRM );
			close( old_sock );
		}
	}
}

/**
Démarre le mode télécommande
\param  port    	Port sur lequel écouter
	password	Mot de passe permettant d'accèder à la console
\return 0 si tout est ok
*/
int start_remote_control( unsigned int port, char* password )
{
	static struct sockaddr_in server_addr;
	int res;

	/* Pas de client à l'initialisation */
	fd_client_sock = -1;
	/* Et il est encore moins loggé */
	current_user_logged = 0;
	/* On conserve le mot de passe */
	strcpy( telnet_password_access, password );

	/* Ouverture d'un socket en mode flux */
	fd_sock = socket( AF_INET, SOCK_STREAM, 0 );
	if( fd_sock < 0 )
	{
		return RCTRL_CANNOT_OPEN_SOCKET;
	}

	/* Autorise les connexions sur le port passé en paramètre par n'importe qui */
	bzero( (char*) &server_addr, sizeof(server_addr) );

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons( port );
	int a=1;
	setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a));
	if( bind( fd_sock, (struct sockaddr *) &server_addr, sizeof(server_addr) ) < 0) 
	{
              	return RCTRL_CANNOT_BIND_SOCKET;
	}

	/* Autorise un certain nombre de connexions */
	listen( fd_sock, MAX_REMOTE_CONNECTIONS_ALLOWED );

	/* Création des nouveaux threads */
	res = pthread_create( &accept_thread, NULL, accept_callback, NULL );

	if( res != 0 )
		return RCTRL_CANNOT_LAUNCH_ACCEPT_THREAD;

	/* Initialisation des commandes */
	init_command();

	/* Tout est ok */
	return RCTRL_OK;
}

/**
Arrête le mode télécommande
*/
void stop_remote_control()
{
	/* Fermeture du socket d'écoute et du thread d'acceptation des connexions */
	close( fd_sock );

	pthread_cancel( accept_thread);

	release_command();
}
