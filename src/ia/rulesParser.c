#include "rulesParser.h"

// Association id champ => nom
static char* Fields[] = {
#define X(define, str) str,
FIELDS
#undef X
};

#define FIELDS_SIZE DRV_LAST_VALUE

// Activation des printf pour débogage (=1)
#define DEBUG_MODE 0

// Liste des "recipients" connus = entités à alerter
#define RECIPIENTS_SIZE 2
static char* Recipients[RECIPIENTS_SIZE] = {"Web", "Unknown_Recipient"};

// Liste des opérations de comparaisons connues
#define COMPARISONS_SIZE 4
static char* Comparisons[COMPARISONS_SIZE] = {"Equ", "Sup", "Inf", "Unknown_Comparison"};

// Pointeur sur la dernière règle éditée (optimisation)
static Rule* lastRule = NULL;

/** Liste de capteur fournie par core permettant d'associer un fd aux noms */
static infos_sensor *sensor;

/**
 * Donne le file descriptor d'un capteur installé à partir de son id
 * Si le capteur n'est pas installé retourne -1
 * \param id identifiant du capteur
 * \return file descriptor du capteur identifié ou -1 si le capteur n'est pas installé
 */
int get_fd(int id){
	int loop_counter = 0;
	while(id != MAX_NUMBER_OF_SENSORS && sensor[loop_counter].id == id){
		loop_counter++;
	}
	if(sensor[loop_counter].id == id)
		return sensor[loop_counter].fd;
	else
		return -1;
}

/**
 * Donne le file descriptor d'un capteur installé à partir de son nom
 * \param name le nom du capteur
 * \return le fd ou -1 si le capteur n'est pas installé
 */
int get_fd_by_name(char* name){
	int i = 0;
	for(i=0; i<MAX_NUMBER_OF_SENSORS; i++)
	{
		if(sensor[i].name && strcmp((char *)sensor[i].name, name) == 0)
			return sensor[i].fd;
	}
	return -1;
}

/**
 * Traduit un Condition_type en chaine affichable et compréhensible
 * \param type le Condition_type à traduire
 * \return la chaîne affichable et compréhensible
 */
char * getConditionType(enum Condition_type type)
{
	if(type < COMPARISONS_SIZE)
	{
		return Comparisons[type];
	}
	else
	{
		return Comparisons[COMPARISONS_SIZE-1];
	}
}

/**
 * Traduit une chaine en son Condition_type équivalent
 * \param type la chaine à traduire
 * \return le Condition_type équivalent
 */
int getConditionTypeIndex(char *type)
{
	int i = 0;

	for(; i < COMPARISONS_SIZE && compare(Comparisons[i], type) == -1; i++);

	return i;
}

/**
 * Retourne l'identifiant d'un device à partir de son nom
 * \param device le nom du device
 * \return le file descriptor du device
 */
int getDeviceIndex(char *device) {
	return get_fd_by_name(device);
}

/**
 * Retourne le nom affichable d'un device
 * \param device le file descriptor du device
 * \return le nom du device
 */
char * getDeviceName(int device)
{
	int i=0;
	for(i=0; i<MAX_NUMBER_OF_SENSORS; i++)
	{
		if(sensor[i].name && (sensor[i].fd == device))
			return (char*)sensor[i].name;
	}
	return "DEVICE";
}

/**
 * Traduit un état booléen d'une chaine vers un char ('t'="true", sinon 'f')
 * \param state état booléen en chaine
 * \return état booléen en caractère équivalent
 */
char getState(char *state) {
	if(compare("True", state) == 0)
	{
		return 't';
	}
	else
	{
		return 'f';
	}
}

/**
 * Traduit un état booléen d'un caractère vers une chaine ('t'="true", sinon 'f')
 * \param state état booléen en caractère
 * \return état booléen en chaine équivalente
 */
char * getStateName(char state)
{
	if(state == 't')
	{
		return "True";
	}
	else
	{
		return "False";
	}
}

/**
 * Traduit un Recipient à partir de son index en chaine affichable et compréhensible
 * \param recipient index (enum) du recipient
 * \return nom du recipient affichable et compréhensible
 */
char * getRecipientName(int recipient) {
	if(recipient < RECIPIENTS_SIZE)
	{
		return Recipients[recipient];
	}
	else
	{
		return Recipients[RECIPIENTS_SIZE-1];
	}
}

/**
 * Retourne l'index d'un recipient à partir de son nom
 * \param recipient nom du recipient
 * \return index du recipient (enum)
 */
int getRecipientIndex(char *recipient) {
	int i = 0;

	for(; i < RECIPIENTS_SIZE && compare(Recipients[i], recipient) == -1; i++);

	return i;
}

