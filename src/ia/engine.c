#include "engine.h"

void apply_action(Action* action)
{
	ios_write(action->device, &(action->state));
}

/**
   Alerte l'utilisateur
   \param: alert	l'alerte à lancer
 */
void apply_alerte(Alert* alert)
{
	printf("Alerte : %s", alert->message);
}

/**
Vérifie si une condition est vérifiée
\param: condition	la condition à vérifier
\return 1 si la condition est vérifiée
*/
char check_condition(Condition* condition)
{
	char value;
	int status = ios_read(condition->device, condition->field, &value);
	switch(condition->type)
	{
	case Equ:
		return (value == condition->value);
	case Sup:
		return (value > condition->value);
	case Inf:
		return (value < condition->value);
	}
	return 0;
}

/**
Vérifie si toutes les conditions sont vérifiées
\param: condition	premiere condition de la liste
\return 1 si toutes les conditions sont vérifiées
 */
char check_conditions(Condition* condition)
{
	while(condition)
	{
		if (!check_condition(condition))
			return 0;
		condition = condition->next;
	}
	return 1;
}

/**
Lit un fichier xml et rempli la structure de règle
\return -1 si il y a un problème
*/
int load_xml()
{
/*	Action act1;
	act1.device = 1;
	act1.state = 0;
	act1.next = NULL;

	Alert alert1;
	alert1.recipient = 1;
	alert1.message = "message de test";
	alert1.next = NULL;

	Condition cond1;
	cond1.device = 1;
	cond1.field = 12;
	cond1.type = Equ;
	cond1.value = */
	return 0;
}

/**
Parcours l'ensemble des règles et applique les actions associées de chacune si les conditions sont vérifiées
 */
void apply_actions()
{
	Rule* rule = rules;
	while(rule->next)
	{
		if (check_conditions(rule->conditions))
		{
			Action* action = rule->actions;
			while(action)
			{
				apply_action(action);
				action = action->next;
			}
		}
		rule = rule->next;
	}
}
