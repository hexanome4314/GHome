/**
Conversion de la forme xml d'une règle à une forme texte
\param	node	Le noeud de la règle
\return	Le texte correspondant
*/
function xml_to_rule_txt( node ) {

	var str = "";

	str += '<p>Condition</p>';
	str += '<div>';

	node.find( 'condition' ).each( function() {
		str += '<p class="value">&bull; ';
		str += 'sensor( '+ $(this).attr( 'device' ) + ' ).' + $(this).attr( 'field' );

		if( $(this).attr( 'type' ) == 'sup' )
			str += ' > ';
		else if( $(this).attr( 'type' ) == 'equ' )
			str += ' = ';
		else if( $(this).attr( 'type' ) == 'inf' )
			str += ' < ';
		else
			str += ' ? ';

		str += $(this).text();

		str += '</p>'
	} );

	str += '</div>';

	str += '<p>Alors</p>';
	str += '<div>';
	node.find( 'activate' ).each( function() {
		str += '<p class="value">&bull; ';
		str += 'Set sensor( ' + $(this).attr( 'device' ) + ' ).' + $(this).attr( 'field' ) + ' = ' + $(this).text();
		str += '</p>';
	} );
	node.find( 'alert' ).each( function() {
		str += '<p class="value">&bull; ';
		str += 'Write( ' + $(this).attr( 'recipient' ) + ', "' + $(this).text() + '" )';
		str += '</p>';
	} );

	return str;
}

/**
Affiche les informations relatives à une règle
\param  rule	Le nom de la règle
*/
function show_rule_info( rule ) {

        /* On commence par récupérer dans le xml les dernières info que l'on a de la règle */
        $.ajax( {
                async: false,
                type: "GET",
                dataType: "xml",
                url: "data/rules.xml",
                success: function( data ) {

                        /* Avec XPath, on choppe le bon noeud */
                        var node = $(data).find( "rule[name='" + rule + "']" );

			if( node != null && node.length > 0 ) {
				if( node.length == 1 ) {
					
					/* Préparation de la dialog */
					var content = '<div class="content">';

					content += "<h3>" + rule + "</h3>";

					content += xml_to_rule_txt( node );

					$("#dialog").html( content );

					/* Et on affiche la dialog */
					$("#dialog").dialog( {
						title: 'Voir une règle',
						width: 500,
						show: 'drop',
						hide: 'drop',
						resizable: false,
						buttons: { "Ok": function() { $(this).dialog( "close" ); }/*,
							   "Modifier": function() { $(this).dialog( "close" ); edit_rule_info( rule ); },
							   "Supprimer": function() { $(this).dialog( "close" ); confirm_remove_rule( rule ); }*/
							 }
					});
				}
				else {
					alert( 'Le fichier XML contenant les règles est incohérent. Impossible de poursuivre' );
				}
			}
			else {
				alert( 'La règle semble ne plus exister.' );
				update_rules_data();
			}
		},
		error: function( x, t, e ) {
			alert( 'Une erreur est survenue lors de l\'envoi de la requête au serveur. Veuillez retenter plus tard.' );
		}
	});
}

