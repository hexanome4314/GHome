function update_sensor_data( hide_notif ) {
        /* On raffraichit les données */
	$.ajax( {
        	type: "GET",
                dataType: "json",
                url: "data/raw_data.json",
                success: function( data ) {

                        // Pour chaque élément du tableau, on va ajouter une nouvelle entrée
                        html_stuff = "";
                        for( var i = 0; i < data.raw_data.length; i++ ) {

                                html_stuff += '<span class="sensor_name">' + data.raw_data[i].name + '</span>';
                                html_stuff += '<div class="table">';

                                if( 'temperature' in data.raw_data[i] ) {
                                        html_stuff += '<div class="row">';
                                        html_stuff += '<img src="img/temp.jpg" class="sensor_img" />';
                                        html_stuff += '<span class="val">Temp&eacute;rature : ';
                                        html_stuff += '<i>' + data.raw_data[i].temperature + '</i>&deg;C</span>';
                                        html_stuff += '</div>';
                                }
				if( 'humidity' in data.raw_data[i] ) {
                                        html_stuff += '<div class="row">';
                                        html_stuff += '<img src="img/humidity.gif" class="sensor_img" />';
                                        html_stuff += '<span class="val">Humidit&eacute; : ';
                                        html_stuff += '<i>' + data.raw_data[i].humidity + '</i>%</span>';
                                        html_stuff += '</div>';
                                 }
                                if( 'brightness' in data.raw_data[i] ) {
                                        html_stuff += '<div class="row">';
                                        html_stuff += '<img src="img/light.jpg" class="sensor_img" />';
                                        html_stuff += '<span class="val">Luminosit&eacute; : ';
                                        html_stuff += '<i>' + data.raw_data[i].brightness + '</i>%</span>';
                                        html_stuff += '</div>';
                                }
                                if( 'voltage' in data.raw_data[i] ) {
                                        html_stuff += '<div class="row">';
                                        html_stuff += '<img src="img/voltage.jpg" class="sensor_img" />';
                                        html_stuff += '<span class="val">Tension : ';
                                        html_stuff += '<i>' + data.raw_data[i].voltage + '</i>V</span>';
                                        html_stuff += '</div>';
                                }

                                html_stuff += '</div>';
                        }

                        $("#sensor_data").html( html_stuff );
                                 
			if( hide_notif == false )       
	                        show_notifier_box( 'refresh.png', 'Donn&eacute;es actualis&eacute;es.' );
                },
		error: function( ) {
                                        
			if( hide_notif == false )
	                        show_notifier_box( 'erreur.png', "Erreur lors de l'actualisation des donn&eacute;es." );
                }
        } );
}

