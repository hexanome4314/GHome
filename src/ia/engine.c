#include "engine.h"
#include "engine-types.h"
#include "parser.h"
#define OVERRIDE_STDLIB

Rule* rules;
/**
 * Execute une action passée en paramêtre (écrit la valeur
 * correspondante sur le capteur)
 * \param action Action à exécuter
 */
void apply_action(Action* action)
{
	ios_write(action->device, action->state);
}

/**
 * Alerte l'utilisateur
 * \param alert l'alerte à lancer
 */
void apply_alert(Alert* alert)
{
	printf("Alerte : %s\n", alert->message);
}

/**
 * Vérifie si une condition est vérifiée
 * \param condition condition à vérifier
 * \return 1 si la condition est vérifiée
 */
char check_condition(Condition* condition)
{
	float value;
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
 * Vérifie si toutes les conditions concernant un capteur sont
 * vérifiées et que les nouvelles conditions concernent le capteur
 * \param condition premiere condition de la liste
 * \param field champ modifié du capteur ayant envoyé une information
 * \return 1 si toutes les conditions sont vérifiées
 */
char check_conditions(Condition* condition, unsigned int device, unsigned int field)
{
	char useful = 0; // Passe à vrai si au moins une condition concernait le capteur
	while(condition)
	{
		if (!check_condition(condition))
			return 0; // Retourne faux si une condition n'est pas vérifiée
		if(condition->field == field && condition->device == device)
			useful = 1;
		condition = condition->next;
	}
	return useful; /* Retourne faux si toutes les conditions sont vraies mais
			  qu'aucune condition ne concernait le changement */
}

/**
 * Lancer la gestion des règles à partir d'un fichier de règles
 * \param file Le fichier de règle
 * \return 0 si tout va bien, négatif sinon
 */
int launch_engine(const char *file)
{
	rules = get_rules(file);
	return 0;
}

/**
 * Relit les règles à partir du fichier xml
 * \return 0 si tout va bien, négatif sinon
 */
int reload_rules()
{
	return 0;
}

/**
 * Lit un fichier xml et rempli la structure de règle
 * \return -1 si il y a un problème
 */
void load_xml()
{
	Action* act1 = malloc(sizeof(Action));
	act1->field = 2;
	act1->device = 1;
	act1->state = 0;
	act1->next = NULL;

	Alert* alert1 = malloc(sizeof(Alert));
	alert1->recipient = 1;
	alert1->message = "message de test";
	alert1->next = NULL;

	Condition* cond1 = malloc(sizeof(Condition));
	cond1->device = 0;
	cond1->field = 2;
	cond1->type = Sup;
	cond1->value = 400;

	Rule* rule = malloc(sizeof(Rule));
	rule->name = "La premiere";
	rule->actions = act1;
	rule->alerts = alert1;
	rule->conditions = cond1;
	rule->next = NULL;
	
	rules = rule;
	return;
}

/**
 * Parcours l'ensemble des règles et applique les actions associées de
 * chacune si les conditions sont vérifiées TODO : prendre en compte
 * le champ modifié pour ne pas tout reparcourir
 * \param device file descriptor du capteur ayant envoyé une info
 * \param field le champ modifié
 * \param val la nouvelle valeur
 */
void apply_actions(unsigned int device, unsigned int field, float val)
{
	Rule* rule = rules;
	while(rule)
	{
		if (check_conditions(rule->conditions, device, field))
		{
			Action* action = rule->actions;
			Alert* alert = rule->alerts;
			while(action)
			{
				apply_action(action);
				action = action->next;
			}
			while (alert)
			{
				apply_alert(alert);
				alert = alert->next;
			}
		}
		rule = rule->next;
	}
}