/**
Permet l'édition d'une règle
\param	rule	Le nom de la règle à modifier
*/
function edit_rule_info( rule ) {

        /* On commence par récupérer dans le xml les dernières info que l'on a de la règle */
        $.ajax( {
                async: false,
                type: "GET",
                dataType: "xml",
                url: "data/rules.xml",
                success: function( data ) {

                        /* Avec XPath, on choppe le bon noeud */
                        var node = $(data).find( "rule[name='" + rule + "']" );

			if( node != null && node.length > 0 ) {
				if( node.length == 1 ) {
					
					/* Préparation de la dialog */
					var content = '<div class="content">';

					content += "<h3>&Eacute;diter une règle</h3>";

					content += '<form class="edit_rule">';

					content += '<p>Nom :</p>';
					content += '<input type="text" name="name" value="' + rule + '" class="text" />';

					/*content += '<p>Menu :</p>';
					content += '<div id="menu">';
					content += '<button id="capteurs">Capteurs</button>';
					content += '<button id="attributs">Attributs</button>';
					content += '<button id="ops">Opérateurs</button>';
					content += '<button id="funcs">Fonctions</button>';
					content += '</div>';*/

					content += '<p>Conditions</p>';

					node.find( 'conditions' ).find( 'condition' ).each( function() {
						var cond = 'device="' + $(this).attr( 'device' ) + '" field="' + $(this).attr( 'field' ) + '" type="' + $(this).attr( 'type' ) + '" val="' + $(this).text() + '"';

						content += '<input type="text" name="cond" value=\'' + cond + '\' class="text" size="55" />';
						content += '</br>';
					} );

					content += '<a href="#" class="tiny_link"><span><img src="img/button/add.png" />Add</span></a>';


					content += '<p>Actions</p>';
					node.find( 'actions' ).find( 'activate' ).each( function() {
						var actions = 'device="' + $(this).attr( 'device' ) + '" field="' + $(this).attr( 'field' ) + '" val="' + $(this).text() + '"';

						content += '<input type="text" name="actions" value=\'' + actions + '\' class="text" size="55" />';
						content += '</br>';
					} );

					content += '</form>';

					content += '</div>';

					$("#dialog").html( content );

					$("#menu button").button( {
						icons: { primary: "ui-icon-triangle-1-s" }
					} );


					/* Et on affiche la dialog */
					$("#dialog").dialog( {
						title: 'Editer une règle',
						width: 600,
						show: 'drop',
						hide: 'drop',
						resizable: false,
						buttons: { "Ok": function() { $(this).dialog( "close" ); },
							   "Modifier": function() { $(this).dialog( "close" ); edit_rule_info( rule ); }
							 }
					});
				}
				else {
					alert( 'Le fichier XML contenant les règles est incohérent. Impossible de poursuivre' );
				}
			}
			else {
				alert( 'La règle semble ne plus exister.' );
				update_rules_data();
			}
		},
		error: function( x, t, e ) {
			alert( 'Une erreur est survenue lors de l\'envoi de la requête au serveur. Veuillez retenter plus tard.' );
		}
	});
}

/**
Permet l'ajout d'une règle
*/
function new_rule() {

	/* Préparation de la dialog */
	var content = '<div class="content">';

	content += "<h3>Ajouter une nouvelle règle</h3>";

	content += '<form action="ajax/upload.php" class="edit_rule" method="post" enctype="multipart/form-data">';

	content += '<p>Nom :</p>';
	content += '<input type="text" name="name" class="text" />';

	/*content += '<p>Menu :</p>';
	content += '<div id="menu">';
	content += '<button id="capteurs">Capteurs</button>';
	content += '<button id="attributs">Attributs</button>';
	content += '<button id="ops">Opérateurs</button>';
	content += '<button id="funcs">Fonctions</button>';
	content += '</div>';*/

	content += '<input type="file" id="file" name="file" class="text" />';

	content += '<input type="submit" value="Ok" />';

	content += '</form>';

	content += '</div>';

	$("#dialog").html( content ); 

	$("#dialog form").iframePostForm ( {
		json: true,
		post: function() {
			alert( 'p' );
		},
		error: function( x, t, e ) {
			alert( t + ' - ' + e );
		},
		complete: function() {
			alert( 'ok' );
		}
	} );

	/* Et on affiche la dialog */
	$("#dialog").dialog( {
		title: 'Ajouter une règle',
		width: 600,
		show: 'drop',
		hide: 'drop',
		resizable: false,
		buttons: { "Ok": function() { $(this).dialog( "close" ); },
			   "Modifier": function() { 
					/*$.ajaxFileUpload( {
						url:'/contents/ajaxfileupload/doajaxfileupload.php',
						fileElementId:'fileToUpload',
						dataType: 'json',
						success: function (data, status)
						{
							if(typeof(data.error) != 'undefined')
							{
								if(data.error != '')
								{
									alert(data.error);
								}else
								{
									alert(data.msg);
								}
							}
						},
						error: function (data, status, e)
						{
							alert(e);
						}
					} );*/
				}
			 }
	});
}

