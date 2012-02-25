/*
Défini les champs utilisables pour chaque capteurs
*/

#ifndef __FIELDS_H__
#define __FIELDS_H__

#define FIELDS \
	X(DRV_FIELD_BUTTON1, "button_1")			\
	X(DRV_FIELD_BUTTON2, "button_2")			\
	X(DRV_FIELD_BUTTON3, "button_3")			\
	X(DRV_FIELD_BUTTON4, "button_4")			\
	X(DRV_FIELD_BUTTON5, "button_5")			\
	X(DRV_FIELD_BUTTON6, "button_6")			\
	X(DRV_FIELD_BUTTON7, "button_7")			\
	X(DRV_FIELD_BUTTON8, "button_8")			\
	X(DRV_FIELD_TEMPERATURE, "temperature")			\
	X(DRV_FIELD_HUMIDITY, "humidity")			\
	X(DRV_FIELD_LIGHTING, "lighting")			\
	X(DRV_FIELD_VOLTAGE, "voltage")				\
	X(DRV_LAST_VALUE, "unknown_Field")

/*
Définition des macros
*/
enum Fields_defined {
#define X(define, str) define,
FIELDS
#undef X
};

#endif
