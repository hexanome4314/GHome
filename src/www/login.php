<?php

require_once( "include/auth.php" );

if( is_authenticated() )
	header( "Location: index.php" );

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="fr" >
  <head>
    <title>.:: GHome - Authentification ::.</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <script type="text/javascript" src="js/jquery-1.7.1.min.js"></script>
    <script type="text/javascript" src="js/jquery-ui-1.8.18.custom.min.js"></script>
    <script type="text/javascript" src="js/SHA1.js"></script>
    <link rel="stylesheet" href="style/style.css" type="text/css" media="screen" />
    <link rel="stylesheet" href="style/jquery-ui-1.8.18.custom.css" type="text/css" media="screen" />
  </head>

  <body>

        <script type="text/javascript">

                $(document).ready(

                        function() {

				$( "#main" ).fadeIn( 1000 );

				$("#auth_button").click( function() {

					var res = false;

					var username = $("input[name=username]").attr( 'value' );
					var password = $("input[name=password]").attr( 'value' );

					/* Petite requête ajax pour vérifier l'identité */
					$.ajax( {
						async: false,
				                type: "POST",
				                dataType: "json",
				                url: "ajax/auth.php",
				                data: "action=auth&username="+username+"&password="+hex_sha1(password) ,
				                success: function( data ) {

							/* Affichage du message si on a une erreur */
				                        if( data.type == "error" || data.type == "fail" ) {

								$(".content").effect( "shake", { times: 1 }, 120 );

								$("#error_msg").html( data.mesg );
								$("#error_msg").slideDown();
							}
				                        else {
								$("#main").hide( "puff", {  }, 1000 );

								$("#main").promise().done( function() { location.reload(); } );
				                        }
				                },
				                error: function( j, t, e) {
				                        $("#error_msg").html( "Une erreur est survenue lors de l'envoi de la requ&ecirc;te au serveur. Veuillez retenter ult&egrave;rieurement." );
							$("#error_msg").slideDown();
				                }				
	
					} );

					return res;
				} );

				return false;
			}
		);
	</script>

	<center>
	<div id="main" class="main hidden">

	<h4>GHome Project</h4>
	<div id="logout"></div>

	<div class="content">
		<h3>Authentification</h3>

		<p id="error_msg" class="error_msg">Le nom d'utilisateur ou le mot passe saisi n'est pas valide.</p>

		<p>Veuillez vous authentifier avant de poursuivre:</p>

		<div class="content auth">
		<div class="table">
			<div class="">
				<div class="cell">
					<label for="username">Utilisateur</label>
				</div>
				<div class="cell">
					<input type="text" name="username" class="text" />
				</div>
			</div>
			<div class="">
				<div class="cell">
					<label for="password">Mot de passe</label>
				</div>
				<div class="cell">
					<input type="password" name="password" class="text" />
				</div>
			</div>
		</div>

		<input type="submit" value="Se connecter" name="action" class="button" id="auth_button" />
		</div>
	</div>

	</div>
	</center>

  </body>
</html>
