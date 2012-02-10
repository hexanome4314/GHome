#ifndef __ENGINE_TYPES_H
#define __ENGINE_TYPES_H

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

#endif /*__ENGINE_TYPES_H */
