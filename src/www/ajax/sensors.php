<?php
/**********************************************
* Fichier appelé lors de la maj des capteurs  *
* Author : Sébastien                          *
**********************************************/

session_start();

/* Vérification que l'utilisateur est bien loggé */
if( @isset( $_SESSION['GHOME'] ) && $_SESSION['GHOME'] == 'yes' ) {

	switch( $_POST['action'] ) {
	
		case "edit":
			break;
		case "delete":
			break;
	}	

}
else {
	echo '{ "type": "Error", "mesg": "Vous n\'êtes pas autorisés à effectuer cette action." }';
}

?>
