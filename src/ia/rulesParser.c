#include "rulesParser.h"

#define FIELDS_SIZE DRV_LAST_VALUE

// Association id champ => nom
static char* Fields[] = {
#define X(define, str) str,
FIELDS
#undef X
};

#define DEBUG_MODE 0

#define RECIPIENTS_SIZE 2
static char* Recipients[RECIPIENTS_SIZE] = {"Web", "Unknown_Recipient"};

#define COMPARISONS_SIZE 4
static char* Comparisons[COMPARISONS_SIZE] = {"Equ", "Sup", "Inf", "Unknown_Comparison"};

static Rule* lastRule = NULL;

/** Liste de capteur fournie par core permettant d'associer un fd aux noms */
static infos_sensor *sensor;

/**
 * Donne le file descriptor d'un capteur installé à partir de son id
 * Si le capteur n'est pas installé retourne -1
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

int getConditionTypeIndex(char *type)
{
	int i = 0;

	for(; i < COMPARISONS_SIZE && compare(Comparisons[i], type) == -1; i++);

	return i;
}

int getDeviceIndex(char *device) {
	return get_fd_by_name(device);
}

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

int getRecipientIndex(char *recipient) {
	int i = 0;

	for(; i < RECIPIENTS_SIZE && compare(Recipients[i], recipient) == -1; i++);

	return i;
}


int getFieldIndex(char *field) {
	int i = 0;

	for(; i < FIELDS_SIZE && compare(Fields[i], field) == -1; i++);

	return i;
}

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

Action * getLastAction(Rule *rule)
{
	Action *a, *res = rule->actions;

	for(a = rule->actions; a != NULL; a = a->next)
	{
		res = a;
	}

	return res;
}

Alert * getLastAlert(Rule *rule)
{
	Alert *a, *res = rule->alerts;

	for(a = rule->alerts; a != NULL; a = a->next)
	{
		res = a;
	}

	return res;
}

Condition * getLastCondition(Rule *rule)
{
	Condition *c, *res = rule->conditions;

	for(c = rule->conditions; c!= NULL; c = c->next)
	{
		res = c;
	}

	return res;
}

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

void traceRules(Rule *rules)
{
	ftraceRules(rules, stdout->_fileno);
}

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
