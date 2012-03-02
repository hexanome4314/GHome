#ifndef __ENGINE_TYPES_H
#define __ENGINE_TYPES_H

// Les types de comparaison
enum Condition_type
{
	Equ,
	Sup,
	Inf
};

// Un envoi à un capteur
typedef struct Action Action;
struct Action
{
	int device;
	int field;
	char state;
	Action* next;
};

// Une alerte à donner
typedef struct Alert Alert;
struct Alert
{
	int recipient;
	char* message;
	Alert* next;
};

// Une condition à respecter
typedef struct Condition Condition;
struct Condition
{
	int device; // id capteur
	int field;  // id sonde
	enum Condition_type type;
	float value;
	Condition* next;
};

// Une règle (ensemble d'actions, d'alertes
// de conditions)
typedef struct Rule Rule;
struct Rule {
	char* name;
	Action* actions;
	Alert*  alerts;
	Condition*  conditions;
	Rule* next;
};

#endif /*__ENGINE_TYPES_H */
