function update_rules_data() {

	$.ajax( {
                type: "GET",
                dataType: "xml",
                url: "data/rules.xml",
                success: function( data ) {
			
			str = "";

			$( data ).find( 'rule' ).each( function() {
				
				str += '<a href="#"><img src="img/collapse.gif" /></a>';
				str += '<span class="rule_name">' + $(this).attr( 'name' ) + "</span>";
				str += '<br />';
				str += '<span class="rule_cond">Si</span>';
				str += '<div class="device_list">';

				$(this).find( 'condition' ).each( function() {
					str += '<span>';
					str += $(this).attr( 'device' ) + '.' + $(this).attr( 'field' );
	
					if( $(this).attr( 'type' ) == 'sup' )
						str += ' > ';
					else if( $(this).attr( 'type' ) == 'equ' )
						str += ' = ';
					else if( $(this).attr( 'type' ) == 'inf' )
						str += ' < ';
					else
						str += ' ? ';

					str += $(this).text();

					str += '</span>'
					str += '<a href="#"><img src="img/trash.png" /></a>'
					str += '<br />';
				} );

				str += '</div>';

				str += '<span class="rule_cond">Alors</span>';
				str += '<div class="device_list">';
				$(this).find( 'activate' ).each( function() {
					str += '<span>';
					str += 'Set ' + $(this).attr( 'device' ) + '.' + $(this).attr( 'field' ) + ' = ' + $(this).text();
					str += '</span>';
					str += '<br />';
				} );
				$(this).find( 'alert' ).each( function() {
					str += '<span>';
					str += 'Alert( ' + $(this).attr( 'recipient' ) + ', ' + $(this).text() + ' )';
					str += '</span>';
				} );
				str += '</div>';
				
			} );

			$( '#rules_data' ).html( str );
		},
		error: function( xhr, str, error) {
			alert( "Impossible de charger les données relatives aux r&egrave;gles." );

			$( '#rules_data' ).html( '<img src="img/error.png" /><span><i>Impossible de charger les donn&eacute;es relatives aux r&egrave;gles.</span>' );
		}
	} );
}
