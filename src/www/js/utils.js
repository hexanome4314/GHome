/**
Affiche une notification en haut de la page
\param 	icon URL vers l'image à afficher
	message Contenu de la notification
*/
function show_notifier_box( icon, message ) {

	$("#notifier_box").html( "<img src=\"img/" + icon + "\"> <span>" + message + "</span>" );
        $("#notifier_box").slideDown().delay(2000).slideUp();
}
