<?php
/**********************************************
* Permet la journalisation des évènements     *
* Author : Sébastien                          *
**********************************************/

include( "config.inc.php" );

class Log {

	/**
	Constructeur qui ne fait rien...
	*/
	public function __construct( ) {
	}

	/**
	Ecrit une nouvelle entrée dans le journal
	\param	type	Le type (erreur, warning, debug, ...)
		msg	Le message décrivant l'entrée
	*/
	public function write( $type, $msg ) {
		
		global $log_file;

		$fd = fopen( $log_file, "a" );

		$line  = date( 'j-m-Y H:i:s' );
		$line .= "\t[".$type."] ".$msg;
		$line .= "   [".$_SERVER['REMOTE_ADDR']."]";
		$line .= " [".$_SERVER['SCRIPT_FILENAME']."]";
		$line .= "\r\n";

		fwrite( $fd, $line );
		fflush( $fd );

		fclose( $fd );
	}
};
	

$log_mgr = new Log;

?>
