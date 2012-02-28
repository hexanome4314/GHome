<?php
/**********************************************
* Fichier appelé lors de la tentativie d'auth *
* Author : Sébastien                          *
**********************************************/

require_once( "../include/auth.php" );
include( "../include/config.inc.php" );
include( "../include/log.php" );

if( @$_POST['action'] == "auth") {
	
	if( is_authenticated() ) {
		echo '{ "type": "success", "mesg": "ok" }';
		die();
	}
	else {
		/* Va chercher les informations d'authentification ! */
		$raw_data = file_get_contents( $auth_xml );
		$auth = new SimpleXMLElement( $raw_data );

		/* On cherche le bon utilisateur */
		foreach( $auth->web->user as $user ) {

			/* Si on a la bon */
			if( strcasecmp( $user['name'], $_POST['username'] ) == 0 ) {
			
				/* On check le mot de passe */
				if( $user['passwd'] == $_POST['password'] ) {
					echo '{ "type": "success", "mesg": "ok" }';

					set_authenticated( $_POST['username'] );


					$log_mgr->write( "Info", "Authentification réussie (username: ".$_POST['username'].")." );
					die();
				}
				else {
					echo '{ "type": "fail", "mesg": "Mot de passe incohérent." }';

					$log_mgr->write( "Warning", "Authentification échouée (username: ".$_POST['username'].")." );
					die();
				}
			}
		}

		echo '{ "type": "fail", "mesg": "Utilisateur inconnu." }';
		$log_mgr->write( "Warning", "Authentification échouée (username: ".$_POST['username'].")." );
		die();
	}
}
else if( $_POST['action'] == "logoff" ) {
	
	/* On déconnecte l'utilisateur si besoin */
	if( is_authenticated() ) {

		$log_mgr->write( "Info", "L'utilisateur s'est déconnecté. (username= ".get_username().")" );
		disconnect_user();
	}

	exit();
}
else {

	echo '{ "type": "error", "mesg": "Unknown action..." }';

	$log_mgr->write( "Error", "Mauvaise requête." );
	die();
}

?>
