<?php
/******************************************
* Gère la communication avec l'actionneur *
* Author : Sébastien                      *
******************************************/

require_once( "config.inc.php" );

/**
Envoie une commande à l'actionneur en gérant la connexion (die si problème de connexion)
\param	command	La commande à envoyer
\return	La chaîne retournée par l'actionneur
*/
function send_ract_request( $command ) {

	global $ract_host;
	global $ract_port;

	/* Il faut supprimer l'entrée dans le fichier xml... funky ! */
        $sock = @socket_create( AF_INET, SOCK_STREAM, SOL_TCP ) or die( '{ "type": "error", "mesg": "Le serveur rencontre un problème. Veuillez réessayer ultèrieurement. (socket_create)" }' );

        //Connexion au serveur
        @socket_connect( $sock, $ract_host, $ract_port ) or die( '{ "type": "error", "mesg": "Le serveur ne peut pas se connecter au noyau central. Veuillez réessayer ultèrieurement. (socket_connect)" }' );

        // Création du paquet
	$command .= "\r\n";
        $len = strlen( $command );
        socket_write( $sock, $command, $len );

	// Réception de la réponse et suppression du CRLF de fin
        $buf = socket_read( $sock, 255 );
        $buf = substr( $buf, 0, strlen( $buf ) - 2 );

	return $buf;

        // La connexion se ferme toute seule, donc pas besoin de la fermer
}


?>
