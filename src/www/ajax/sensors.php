<?php
/**********************************************
* Fichier appelé lors de la maj des capteurs  *
* Author : Sébastien                          *
**********************************************/

require_once( "../include/auth.php" );
require_once( "../include/config.inc.php" );
require_once( "../include/remote-act.php" );
require_once( "../include/sensors.php" );


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

	/* Il est demandé d'ajouter un capteur */
	case "add":
		if( @(  isset( $_POST['new_id'] ) && 
			isset( $_POST['new_drv'] ) && 
			isset( $_POST['name'] ) &&
			isset( $_POST['state'] )) ) {

			/* On désérialise le nom du driver */
			list($drv, $ip, $port) = parse_driver_name( $_POST['new_drv'] );

			/* Quelque tests préliminaires pour voir si le fichier XML a les bons droits */
			if( is_readable( $sensors_xml ) == false ) {
				$log_mgr->write( "Critical", "Le fichier '".$sensors_xml."' ne peut pas être lu (droits iOS). (user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "Le fichier de configuration du serveur distant n\'est pas accessible. (+x)" }' );
			}
			if( is_writable( $sensors_xml ) == false ) {
				$log_mgr->write( "Critical", "Le fichier '".$sensors_xml."' ne peut pas être modifié (droits iOS). (user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "Le fichier de configuration du serveur distant n\'est pas accessible (+w)." }' );
			}

			/* Parsing du XML */
			$xml_content = file_get_contents( $sensors_xml );
			$drivers = new SimpleXMLElement( $xml_content , LIBXML_DTDATTR );

			/* On cherche le capteur à éditer avec une requête xpath */
			$driver_node = fetch_driver_node( $drivers, $drv, $ip, $port );
			
			/* Maintenant que l'on a le bon noeud, on procède à l'ajout ! */
			/* Et on crée le nouveau noeud ! */
			$new_node = $driver_node->addChild( "capteur", $_POST['name'] );
			$new_node->addAttribute( "id", $_POST['new_id'] );
			$new_node->addAttribute( "etat", $_POST['state'] );

			/* Il ne reste plus qu'à écrire le nouveau fichier */
			if( @$drivers->asXML( $sensors_xml ) == false ) {

				$log_mgr->write( "Critical", "L'écriture du nouveau XML a échoué. (action=".$_POST['action']." / user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "L\'écriture du nouveau fichier a échoué." }' );
			}

			/* On averti le core */
			$res = /* "ok"; //*/ send_ract_request( "RELOAD_DRIVERS" );

			exit( '{ "type": "success", "mesg": "'.$res.'" }' );
		}
		else {
			$log_mgr->write( "Error", "Requête erronée (action=".$_POST['action']." / user=".get_username().")" );
			die( '{ "type": "fail", "mesg": "Mauvaise requête." }' );
		}

		break;

	/* Il est demandé d'éditer un capteur */	
	case "edit":

		if( @(  isset( $_POST['id'] ) &&
			isset( $_POST['drv'] ) && 
			isset( $_POST['new_id'] ) && 
			isset( $_POST['new_drv'] ) && 
			isset( $_POST['name'] ) &&
			isset( $_POST['state'] )) ) {

			/* On désérialise le nom du driver */
			list($drv, $ip, $port) = parse_driver_name( $_POST['drv'] );

			/* Quelque tests préliminaires pour voir si le fichier XML a les bons droits */
			if( is_readable( $sensors_xml ) == false ) {
				$log_mgr->write( "Critical", "Le fichier '".$sensors_xml."' ne peut pas être lu (droits iOS). (user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "Le fichier de configuration du serveur distant n\'est pas accessible. (+x)" }' );
			}
			if( is_writable( $sensors_xml ) == false ) {
				$log_mgr->write( "Critical", "Le fichier '".$sensors_xml."' ne peut pas être modifié (droits iOS). (user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "Le fichier de configuration du serveur distant n\'est pas accessible (+w)." }' );
			}

			/* Parsing du XML */
			$xml_content = file_get_contents( $sensors_xml );
			$drivers = new SimpleXMLElement( $xml_content , LIBXML_DTDATTR );

			/* On cherche le capteur à éditer avec une requête xpath */
			$driver_node = fetch_driver_node( $drivers, $drv, $ip, $port );
			
			/* Maintenant que l'on a le bon noeud, on choppe le bon capteur tjs avec XPath*/
			$sensor_node = $driver_node->xpath( "capteur[@id='".$_POST['id']."']" );
			
			/* Vérification de la cohérence */
			if( count( $sensor_node ) == 0 ) {
				$log_mgr->write( "Warning", "Demande de modification d'un capteur introuvable. (drv=".$_POST['drv']." / id=".$_POST['id']." / user=".get_username().")" );
				die( '{ "type": "error", "mesg": "Le capteur \''.$_POST['id'].'\' ne peut être trouvé." }' );
			}
			else if( count( $sensor_node ) > 1 ) {
				$log_mgr->write( "Critical", "Demande de modification d'un capteur redondant. (drv=".$_POST['drv']." / id=".$_POST['id']." / count=".count( $driver_node )." / user=".get_username().")" );
				die( '{ "type": "error", "mesg": "Fichier XML incohérent." }' );
			}

			/* On peut appliquer les modifications */
			$sensor_node = $sensor_node[0];
		
			/* Si jamais on change le driver de référence, et là c'est corsé */
			if( strcmp( $_POST['drv'], $_POST['new_drv'] ) != 0 ) {

				/* On désérialise le nouveau nom */
				list($new_drv, $new_ip, $new_port) = parse_driver_name( $_POST['new_drv'] );				

				/* On choppe le bon noeud avec XPath */
				$target_node = fetch_driver_node( $drivers, $new_drv, $new_ip, $new_port );
				
				/* Et on crée le nouveau noeud ! */
				$new_node = $target_node->addChild( "capteur", $_POST['name'] );
				$new_node->addAttribute( "id", $_POST['new_id'] );
				$new_node->addAttribute( "etat", $_POST['state'] );

				/* Suppression de l'ancien noeud du capteur */
				$drivers = remove_sensor_node( $drivers, $drv, $ip, $port, $_POST['id'] );
			}
			else {
				/* Modifications basiques */
				$sensor_node['etat'] = $_POST['state'];
				$sensor_node['id']   = $_POST['new_id'];
				$sensor_node[0] = $_POST['name'];
			}

			/* Il ne reste plus qu'à écrire le nouveau fichier */
			if( @$drivers->asXML( $sensors_xml ) == false ) {

				$log_mgr->write( "Critical", "L'écriture du nouveau XML a échoué. (action=".$_POST['action']." / user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "L\'écriture du nouveau fichier a échoué." }' );
			}

			/* On averti le core */
			$res = /*"ok"; //*/ send_ract_request( "RELOAD_DRIVERS" );

			exit( '{ "type": "success", "mesg": "'.$res.'" }' );
		}
		else {
			$log_mgr->write( "Error", "Requête erronée (action=".$_POST['action']." / user=".get_username().")" );
			die( '{ "type": "fail", "mesg": "Mauvaise requête." }' );
		}

		break;

	/* Il est demandé de supprimer un capteur */
	case "delete":
		if( @isset( $_POST['id'] ) &&
		     isset( $_POST['drv'] ) ) {
			
			/* On désérialise le nom du driver */
			list($drv, $ip, $port) = parse_driver_name( $_POST['drv'] );

			/* Quelque tests préliminaires pour voir si le fichier XML a les bons droits */
			if( is_readable( $sensors_xml ) == false ) {
				$log_mgr->write( "Critical", "Le fichier '".$sensors_xml."' ne peut pas être lu (droits iOS). (user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "Le fichier de configuration du serveur distant n\'est pas accessible. (+x)" }' );
			}
			if( is_writable( $sensors_xml ) == false ) {
				$log_mgr->write( "Critical", "Le fichier '".$sensors_xml."' ne peut pas être modifié (droits iOS). (user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "Le fichier de configuration du serveur distant n\'est pas accessible (+w)." }' );
			}

			/* Parsing du XML */
			$xml_content = file_get_contents( $sensors_xml );
			$drivers = new SimpleXMLElement( $xml_content , LIBXML_DTDATTR );

			/* Suppression du noeud du capteur */
			$drivers = remove_sensor_node( $drivers, $drv, $ip, $port, $_POST['id'] );

			if( @$drivers->asXML( $sensors_xml ) == false ) {
				
				$log_mgr->write( "Critical", "L'écriture du nouveau XML a échoué. (action=".$_POST['action']." / user=".get_username().")" );
				die( '{ "type": "fail", "mesg": "L\'écriture du nouveau fichier a échoué." }' );
			}

			/* Averti le core */
			$res = /*"ok"; //*/ send_ract_request( "RELOAD_DRIVERS" );
			
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
