function update_driver_data() {

	$.ajax( {
                type: "GET",
                dataType: "xml",
                url: "data/drivers.xml",
                success: function( data ) {
			
			str = "";

			$( data ).find( 'driver' ).each( function() {
				
				str += '<a href="#"><img src="img/collapse.gif" /></a>';
				str += '<span class="driver_name">' + $(this).attr( 'so' ) + "</span>";
				str += '<div class="device_list">';

				$(this).find( 'capteur' ).each( function() {
					str += '<span>' + $(this).text() + '</span>'
					str += '<a href="#"><img src="img/trash.png" /></a>'
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
