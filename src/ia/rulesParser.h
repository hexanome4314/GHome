#ifndef RULESPARSER_H
#define RULESPARSER_H
#include <stdio.h>
#include <stdlib.h>
#include "XMLParser.h"
#include "engine.h"
#include "engine-types.h"

#define FIELDS_SIZE 13
char* Fields[FIELDS_SIZE] = {"Button_1", "Button_2", "Button_3", "Button_4", "Button_5", "Button_6", "Button_7", "Button_8", "Temperature", "Humidity", "Lighting", "Voltage", "Unknown_Field"};

#define RECIPIENTS_SIZE 2
char* Recipients[RECIPIENTS_SIZE] = {"Web", "Unknown_Recipient"};

#define COMPARISONS_SIZE 4
char* Comparisons[COMPARISONS_SIZE] = {"Equ", "Sup", "Inf", "Unknown_Comparison"};

Rule * get_rules(char *filename);

#endif /* RULESPARSER_H */
