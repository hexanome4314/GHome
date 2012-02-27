<?php
/**********************************************
* Fichier appelé lors de la maj des capteurs  *
* Author : Sébastien                          *
**********************************************/


require_once( "../include/auth.php" );
require_once( "../include/remote-act.php" );

if( is_authenticated() == false )
	die( '{ "type": "Error", "mesg": "Vous n\'êtes pas autorisés à effectuer cette action." }' );


/* Vérification que l'utilisateur est bien loggé */
switch( $_POST['action'] ) {

	/* Il est demandé d'éditer un capteur */	
	case "edit":

		if( @isset( $_POST['id'] ) ) {

			$res = send_ract_request( "PING" );
			echo '{ "type": "success", "mesg": "'.$res.'" }';
		}
		else 
			die( '{ "type": "fail", "mesg": "Mauvaise requête." }' );

		break;

	/* Il est demandé de supprimer un capteur */
	case "delete":
		if( @isset( $_POST['id'] ) ) {
			
			$res = send_ract_request( "PING" );
			echo '{ "type": "success", "mesg": "'.$res.'" }';
		}
		else
			die( '{ "type": "fail", "mesg": "Mauvaise requête." }' );

		break;
}	
?>
