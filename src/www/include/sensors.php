<?php
/**********************************************
* Boîte à outils pour la gestion des capteurs *
* Author : Sébastien                          *
**********************************************/

require_once( "config.inc.php" );

include( "log.php" );

/**
Permet de parser le nom d'un driver sérialisé
\param	raw	Le nom brut
\return	Un tableau contenant (drv_name, ip, port)
*/
function parse_driver_name( $raw ) {

	$drv  = "";
	$ip   = "";
	$port = "";

	/* Le nom du driver est de la forme "drv_name [ip:port]", donc il faut parser ça, si ça colle pas, pas d'IP ! */
        $off_bl = strpos( $raw, " [" );
        $off_br = strpos( $raw, "]" );

        if( $off_bl != 0 ) {
		$ip_len = strpos( strstr( $raw, "["  ), ":" ) - 1;
		$port_len = strpos( strstr( strstr( $raw, "[" ), ":" ), "]" ) - 1;

		/* Pour le nom du driver, on prend du début jusqu'à '[' */
		$drv = substr( $raw, 0, $off_bl );
		$ip  = substr( $raw, $off_bl + 2, $ip_len );
		$port= substr( $raw, $off_bl + $ip_len + 3, $port_len );
	}
	else {
		$drv = $raw;
	}

	return array( $drv, $ip, $port ); 
}

/**
Récupère le noeud d'un driver
\param	root_node	Le noeud racine de l'arbre XML
	drv		Le nom du driver
	ip		L'IP du driver
	port		Le port du driver
\return Le noeud du driver cherché
*/
function fetch_driver_node( $root_node, $drv, $ip, $port ) {

	global $log_mgr;

	/* On cherche le capteur à éditer avec une requête xpath */
	$driver_node = null;

	if( strlen( $ip ) > 0 ) {
		$driver_node = $root_node->xpath( "driver[@so='".$drv."' and @ip='".$ip."' and @port='".$port."']" );
	}
	else {
		$driver_node = $root_node->xpath( "driver[@so='".$drv."' and not(@ip)]" );
	}

	/* On vérifie que l'on a des données cohérentes */
	if( count( $driver_node ) == 0 ) {
		$log_mgr->write( "Warning", "Demande de modification d'un capteur sur un driver introuvable. (drv=".$drv." / user=".get_username().")" );
		die( '{ "type": "error", "mesg": "Le driver \''.$drv.' ['.$ip.':'.$port.']\' ne peut être trouvé." }' );
	}
	else if( count( $driver_node ) > 1 ) {
		$log_mgr->write( "Critical", "Demande de modification d'un capteur sur un driver redondant. (drv=".$drv." / count=".count( $driver_node )." / user=".get_username().")" );
		die( '{ "type": "error", "mesg": "Fichier XML incohérent." }' );
	}

	return $driver_node[0];
}

/**
Supprime un capteur d'un driver
\param	root_node	Noeud racine de l'arbre XML
	drv		Nom du capteur contenant le capteur
	ip		IP du capteur
	port		Port du capteur
	id		ID du capteur à supprimer
\return Le nouvel arbre XML
*/
function remove_sensor_node( $root_node, $drv, $ip, $port, $id ) {

	global $log_mgr;

	/* Suppression de l'ancien noeud du capteur en utilisant DOM car SimpleXML le fait pas... (et merde) */
	$dom = new DOMDocument();

	$drv_dom = $dom->importNode( dom_import_simplexml( $root_node ), true );
        $dom->appendChild( $drv_dom );
	$xp = new DOMXPath( $dom );

	/* On sélectionne le noeud avec une petite requête xpath */
	$res = null;
	if( strlen( $ip ) > 0 ) {
		$res = $xp->query( "driver[@so='".$drv."' and @ip='".$ip."' and @port='".$port."']/capteur[@id='".$id."']" );
	}
	else {
		$res = $xp->query( "driver[@so='".$drv."' and not(@ip)]/capteur[@id='".$id."']" );
	}

	/* Si on a pas de noeud, il y a une couille */
	if( $res->length != 1 ) {
		$log_mgr->write( "Critical", "DOM a foiré pour fetcher un capteur à supprimer. (drv=".$drv." / ip=".$ip.":".$port." / id=".$id." / user=".get_username().")" );
		die( '{ "type": "error", "mesg": "Une erreur est survenue lors de l\'intérogation du DOM. Cette erreur est dommage..." }' );
	}

	/* Supression ! */
	$res->item(0)->parentNode->removeChild( $res->item(0) );

	/* Recréation du XMLElement */
	return new SimpleXMLElement( $dom->saveXML() );
}

?>
