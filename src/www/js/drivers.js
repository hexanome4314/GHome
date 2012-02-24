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
Lecture du XML concernant les périphériques et les drivers
*/
function update_driver_data() {

	$.ajax( {
                type: "GET",
                dataType: "xml",
                url: "data/drivers.xml",
                success: function( data ) {
			
			str = "";

			$( data ).find( 'driver' ).each( function() {
				
				str += '<a href="#" class="collapse expanded"><img src="img/collapse.gif" /></a>';
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
