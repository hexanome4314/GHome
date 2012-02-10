#include "parser.h"

#define PATH_ALERT "/rules/rule/actions/alert"
#define PATH_COND "/rules/rule/conditions/condition"
#define PATH_ACTION "/rules/rule/actions/activate"

typedef void (*fct_parcours_t)(xmlNodePtr);
Rule* lastRule = NULL;

void nodeBrowser(xmlNodePtr noeud, fct_parcours_t f) {
    xmlNodePtr n;

    for (n = noeud; n != NULL; n = n->next) {
        f(n);
        if ((n->type == XML_ELEMENT_NODE) && (n->children != NULL)) {
            nodeBrowser(n->children, f);
        }
    }
}

xmlAttrPtr getAttrByName(xmlNodePtr node, char *attrName)
{
    xmlAttrPtr a;

    if(node->properties != NULL)
    {
        for(a = node->properties; a != NULL; a = a->next)
        {
            if(a->type == XML_ATTRIBUTE_NODE && a->children != NULL)
            {
                if(strcmp((char *)a->name, attrName) == 0)
                {
                    return a;
                }
            }
        }
    }
    return NULL;
}


char *  getConditionType(enum Condition_type type)
{
    return "TYPE";
}

int getConditionTypeIndex(char *type)
{
    return 0;
}

int getDeviceIndex(char *device) {
    return 0;
}

char * getDeviceName(int device)
{
    return "DEVICE";
}

char * getRecipientName(int recipient) {
    return "RECIPIENT";
}


char getState(char *state) {
    return 0;
}

char * getStateName(char state)
{
    return "STATE";
}

int getRecipientIndex(char *recipient) {
    return 0;
}

int getFieldIndex(char *field) {
    return 0;
}

char * getFieldName(int field)
{
    return "FIELD";
}

Action * getLastAction(Rule *rule)
{
    Action *a, *res;

    for(a = rule->actions; a != NULL; a = a->next)
    {
        res = a;
    }

    return res;
}

Alert * getLastAlert(Rule *rule)
{
    Alert *a, *res;

    for(a = rule->alerts; a != NULL; a = a->next)
    {
        res = a;
    }

    return res;
}

Condition * getLastCondition(Rule *rule)
{
    Condition *c, *res;

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
    printf("New rule added : %s\n", name);
}

void addAction(char *device, char *state) {
    Action *action = malloc(sizeof(Action));
    action->device = getDeviceIndex(device);
    action->state = getState(state);
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

    printf("New action added : %s -> %s\n", device, state);
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
    printf("New alert added : %s -> %s\n", recipient, message);
}

void addCondition(char *device, char *field, char *type, char *value)
{
    Condition *condition = malloc(sizeof(Condition));
    condition->device = getDeviceIndex(device);
    condition->field = getFieldIndex(field);
    condition->type = getConditionTypeIndex(type);
    condition->value = *value;
    condition->next = NULL;

    Condition *lastCondition = getLastCondition(lastRule);

    if(lastCondition == NULL)
    {
        lastRule->conditions = condition;
    }
    else
    {
        lastCondition->next = condition;
    }
    printf("New condition added : %s->%s %s %s\n", device, field, type, value);
}

void fillRules(xmlNodePtr node) {
    xmlChar *chemin = xmlGetNodePath(node);

    // Ajout d'une regle
    if(strcmp((char *)chemin, "/rules/rule") == 0)
    {
        xmlAttrPtr attr = getAttrByName(node, "name");

        if(attr != NULL && attr->children != NULL)
        {
            xmlChar *content = xmlNodeGetContent(attr->children);
            addRule((char *)content);
        }
    }

    // Ajout d'une action
    int chAcSize=strlen(PATH_ACTION);
    char * cheminAction = malloc(chAcSize+1);
    cheminAction[chAcSize] = '\0';
    strncpy(cheminAction, (char *)chemin, chAcSize);
    if(strcmp((char *)cheminAction, PATH_ACTION) == 0)
    {
        xmlAttrPtr attr = getAttrByName(node, "device");

        if(attr != NULL && attr->children != NULL && node->children != NULL && node->children->type == XML_TEXT_NODE)
        {
            xmlChar *state = xmlNodeGetContent(node);
            xmlChar *device = xmlNodeGetContent(attr->children);
            addAction((char *)device, (char *) state);
        }
    }

    // Ajout d'une alerte
    int chAlSize=strlen(PATH_ALERT);
    char * cheminAlert = malloc(chAlSize+1);
    cheminAlert[chAlSize] = '\0';
    strncpy(cheminAlert, (char *)chemin, chAlSize);

    if(strcmp((char *)chemin, PATH_ALERT) == 0)
    {
        xmlAttrPtr attr = getAttrByName(node, "recipient");

        if(attr != NULL && attr->children != NULL && node->children != NULL && node->children->type == XML_TEXT_NODE)
        {
            xmlChar *message = xmlNodeGetContent(node);
            xmlChar *recipient = xmlNodeGetContent(attr->children);
            addAlert((char *)recipient, (char *) message);
        }
    }

    // Ajout d'une condition
    int chCdSize = strlen(PATH_COND);
    char * cheminCond = malloc(chCdSize+1);
    cheminCond[chCdSize] = '\0';
    strncpy(cheminCond, (char *)chemin, chCdSize);
    
    if(strcmp(cheminCond, PATH_COND) == 0)
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
        }
    }

    xmlFree(chemin);
}

