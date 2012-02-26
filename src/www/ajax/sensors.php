<?php
/**********************************************
* Fichier appelé lors de la maj des capteurs  *
* Author : Sébastien                          *
**********************************************/


require_once( "../include/auth.php" );

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
			
			/* Il faut supprimer l'entrée dans le fichier xml... funky ! */
		}
		else
			die( '{ "type": "fail", "mesg": "Mauvaise requête." }' );


		break;
}	
?>
