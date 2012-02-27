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
	
	case "edit":
		die( '{ "type": "Error", "mesg": "Vous n\'êtes pas autorisés à effectuer cette action." }' );
		break;
	case "delete":

		/* Il est demandé de supprimer un capteur */
		if( @isset( $_POST['id'] ) ) {
			
			$res = send_ract_request( "PING" );

			echo '{ "type": "success", "mesg": "'.$res.'" }';
		}
		else
			die( '{ "type": "fail", "mesg": "Mauvaise requête." }' );


		break;
}	
?>
