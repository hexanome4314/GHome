/**
Permet de replier et de déplier les différentes infos des drivers
*/
function collapse_device_list( element ) {

	if( element.hasClass( 'expanded' ) ) {
		element.find( 'img' ).attr( 'src', "img/expand.gif" );
		$(".device_list").slideUp();

		element.removeClass( 'expanded' );
	}
	else {
		element.find( 'img' ).attr( 'src', "img/collapse.gif" );
		$(".device_list").slideDown();

		element.addClass( 'expanded' );
	}
}

/**
Affiche les informations relatives à un capteurs
*/
function show_device_info( id ) {
	var dialog_title = "";

	/* On commence pa récupérer dans le xml les dernières info que l'on a du capteur */
	$.ajax( {
		async: false,
		type: "GET",
		dataType: "xml",
		url: "data/sensors.xml",
		success: function( data ) {
	
			/* Avec XPath, on choppe le bon noeud */
			var node = $(data).find( 'capteur[id=' + id + ']' );

			if( node.length == 1 ) {
				/* On prépare les infos de la fenêtre */
				dialog_title = 'Capteur "' + node.text()  + '"';

				var content = '<div class="content">';

				content += "<h3>" + node.text() + "</h3>";

				content += '<p>Driver :</p>';
				content += '<p class="value">' + node.parent().attr( 'so' ) + "</p>";

				content += "<p>Identifiant :</p>";
				content += '<p class="value">' + node.attr( 'id' ) + "</p>";

				content += '<p>Type : </p>';
				content += '<p class="value">' + node.attr( 'type' ) + "</p>";

				content += '<p>Etat : </p>';
				content += '<p class="value">' + node.attr( 'etat' ) + "</p>";

				content += "</div>";

				$("#dialog").html( content );
			}
			else {
				dialog_title = 'Oups !';

				var content = '<div class="content">';
				content += "<p>Il semblerait que le capteur demandé n'existe pas !.</p>";
				content += '</div>';

				$("#dialog").html( content );
			}
		},
		error: function( data ) {
			dialog_title = "Erreur...";

			var content = '<div class="content">';
			content += '<p>Une erreur est survenue lors du chargement des informations liées au capteur.</p>';
			content += '</div>';

			$("#dialog").html( content );
		}
	} );

	/* Et on affiche la dialog */
	$("#dialog").dialog( {
		title: dialog_title,
		width: 400,
		resizable: false,
		buttons: { "Ok": function() { $(this).dialog( "close" ); },
			   "Modifier": function() { $(this).dialog( "close" ); edit_device_info( id ); },
			   "Supprimer": function() { $(this).dialog( "close" ); confirm_remove_device( id ); }
			 }
	});
}

/**
Permet d'éditer les informations d'un capteur
*/
function edit_device_info( id ) {
	alert( "Edit " + id );
}

/**
Demande à l'utilisateur de confirmer qu'il veut supprimer un capteur
*/
function confirm_remove_device( id ) {

	var content = '<div class="content">';
	content += '<h3>Suppression d\'un capteur</h3>';
	content += '<img src="img/button/delete.png" />';
	content += '<p>&Ecirc;tes-vous vraiment s&ucirc;r de vouloir supprimer ce capteur?</p>';
	content += '</div>';

	$("#dialog").html( content );

	$("#dialog").dialog( {
		title: 'Confirmation',
		width: 400,
		resizable: false,
		buttons: { "Oui": function() { $(this).dialog( "close" ); remove_device( id ); },
			   "Non": function() { $(this).dialog( "close" ); }
			}
	} );
}

/**
Supprime un capteur
*/
function remove_device( id ) {

	/* On poste la requête */
	$.ajax( {
		async: true,
		type: "POST",
		dataType: "json",
		url: "ajax/sensors.php",
		data: "action=delete&id="+id ,
		success: function( data ) {

			if( data.type == "error" )
				alert( "Erreur :\n" + data.mesg );
			else {
				alert( data.mesg );
			}
		},
		error: function() {
			alert( "Une erreur est survenue lors de l'envoi de la requête de suppression au serveur.\nVeuillez réessayer plus tard." );
		}
	});
}

/**
Lecture du XML concernant les périphériques et les drivers
*/
function update_driver_data() {

	$.ajax( {
		async: false,
                type: "GET",
                dataType: "xml",
                url: "data/sensors.xml",
                success: function( data ) {
			
			str = "";

			$( data ).find( 'driver' ).each( function() {
				
				str += '<a href="#" class="collapse expanded"><img src="img/collapse.gif" /></a>';
				str += '<span class="driver_name">' + $(this).attr( 'so' ) + "</span>";
				str += '<div class="device_list">';

				$(this).find( 'capteur' ).each( function() {
					id = $(this).attr( 'id' );

					str += '<span>' + $(this).text() + '</span>'
					str += '<a href="#" class="view" alt="' + id + '"><img src="img/button/view.png" /></a>'
					str += '<a href="#" class="edit" alt="' + id + '"><img src="img/button/edit.png" /></a>'
					str += '<a href="#" class="del"  alt="' + id + '"><img src="img/button/delete.png" /></a>'
					str += '<br />';
				} );

				str += '</div>';
				
			} );

			$( '#driver_data' ).html( str );
		},
		error: function( xhr, str, error) {
			alert( "Impossible de charger les données relatives aux pilotes." );

			$( '#driver_data' ).html( '<img src="img/error.png" /><span><i>Impossible de charger les donn&eacute;es relatives aux pilotes.</span>' );
		}
	} );
}