/**
 * Retourne l'index d'un Field à partir de son nom
 * \param field nom du field
 * \return index du field (enum)
 */
int getFieldIndex(char *field) {
	int i = 0;

	for(; i < FIELDS_SIZE && compare(Fields[i], field) == -1; i++);

	return i;
}

/**
 * Traduit un field à partir de son index en chaine affichable et compréhensible
 * \param field index (enum) du field
 * \return nom du field affichable et compréhensible
 */
char * getFieldName(int field)
{
	if(field < FIELDS_SIZE)
	{
		return Fields[field];
	}
	else
	{
		return Fields[FIELDS_SIZE-1];
	}
}

/**
 * Retourne la dernière action (dans la liste chainée) de la règle donnée
 * \param rule règle dont la dernière action est à chercher
 * \return pointeur sur la dernière action de la règle
 */
Action * getLastAction(Rule *rule)
{
	Action *a, *res = rule->actions;

	for(a = rule->actions; a != NULL; a = a->next)
	{
		res = a;
	}

	return res;
}

/**
 * Retourne la dernière alerte (dans la liste chainée) de la règle donnée
 * \param rule règle dont la dernière alerte est à chercher
 * \return pointeur sur la dernière alerte de la règle
 */
Alert * getLastAlert(Rule *rule)
{
	Alert *a, *res = rule->alerts;

	for(a = rule->alerts; a != NULL; a = a->next)
	{
		res = a;
	}

	return res;
}

/**
 * Retourne la dernière condition (dans la liste chainée) de la règle donnée
 * \param rule règle dont la dernière condition est à chercher
 * \return pointeur sur la dernière condition de la règle
 */
Condition * getLastCondition(Rule *rule)
{
	Condition *c, *res = rule->conditions;

	for(c = rule->conditions; c!= NULL; c = c->next)
	{
		res = c;
	}

	return res;
}

/**
 * Ajoute un règle à la règle en cours d'édition (variable gloable lastRule)
 * \param name nom de la règle
 */
void addRule(char *name)
{
	Rule *rule = malloc(sizeof(Rule));
	rule->name = name;
	rule->actions = NULL;
	rule->alerts = NULL;
	rule->next = NULL;
	rule->conditions = NULL;
	lastRule->next = rule;
	lastRule = rule;
    
    if(DEBUG_MODE == 1)
    {    
        printf("New rule addedd : %s\n", name);
    }
}

/**
 * Ajoute une action à la règle en cours d'édition (variable gloable lastRule)
 * \param device capteur concerné
 * \param state état du field du device
 * \param field champ du capteur concerné
 */
void addAction(char *device, char *state, char *field) {
	Action *action = malloc(sizeof(Action));
	action->device = getDeviceIndex(device);
	action->state = getState(state);
	action->field = getFieldIndex(field);
	action->next = NULL;
    
	Action *lastAction = getLastAction(lastRule);
    
	if(lastAction == NULL)
	{
		lastRule->actions = action;
	}
	else
	{
		lastAction->next = action;
	}

	if(DEBUG_MODE == 1)
    {
        printf("New action added : %s -> %s\n", device, getStateName(getState(state)));
    }
}

/**
 * Ajoute une alerte à la règle en cours d'édition (variable gloable lastRule)
 * \param recipient destinataire de l'alerte
 * \param message message à envoyer au destinataire
 */
void addAlert(char *recipient, char *message)
{
	Alert *alert = malloc(sizeof(Action));
	alert->recipient = getRecipientIndex(recipient);
	alert->message = message;
	alert->next = NULL;

	Alert *lastAlert = getLastAlert(lastRule);

	if(lastAlert == NULL)
	{
		lastRule->alerts = alert;
	}
	else
	{
		lastAlert->next = alert;
	}
    
    if(DEBUG_MODE == 1)
    { 
        printf("New alert added : %s -> %s\n", recipient, message);
    }
}

/**
 * Ajoute une condition à la règle en cours d'édition (variable gloable lastRule)
 * \param device capteur concerné
 * \param field champ du capteur concerné
 * \param type nature de la comparaison à effectuer (enum Condition_type)
 * \param value valeur à comparer dans la condition (~seuil)
 */
void addCondition(char *device, char *field, char *type, char *value)
{
	Condition *condition = malloc(sizeof(Condition));
	condition->device = getDeviceIndex(device);
	condition->field = getFieldIndex(field);
	condition->type = getConditionTypeIndex(type);
	condition->next = NULL;
    
	if(compare("true", value) == 0)
	{
		condition->value = 1.f;
	}
	else
	{
		condition->value = atof(value);
	}
    
	Condition *lastCondition = getLastCondition(lastRule);

	if(lastCondition == NULL)
	{
		lastRule->conditions = condition;
	}
	else
	{
		lastCondition->next = condition;
	}
    
    if(DEBUG_MODE == 1)
    {
        printf("New condition added : %s->%s %s %f\n", device, field, type, condition->value);
    }
}

