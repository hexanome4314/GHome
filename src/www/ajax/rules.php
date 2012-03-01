<?php
/**********************************************
* Fichier appelé lors de la maj des règles    *
* Author : Sébastien                          *
**********************************************/

require_once( "../include/auth.php" );
require_once( "../include/config.inc.php" );
require_once( "../include/remote-act.php" );
require_once( "../include/rules.php" );


/* Vérification que l'utilisateur est bien loggé */
if( is_authenticated() == false ) {
	$log_mgr->write( "Error", "Tentative d'accès à la ressource sans être authentifié (action=".$_POST['action'].")" );
	die( '{ "type": "Error", "mesg": "Vous n\'êtes pas autorisés à effectuer cette action." }' );
}

/* Vérification de la pertinence de la requête ... */
if( @isset( $_POST['action'] ) == false ) {
	$log_mgr->write( "Error", "Aucune action précisée. (user=".get_username().")" );
	die();
}



switch( $_POST['action'] ) {

	/* Il est demandé de supprimer une règle */
	case "delete":
		if( @isset( $_POST['rule'] ) ) {
			
			/* Quelque tests préliminaires pour voir si le fichier XML a les bons droits */
			if( is_readable( $rules_xml ) == false ) {
				$log_mgr->write( "Critical", "Le fichier '".$rules_xml."' ne peut pas être lu (droits iOS). (user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "Le fichier de configuration du serveur distant n\'est pas accessible. (+x)" }' );
			}
			if( is_writable( $rules_xml ) == false ) {
				$log_mgr->write( "Critical", "Le fichier '".$rules_xml."' ne peut pas être modifié (droits iOS). (user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "Le fichier de configuration du serveur distant n\'est pas accessible (+w)." }' );
			}

			/* Parsing du XML */
			$xml_content = file_get_contents( $rules_xml );
			$rules = new SimpleXMLElement( $xml_content );

			/* Suppression du noeud du capteur */
			$rules = remove_rule_node( $rules, $_POST['rule'] );

			if( @$rules->asXML( $rules_xml ) == false ) {
				
				$log_mgr->write( "Critical", "L'écriture du nouveau XML a échoué. (action=".$_POST['action']." / user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "L\'écriture du nouveau fichier a échoué." }' );
			}

			/* Averti le core */
			$res = /*"ok"; //*/ send_ract_request( "RELOAD_RULES" );
			
			exit( '{ "type": "success", "mesg": "'.$res.'" }' );
		}
		else {
			$log_mgr->write( "Error", "Requête erronée (action=".$_POST['action']." / user=".get_username().")" );
			die( '{ "type": "fail", "mesg": "Mauvaise requête." }' );
		}

		break;

	default:
		$log_mgr->write( "Error", "Requête erronée (action=".$_POST['action']." / user=".get_username().")" );
		die( '{ "type": "fail", "mesg": "Mauvaise requête." }' );
}	
?>
