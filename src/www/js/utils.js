function show_notifier_box( icon, message ) {

	$("#notifier_box").html( "<img src=\"img/" + icon + "\"> <span>" + message + "</span>" );
        $("#notifier_box").slideDown().delay(2000).slideUp();
}