/**
Demande à l'utilisateur de confirmer sa volonté de supprimer une règle
\param	rule	Le nom de la règle à supprimer
*/
function confirm_remove_rule( rule ) {

        var content = '<div class="content">';
        content += '<h3>Suppression d\'une règle</h3>';
        content += '<img src="img/button/delete.png" />';
        content += '<p>&Ecirc;tes-vous vraiment s&ucirc;r de vouloir supprimer la règle "' + rule + '" ?</p>';
        content += '</div>';

        $("#dialog").html( content );

        $("#dialog").dialog( {
                title: 'Confirmation',
                width: 400,
                resizable: false,
                show: 'drop',
                hide: 'drop',
                buttons: { "Oui": function() { $(this).dialog( "close" ); remove_rule( rule ); },
                           "Non": function() { $(this).dialog( "close" ); }
                        }
        } );

}

/**
Supprime une règle
\param	rule	Le nom de la règle
*/
function remove_rule( rule ) {

	/* On poste la requête */
        $.ajax( {
                async: false,
                type: "POST",
                dataType: "json",
                url: "ajax/rules.php",
                data: "action=delete&rule="+rule,
                success: function( data ) {

                        if( data.type != "success" ) {
                                alert( data.mesg );
                        }
                        else {
                                update_rules_data();
                        }
                },
                error: function( j, e, t ) {
                        alert( "Une erreur est survenue lors de l'envoi de la requête de suppression au serveur.\nVeuillez réessayer plus tard.\n" + e + " - " + t + "\n" + j.responseText );
                }
        });

}

/**
Raffraichit les informations liées aux règles
*/
function update_rules_data() {

$.ajax( {
	type: "GET",
	dataType: "xml",
	url: "data/rules.xml",
	success: function( data ) {
		
		str = "";

		/*str += '<a href="#" class="tiny_link add_rule">';
		str += '<span><img src="img/button/add.png" />Ajouter une nouvelle r&egrave;gle</span>';
		str += '</a><br /><br />';*/

		str += '<div class="rules_list">';

		$( data ).find( 'rule' ).each( function() {
			
			str += '<div class="row"><div class="cell">';

			str += '<span class="rule_name">&bull; ' + $(this).attr( 'name' ) + "</span>";

			str += '</div><div class="cell">';

			str += '<a href="#" class="tiny_link view" rule="' + $(this).attr( 'name' ) + '" >';
			str += '<span><img src="img/button/view.png" />Voir</span></a>';
			/*str += '<a href="#" class="tiny_link edit" rule="' + $(this).attr( 'name' ) + '" >';
			str += '<span><img src="img/button/edit.png" />&Eacute;diter</span></a>';*/
			str += '<a href="#" class="tiny_link del" rule="' + $(this).attr( 'name' ) + '" >';
			str += '<span><img src="img/button/delete.png" />Supprimer</span></a>';

			str += '</div></div>';
		} );

		str += '</div>';

		$( '#rules_data' ).html( str );


		$( 'div#rules_data .add_rule' ).click( function() {
			new_rule();
			return false;
		} );

		$( 'div#rules_data .view' ).click( function() {
			show_rule_info( $(this).attr( 'rule' ) );
			return false;
		});

		$( 'div#rules_data .edit' ).click( function() {
			edit_rule_info( $(this).attr( 'rule' ) );
			return false;
		});

		$( 'div#rules_data .del' ).click( function() {
			confirm_remove_rule( $(this).attr( 'rule' ) );
			return false;
		});
	},
	error: function( xhr, str, error) {
		alert( "Impossible de charger les données relatives aux r&egrave;gles." );

		$( '#rules_data' ).html( '<img src="img/error.png" /><span><i>Impossible de charger les donn&eacute;es relatives aux r&egrave;gles.</span>' );
	}
} );
}
