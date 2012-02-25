/*
Défini les champs utilisables pour chaque capteurs
*/

#ifndef __FIELDS_H__
#define __FIELDS_H__

#define FIELDS \
	X(DRV_FIELD_BUTTON1, "Button_1")			\
	X(DRV_FIELD_BUTTON2, "Button_2")			\
	X(DRV_FIELD_BUTTON3, "Button_3")			\
	X(DRV_FIELD_BUTTON4, "Button_4")			\
	X(DRV_FIELD_BUTTON5, "Button_5")			\
	X(DRV_FIELD_BUTTON6, "Button_6")			\
	X(DRV_FIELD_BUTTON7, "Button_7")			\
	X(DRV_FIELD_BUTTON8, "Button_8")			\
	X(DRV_FIELD_TEMPERATURE, "Temperature")			\
	X(DRV_FIELD_HUMIDITY, "Humidity")			\
	X(DRV_FIELD_LIGHTING, "Lighting")			\
	X(DRV_FIELD_VOLTAGE, "Voltage")				\
	X(DRV_LAST_VALUE, "Unknown_Field")

/*
Définition des macros
*/
enum Fields_defined {
#define X(define, str) define,
FIELDS
#undef X
};

#endif
