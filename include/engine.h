#ifndef __ENGINE_H_
#define __ENGINE_H_

#include <stdio.h>
#include "memory.h"
#include "ios_api.h"

enum Condition_type
{
	Equ,
	Sup,
	Inf
};

typedef struct Action Action;
struct Action
{
	int device;
	int field;
	char state;
	Action* next;
};

typedef struct Alert Alert;
struct Alert
{
	int recipient;
	char* message;
	Alert* next;
};

typedef struct Condition Condition;
struct Condition
{
	int device; // id capteur
	int field;  // id sonde
	enum Condition_type type;
	float value;
	Condition* next;
};

typedef struct Rule Rule;
struct Rule {
	char* name;
	Action* actions;
	Alert*  alerts;
	Condition*  conditions;
	Rule* next;
};

Rule* rules;

/**
 * Lancer la gestion des règles à partir d'un fichier de règles
 * \param file Le fichier de règle
 * \return 0 si tout va bien, négatif sinon
 */
int launch_engine(const char *file);

/**
 * Relit les règles à partir du fichier xml
 * \return 0 si tout va bien, négatif sinon
 */
int reload_rules();

#endif
