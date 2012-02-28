/**
Permet de replier et de déplier les différentes infos des drivers
*/
function collapse_device_list( element ) {

	if( element.hasClass( 'expanded' ) ) {
		element.find( 'img' ).attr( 'src', "img/expand.gif" );
		element.siblings( ".device_list" ).slideUp();

		element.removeClass( 'expanded' );
	}
	else {
		element.find( 'img' ).attr( 'src', "img/collapse.gif" );
		element.siblings( ".device_list" ).slideDown();

		element.addClass( 'expanded' );
	}
}

/**
Affiche les informations relatives à un capteurs
*/
function show_device_info( id ) {
	var dialog_title = "";

	/* On commence par récupérer dans le xml les dernières info que l'on a du capteur */
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
				content += '<p class="value">';

				/* Formattage du nom du driver en fonction de s'il y a une IP ou pas */
				content += node.parent().attr( 'so' );
				if( node.parent().attr( 'ip' ) != null ) {
					content += ' [' + node.parent().attr( 'ip' ) + ':' + node.parent().attr( 'port' ) + ']';
				}

				content += '</p>';

				content += "<p>Identifiant :</p>";
				content += '<p class="value">' + node.attr( 'id' ) + "</p>";

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
	var dialog_title = "";
	var drv_name ="";

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

				content += "<h3>&Eacute;dition</h3>";

				content += '<form id="edit_sensor" class="edit_sensor">';

				content += '<p>Driver :</p>';
				content += '<select id="new_driver" name="new_driver" class="text">';

				/* Récupération de l'ensemble des drivers disponibles via le xml et une requête xpath */
				drv_name = node.parent().attr( 'so' );
				if( node.parent().attr( 'ip' ) ) {
					drv_name += ' [' + node.parent().attr( 'ip' ) + ':' + node.parent().attr( 'port' ) + ']';
				}

				$(data).find( 'driver' ).each( function() {
					
					var cur_drv_name = $(this).attr( 'so' );
					var sel = "";
			
					if( $(this).attr( 'ip' ) != null ) {
						cur_drv_name += ' [' + $(this).attr( 'ip' ) + ':' + $(this).attr( 'port' ) + ']';
					}
					
					if( cur_drv_name == drv_name ) sel = "SELECTED";

					content += '<option value="' + cur_drv_name + '" ' + sel + '>';
					content += cur_drv_name;
					content += '</option>';
				} );

				content += '</select>';

				content += '<p>Nom capteur :</p>';
				content += '<input type="text" id="name" name="name" value="' + node.text() + '" class="text" />';

				content += "<p>Identifiant :</p>";
				content += '<input type="text" id="new_id" name="new_id" value="' + node.attr( 'id' ) + '" class="text" />';

				content += '<p>Etat : </p>';
				content += '<select id="state" name="state" class="text">';
				
				var is_on  = "";
				var is_off = "";

				if( node.attr( 'etat' ) == 'ON' )
					is_on = "selected";
				else
					is_off = "selected";

				content += '<option value="OFF" ' + is_off + '>OFF</option>';
				content += '<option value="ON" ' + is_on + '>ON</option>';

				content += '</select>';

				content += '</form>';

				content += "</div>";

				$("#dialog").html( content );

				/* Et on affiche la dialog */
				$("#dialog").dialog( {
					title: dialog_title,
					width: 450,
					resizable: false,
					buttons: { "Enregistrer": function() { if( save_device_info( drv_name, id ) == 1 ) $(this).dialog( "close" ); },
						   "Annuler": function() { $(this).dialog( "close" ); },
						 }
				});
			}
			else {
				dialog_title = 'Oups !';

				var content = '<div class="content">';
				content += "<p>Il semblerait que le capteur demandé n'existe pas !.</p>";
				content += '</div>';

				$("#dialog").html( content );

				/* Et on affiche la dialog */
				$("#dialog").dialog( {
					title: dialog_title,
					width: 350,
					resizable: false,
					buttons: { 
						   "Ok": function() { $(this).dialog( "close" ); },
						 }
				});
			}
		},
		error: function( data ) {
			dialog_title = "Erreur...";

			var content = '<div class="content">';
			content += '<p>Une erreur est survenue lors du chargement des informations liées au capteur.</p>';
			content += '</div>';

			$("#dialog").html( content );

			/* Et on affiche la dialog */
			$("#dialog").dialog( {
				title: dialog_title,
				width: 350,
				resizable: false,
				buttons: { 
					   "Ok": function() { $(this).dialog( "close" ); },
					 }
			});
		}
	} );
}

/**
Enregistrement des données saisies par l'utilisateur
*/
function save_device_info( old_driver, old_id ) {

	var wrong_data = false;

	/* On commence par vérifier que l'on a toutes les infos nécessaires */
	$( "#new_driver" ).css( 'background', '#33FF66').css( 'color', '#000000' );

	if( $( "#name" ).attr( 'value' ).length == 0 ) {
		$( "#name" ).css( 'background', '#B00000').css( 'color', '#FFFFFF' );
		wrong_data = true;
	}
	else {
		$( "#name" ).css( 'background', '#33FF66').css( 'color', '#000000' );
	}

	if( $( "#new_id" ).attr( 'value' ).length == 0 ) {
		$( "#new_id" ).css( 'background', '#B00000').css( 'color', '#FFFFFF' );
		wrong_data = true;
	}
	else {
		$( "#new_id" ).css( 'background', '#33FF66').css( 'color', '#000000' );
	}

	if( $( "#state" ).attr( 'value' ).length == 0 ) {
		$( "#state" ).css( 'background', '#B00000').css( 'color', '#FFFFFF' );
		wrong_data = true;
	}
	else {
		$( "#state" ).css( 'background', '#33FF66').css( 'color', '#000000' );
	}

	/* Si une donnée est pas bonne, on s'arrête là. */
	if( wrong_data == true )
		return 0;

	/* Stockage des données */
	var new_driver 	= $( "#new_driver" ).attr( 'value' );
	var name 	= $( "#name" ).attr( 'value' );
	var new_id	= $( "#new_id" ).attr( 'value' );
	var state	= $( "#state" ).attr( 'value' );


	/* Et maintenant on envoie tout au php qui va computer tout ça */
	$.ajax( {
		async: false,
		type: "POST",
		dataType: "json",
		url: "ajax/sensors.php",
		data: "action=edit&id="+old_id+"&drv="+old_driver+"&new_drv="+new_driver+"&new_id="+new_id+"&name="+name+"&state="+state,
		success: function( data ) {
			alert( data.mesg );
		},
		error: function( j, t, e ) {
			alert( 'Une erreur est survenue lors de l\'enregistrement des données. Veuillez réessayer plus tard.\n' + t + ' - ' + e );
		}
	} );

	return 0;
}