/**
 * Switch case sur un noeud permettant de définir sa nature et extraire les 
 * champs utiles afin de les ajouter à la liste chainée de rules
 * \param node le noeud à parser
 */
void fillRules(xmlNodePtr node)
{
	// Ajout d'une regle
	if(strcmp((char *)node->name, "rule") == 0)
	{
		xmlAttrPtr attr = getAttrByName(node, "name");

		if(attr != NULL && attr->children != NULL)
		{
			xmlChar *content = xmlNodeGetContent(attr->children);
			addRule((char *)content);
		}
	}

	// Ajout d'une action
	if(strcmp((char *)node->name, "activate") == 0)
	{
		xmlAttrPtr devicePtr = getAttrByName(node, "device");
		xmlAttrPtr fieldPtr = getAttrByName(node, "field");

		if(node->children != NULL && node->children->type == XML_TEXT_NODE
		   && devicePtr != NULL && devicePtr->children != NULL
		   && fieldPtr !=NULL && fieldPtr->children != NULL)
		{
			xmlChar *state = xmlNodeGetContent(node);
			xmlChar *device = xmlNodeGetContent(devicePtr->children);
			xmlChar *field = xmlNodeGetContent(fieldPtr->children);
			addAction((char *) device, (char *) state, (char *) field);
			xmlFree(state);
			xmlFree(device);
			xmlFree(field);
		}
	}

	// Ajout d'une alerte
	if(strcmp((char *)node->name, "alert") == 0)
	{
		xmlAttrPtr attr = getAttrByName(node, "recipient");

		if(attr != NULL && attr->children != NULL && node->children != NULL && node->children->type == XML_TEXT_NODE)
		{
			xmlChar *message = xmlNodeGetContent(node);
			xmlChar *recipient = xmlNodeGetContent(attr->children);
			addAlert((char *)recipient, (char *) message);
			xmlFree(recipient);
		}
	}

	// Ajout d'une condition
	if(strcmp((char *)node->name, "condition") == 0)
	{
		xmlAttrPtr devicePtr = getAttrByName(node, "device");
		xmlAttrPtr fieldPtr = getAttrByName(node, "field");
		xmlAttrPtr typePtr = getAttrByName(node, "type");

		if(node->children != NULL && node->children->type == XML_TEXT_NODE
		   && devicePtr != NULL && devicePtr->children != NULL
		   && fieldPtr !=NULL && fieldPtr->children != NULL
		   && typePtr !=NULL && typePtr->children != NULL)
		{
			xmlChar *value = xmlNodeGetContent(node);
			xmlChar *device = xmlNodeGetContent(devicePtr->children);
			xmlChar *field = xmlNodeGetContent(fieldPtr->children);
			xmlChar *type = xmlNodeGetContent(typePtr->children);
			addCondition((char *)device, (char *) field, (char *) type, (char *) value);
			xmlFree(value);
			xmlFree(device);
			xmlFree(field);
			xmlFree(type);
		}
	}
}

/**
 * Ecrit un fichier de règles sur une sortie
 * \param rules les règles à écrire
 * \param output la sortie à utiliser
 */
