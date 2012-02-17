<?php
/**********************************************
* Fichier appelé lors de la maj des règles    *
* Author : Sébastien                          *
**********************************************/

$host = "localhost";
$port = "1100";

// Aucun timeout
set_time_limit( 0 );

$sock = socket_create( AF_INET, SOCK_STREAM, SOL_TCP ) or die('Création de socket refusée');

//Connexion au serveur
socket_connect( $sock, $host, $port ) or die('Connexion impossible');


// Création du paquet

//$pack = "help\r\n";
//$len = strlen( $pack );
// socket_write( $sock, $paquet, $len );
// $buf = socket_read( $sock, 255 );
// echo $buf;


//Fermeture de la connexion
socket_close($sock);

?>