void traceRules(Rule *rules)
{
    Rule *r;
    int i=0, j;

    for(r = rules; r != NULL; r = r->next)
    {
        printf("\n\033[31mRule n°%d :\033[00m %s\n", ++i, r->name);

        // Liste les actions
        Action *ac;
        j=0;
        printf("├── \033[32mActions\033[00m\n");
        for(ac = r->actions; ac != NULL; ac = ac->next)
        {
            if(ac->next == NULL)
            {
                printf("│\t└── \033[33mAction n°%d : \033[34mDo\033[00m %s\033[34m->\033[00m%s \033[34m=\033[00m %s\n", ++j, getDeviceName(ac->device), getFieldName(ac->field), getStateName(ac->state));
            }
            else
            {
                printf("│\t├── \033[33mAction n°%d : \033[34mDo\033[00m %s\033[34m->\033[00m%s \033[34m=\033[00m %s\n", ++j, getDeviceName(ac->device), getFieldName(ac->field), getStateName(ac->state));
            }
        }

        // Liste les alertes
        Alert *al;
        j=0;
        printf("├── \033[32mAlertes\033[00m\n");
        for(al = r->alerts; al != NULL; al = al->next)
        {
            if(al->next == NULL)
            {
                printf("│\t└── \033[33mAlert n°%d : \033[34mSend \"\033[00m%s\033[34m\" To\033[00m %s\n", ++j, al->message, getRecipientName(al->recipient));
            }
            else
            {
                printf("│\t├── \033[33mAlert n°%d : \033[34mSend\033[00m \"%s\" \033[34mTo\033[00m %s\n", ++j, al->message, getRecipientName(al->recipient));
            }
        }

        // Liste les conditions
        Condition *cd;
        j=0;
        printf("└── \033[32mConditions\033[00m\n");
        for(cd = r->conditions; cd != NULL; cd = cd->next)
        {
            if(cd->next == NULL)
            {
                printf("\t└── \033[33mCondition n°%d : \033[34mWhen\033[00m %s\033[34m->\033[00m%s %s %c\n", ++j, getDeviceName(cd->device), getFieldName(cd->field), getConditionType(cd->type), cd->value);
            }
            else
            {
                printf("\t├── \033[33mCondition n°%d : \033[34mWhen\033[00m %s\033[34m->\033[00m%s %s %c \033[34mAnd\033[00m\n", ++j, getDeviceName(cd->device), getFieldName(cd->field), getConditionType(cd->type), cd->value);
            }
        }
    }
}

Rule * get_rules(char *filename) {
    xmlDocPtr doc;
    xmlNodePtr racine;
    Rule *rules = malloc(sizeof(Rule));

    // Ouverture du document
    xmlKeepBlanksDefault(0); // Ignore les noeuds texte composant la mise en forme
    doc = xmlParseFile(filename);
    if (doc == NULL) {
        fprintf(stderr, "Document XML invalide\n");
        return NULL;
    }

    // Récupération de la racine
    racine = xmlDocGetRootElement(doc);
    if (racine == NULL) {
        fprintf(stderr, "Document XML vierge\n");
        xmlFreeDoc(doc);
        return NULL;
    }

    // Parcours
    lastRule=rules;
    nodeBrowser(racine, fillRules);
    
    // Libération de la mémoire
    xmlFreeDoc(doc);

    return rules->next;
}
/*
int main() {
    Rule *rules;
    rules = getRules("rules.xml");
    traceRules(rules);
    return 0;
}*/