void ftraceRules(Rule *rules, int output)
{
	Rule *r;
	int i=0, j;
	char msg[1024];
    
    printf("\n==========[[ Regles chargees ]]==========\n");
    
	for(r = rules; r != NULL; r = r->next)
	{
		sprintf(msg, "\n\033[31mRule n°%d :\033[00m %s\n", ++i, r->name);        
		write(output, msg, strlen(msg));
        
		// Liste les actions
		Action *ac;
		j=0;
		sprintf(msg, "├── \033[32mActions\033[00m\n");
		write(output, msg, strlen(msg));
		for(ac = r->actions; ac != NULL; ac = ac->next)
		{
			if(ac->next == NULL)
			{
				sprintf(msg, "│\t└── \033[33mAction n°%d : \033[34mDo\033[00m %s\033[34m->\033[00m%s \033[34m=\033[00m %s\n", ++j, getDeviceName(ac->device), getFieldName(ac->field), getStateName(ac->state));
				write(output, msg, strlen(msg));
			}
			else
			{
				sprintf(msg, "│\t├── \033[33mAction n°%d : \033[34mDo\033[00m %s\033[34m->\033[00m%s \033[34m=\033[00m %s\n", ++j, getDeviceName(ac->device), getFieldName(ac->field), getStateName(ac->state));
				write(output, msg, strlen(msg));
			}
		}

		// Liste les alertes
		Alert *al;
		j=0;
		sprintf(msg, "├── \033[32mAlertes\033[00m\n");
		write(output, msg, strlen(msg));
		for(al = r->alerts; al != NULL; al = al->next)
		{
			if(al->next == NULL)
			{
				sprintf(msg, "│\t└── \033[33mAlert n°%d : \033[34mSend \"\033[00m%s\033[34m\" To\033[00m %s\n", ++j, al->message, getRecipientName(al->recipient));
				write(output, msg, strlen(msg));
			}
			else
			{
				sprintf(msg, "│\t├── \033[33mAlert n°%d : \033[34mSend\033[00m \"%s\" \033[34mTo\033[00m %s\n", ++j, al->message, getRecipientName(al->recipient));
				write(output, msg, strlen(msg));
			}
		}

		// Liste les conditions
		Condition *cd;
		j=0;
		sprintf(msg, "└── \033[32mConditions\033[00m\n");
		write(output, msg, strlen(msg));
		for(cd = r->conditions; cd != NULL; cd = cd->next)
		{
			if(cd->next == NULL)
			{
				sprintf(msg, "\t└── \033[33mCondition n°%d : \033[34mWhen\033[00m %s[%d]\033[34m->\033[00m%s %s %f\n", ++j,  getDeviceName(cd->device), cd->device, getFieldName(cd->field), getConditionType(cd->type), cd->value);
				write(output, msg, strlen(msg));
			}
			else
			{
				sprintf(msg, "\t├── \033[33mCondition n°%d : \033[34mWhen\033[00m %s[%d]\033[34m->\033[00m%s %s %f \033[34mAnd\033[00m\n", ++j, getDeviceName(cd->device), cd->device, getFieldName(cd->field), getConditionType(cd->type), cd->value);
				write(output, msg, strlen(msg));
			}
		}
	}
    
    printf("\n=========================================\n");    
}                       

/**
 * Alais de ftraceRules permettant d'écrire sur la sortie standard
 * \param rules les règles à écrire
 */
void traceRules(Rule *rules)
{
	ftraceRules(rules, stdout->_fileno);
}

/**
 * Parse le fichier XML de règles et les intègre dans la structure Rule afin 
 * qu'elles soient exploitables par le core.
 * Le fichier est validé par le dtd
 * La librairie libxml2 est utilisé en DOM
 * \param filename le fichier xml de règles à parser
 * \param sensor les capteurs reconnus par le core afin de les associer aux règles
 * \return liste de règles dans la structure du code
 */
Rule * get_rules(const char *filename, infos_sensor *sensor_list)
{
    xmlDocPtr doc;
	xmlNodePtr racine;
	Rule *rules = malloc(sizeof(Rule));
	sensor = sensor_list;

	// Ouverture du document
	xmlKeepBlanksDefault(0); // Ignore les noeuds texte composant la mise en forme
	doc = xmlParseFile(filename);
    
	if(doc == NULL || !DTDValidation(doc, "config/rules.dtd", 0)) {
		fprintf(stderr, "Invalid rule file\n");
		return (Rule*)NULL;
	}

	// Récupération de la racine
	racine = xmlDocGetRootElement(doc);
	if (racine == NULL) {
		fprintf(stderr, "Rules file empty\n");
		xmlFreeDoc(doc);
		return NULL;
	}
    
    // Parcours
    if(DEBUG_MODE == 1)
    {
        printf("Starting to browse %d...\n");
    }
	lastRule=rules;
	nodeBrowser(racine, fillRules);
    if(DEBUG_MODE == 1)
    {    
        printf("Browsing end reached!\n");
    }
    
	// Libération de la mémoire
	xmlFreeDoc(doc);
	traceRules(rules->next);
	return rules->next;
}

/**
 * Libère la mémoire allouée par le parsing XML à partir d'une liste Rule.
 * (Notamment les xml_free de la librairie libxml2)
 * \param rules les règles à nettoyer
 */
void free_rules(Rule* rules)
{
	Rule *rule, *new_rule;
	Action *action, *new_action;
	Alert *alert, *new_alert;
	Condition *condition, *new_condition;
	rule = rules;
	while(rule)
	{
		free(rule->name);
		action = rule->actions;
		while(action)
		{
			new_action = action->next;
			free(action);
			action = new_action;
		}
		alert = rule->alerts;
		while(alert)
		{
			xmlFree(alert->message);
			new_alert = alert->next;
			free(alert);
			alert = new_alert;
		}
		condition = rule->conditions;
		while(condition)
		{
			new_condition = condition->next;
			free(condition);
			condition = new_condition;
		}
		new_rule = rule->next;
		free(rule);
		rule = new_rule;
	}
}
