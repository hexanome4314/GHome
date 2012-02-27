<?php
/**********************************************
* Permet l'authentification de l'utilisateur  *
* Author : Sébastien                          *
**********************************************/

session_start();

/**
Vérifie que l'utilisateur courant est authentifié
\return Vrai si c'est le cas, faux dans le cas contraire
*/
function is_authenticated() {

	return @(isset( $_SESSION['GHOME'] ) && $_SESSION['GHOME'] == "yes");
}

/**
Force l'authentification de l'utilisateur si celui-ci n'est pas déjà authentifié
*/
function force_authentication() {

	if( is_authenticated() )
		return;

	header( "Location: login.php" );
}

/**
Enregistre la session de l'utilisateur
*/
function set_authenticated( $user ) {
	
	$_SESSION['GHOME'] = "yes";
}

?>