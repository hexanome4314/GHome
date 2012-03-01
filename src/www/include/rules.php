<?php
/**********************************************
* Boîte à outils pour la gestion des règles   *
* Author : Sébastien                          *
**********************************************/

require_once( "config.inc.php" );

include( "log.php" );

/**
Supprime une règle
\param	root_node	Noeud racine de l'arbre XML
	rule		Le nom de la règle à supprimer
\return Le nouvel arbre XML
*/
function remove_rule_node( $root_node, $rule ) {

	global $log_mgr;

	/* Suppression en utilisant DOM car SimpleXML le fait pas... (et merde) */
	$dom = new DOMDocument();

	$drv_dom = $dom->importNode( dom_import_simplexml( $root_node ), true );
        $dom->appendChild( $drv_dom );
	$xp = new DOMXPath( $dom );

	/* On sélectionne le noeud avec une petite requête xpath */
	$res = $xp->query( "rule[@name='".$rule."']" );

	/* Si on a pas de noeud, il y a une couille */
	if( $res->length != 1 ) {
		$log_mgr->write( "Critical", "DOM a foiré pour fetcher une règle à supprimer. (drv=".$drv." / ip=".$ip.":".$port." / id=".$id." / user=".get_username().")" );
		die( '{ "type": "error", "mesg": "Une erreur est survenue lors de l\'intérogation du DOM. Cette erreur est dommage..." }' );
	}

	/* Supression ! */
	$res->item(0)->parentNode->removeChild( $res->item(0) );

	/* Recréation du XMLElement */
	return new SimpleXMLElement( $dom->saveXML() );
}

?>
