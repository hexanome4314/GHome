#ifndef __ENGINE_H_
#define __ENGINE_H_

#include <stdio.h>
#include "../../include/ios_api.h"

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
//	int field;
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
	char value;
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

#endif
