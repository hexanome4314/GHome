<?php

require_once( "include/auth.php" );

force_authentication();

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="fr" >
  <head>
    <title>.:: GHome - Interface de gestion ::.</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <script type="text/javascript" src="js/jquery-1.7.1.min.js"></script>
    <script type="text/javascript" src="js/jquery-ui-1.8.18.custom.min.js"></script>
    <script type="text/javascript" src="js/sensors.js"></script>
    <script type="text/javascript" src="js/drivers.js"></script>
    <script type="text/javascript" src="js/rules.js"></script>
    <script type="text/javascript" src="js/utils.js"></script>
    <script type="text/javascript" src="js/ajaxfileupload.js"></script>
    <link rel="stylesheet" href="style/style.css" type="text/css" media="screen" />
    <link rel="stylesheet" href="style/jquery-ui-1.8.18.custom.css" type="text/css" media="screen" />
  </head>
  <body>

	<script type="text/javascript">

		$(document).ready(

			function() {

				update_sensor_data( true );
				update_driver_data();
				update_rules_data();
				setInterval( function() { update_sensor_data( true ); update_rules_data(); update_driver_data();  } , 300 );

				$("#main").show( "puff", 1000 );
				
				/* Logging off */
				$( "#logout span" ).click( function() {

					$.ajax( {
						async: false,
                                                type: "POST",
                                                url: "ajax/auth.php",
                                                data: "action=logoff" ,
                                                success: function( data ) {

							$( "#main" ).fadeOut( 1500, location.reload() );
						},
						error: function( ) {

							$( "#main" ).fadeOut( 1500, location.reload() );
						}						
					} );

					return false;
				} );

				/* Clique sur les onglets */
				$("a.tab").click(
					function() {
						/* Si l'élément courant n'est pas déjà actif */
						if( ! $(this).hasClass( "active" ) )
						{
							/* On change l'élément actif par celui en cours et on fait des effets jolis */
							$("ul.tabs a.active").removeClass( "active" );
							$(this).addClass( "active" );
							$(".content").slideUp();

							var contenu_aff = $(this).attr( "alt" );
							$("#" + contenu_aff).slideDown();
						}

						//update_fields();

						return false;
					}
				);

				$("a.under_tab").click(
					function() {
                                                /* Si l'élément courant n'est pas déjà actif */
                                                if( ! $(this).hasClass( "active" ) )
                                                {
                                                        /* On change l'élément actif par celui en cours et on fait des effets jolis */
                                                        $("ul.under_tabs a.active").removeClass( "active" );
                                                        $(this).addClass( "active" );
                                                        $(".under_content").slideUp();

                                                        var contenu_aff = $(this).attr( "alt" );
                                                        $("#" + contenu_aff).slideDown();
                                                }

                                                return false;
                                        }
				);

				return false;
			}
		);
	</script>

	<center>
	<div id="main" class="main hidden">

	<h4>GHome Project</h4>
	<div id="logout"><span>[D&eacute;connexion]</span></div>

	<div id="dialog">
		<span>&nbsp;</span>
	</div>

	<div id="notifier_box">
		<span>&nbsp;</span>
	</div>

	<div class="tabbed_area">
		<ul class="tabs">
			<li><a href="#" alt="content_1" class="tab active">G&eacute;n&eacute;ral</a></li>
			<li><a href="#" alt="content_2" class="tab">Configuration</a></li>
			<li><a href="#" alt="content_3" class="tab">Aide</a></li>
		</ul>

		<div id="content_1" class="content">
			<h3>G&eacute;n&eacute;ral</h3>

			<div id="sensor_data">
			&nbsp;
			</div>
		</div> 

		<div id="content_2" class="content">
			<h3>Configuration</h3>

			<div class="under_tabbed_area">
				<ul class="under_tabs">
					<li><a href="#" alt="drivers" class="under_tab active">Pilotes</a></li>
					<li><a href="#" alt="rules" class="under_tab">R&egrave;gles</a></li>
					<li><a href="#" alt="remote" class="under_tab">Acc&eacute;s distant</a></li>
				</ul>

				<div id="drivers" class="under_content">
					<h5>Pilotes</h5>

					<div id="driver_data">
					&nbsp;
					</div>
				</div>

				<div id="rules" class="under_content">
					<h5>R&egrave;gles</h5>

					<div id="rules_data">
					&nbsp;
					</div>
				</div>

				<div id="remote" class="under_content">
					<h5>Acc&egrave;s distant</h5>
					<p>Controle &agrave; distance</p>
				</div>
			</div>	

		</div>

		<div id="content_3" class="content">
			<h3>Aide</h3>

			<span>FAQ :</span>

			<div id="help_data">
				<ul>
					<li>Comment ajouter un nouveau driver?</li>
					<li>Comment ajouter un capteur &agrave; un driver?</li>
					<li>Comment ajouter une nouvelle r&egrave;gle?</li>
				</ul>
			</div>
		</div>
	</div>

	<div id="foot_box">
		<p>GHome Project by H4314</p>
		<p>Cherche pas, tu peux pas test.</p>
		<p>Copyleft 2012</p>
	</div>

	</div>
	</center>


  </body>
</html>