/**
Permet l'ajout d'un nouveau capteur
\driver_name	Le nom du driver auquel doit être affecté le nouveau capteur
*/
function new_device( driver_name ) {
	var dialog_title = "Ajout d'un nouveau capteur";
	var show_dialog = true;

	/* On prépare les infos de la fenêtre */
	var content = '<div class="content">';

	content += "<h3>Ajout d'un nouveau capteur</h3>";

	content += '<form id="edit_sensor" class="edit_sensor">';

	content += '<p>Driver :</p>';
	content += '<select id="new_driver" name="new_driver" class="text">';

	/* On récupére dans le xml la liste des drivers */
	$.ajax( {
		async: false,
		type: "GET",
		dataType: "xml",
		url: "data/sensors.xml",
		success: function( data ) {
	
			/* Avec XPath, on choppe le bon noeud */
			var node = $(data).find( 'capteur[id=' + id + ']' );

				/* Récupération de l'ensemble des drivers disponibles via le xml et une requête xpath */
				$(data).find( 'driver' ).each( function() {
				
					var cur_drv_name = $(this).attr( 'so' );
					var sel = "";
			
					if( $(this).attr( 'ip' ) != null ) {
						cur_drv_name += ' [' + $(this).attr( 'ip' ) + ':' + $(this).attr( 'port' ) + ']';
					}

					var sel = "";
					if( driver_name == cur_drv_name ) sel = "SELECTED";
	
					content += '<option value="' + cur_drv_name + '" ' + sel + '>';
					content += cur_drv_name;
					content += '</option>';
				} );
		}
	} );

	content += '</select>';

	content += '<p>Nom capteur :</p>';
	content += '<input type="text" id="name" name="name" value="Nouveau capteur" class="text" />';

	content += "<p>Identifiant :</p>";
	content += '<input type="text" id="new_id" name="new_id" class="text" />';

	content += '<p>Etat : </p>';
	content += '<select id="state" name="state" class="text">';
				
	content += '<option value="OFF">OFF</option>';
	content += '<option value="ON">ON</option>';

	content += '</select>';

	content += '</form>';

	content += "</div>";

	$("#dialog").html( content );

	/* Et on affiche la dialog */
	if( show_dialog == true ) {

		$("#dialog").dialog( {
			title: dialog_title,
			width: 450,
			resizable: false,
			buttons: { "Enregistrer": function() { if( save_device_info() == 1 ) $(this).dialog( "close" ); },
				   "Annuler": function() { $(this).dialog( "close" ); },
				 }
		} );
	}
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
		error: function( j, e, t ) {
			alert( "Une erreur est survenue lors de l'envoi de la requête de suppression au serveur.\nVeuillez réessayer plus tard.\n" + e + " - " + t + "\n" + j.responseText );
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
			
			var str = "";

			$( data ).find( 'driver' ).each( function() {
				
				str += '<div class="driver_list">';

				str += '<a href="#" class="collapse expanded"><img src="img/collapse.gif" /></a>';
				str += '<span class="driver_name">';

				/* Formattage du nom du driver en fonction de s'il y a une IP ou pas */
				str += $(this).attr( 'so' );
				if( $(this).attr( 'ip' ) != null ) {
					str += ' [' + $(this).attr( 'ip' ) + ':' + $(this).attr( 'port' ) + ']';
				}
				str += "</span>";

				str += '<a href="#" class="tiny_link add" alt="';

				/* Formattage du nom du driver en fonction de s'il y a une IP ou pas */
				str += $(this).attr( 'so' );
				if( $(this).attr( 'ip' ) != null ) {
					str += ' [' + $(this).attr( 'ip' ) + ':' + $(this).attr( 'port' ) + ']';
				}
				str += '"><span><img src="img/button/add.png" />Ajouter capteur</span></a>';
				str += '<div class="device_list">';

				$(this).find( 'capteur' ).each( function() {
					id = $(this).attr( 'id' );

					str += '<div class="row">';
					str += '<div class="cell">';
					str += '<span>' + $(this).text() + '</span>'
					str += '</div><div class="cell">';
					str += '<a href="#" class="tiny_link view" alt="' + id + '"><span><img src="img/button/view.png" class="tiny_btn"/>Voir</span></a>'
					str += '<a href="#" class="tiny_link edit" alt="' + id + '"><span><img src="img/button/edit.png" class="tiny_btn"/>Editer</span></a>'
					str += '<a href="#" class="tiny_link del"  alt="' + id + '"><span><img src="img/button/delete.png" class="tiny_btn"/>Supprimer</span></a>'
					str += '</div>';
					str += '</div>';
				} );

				str += '</div>';
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
