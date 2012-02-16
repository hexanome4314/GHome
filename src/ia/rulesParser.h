#ifndef RULESPARSER_H
#define RULESPARSER_H
#include <stdio.h>
#include <stdlib.h>
#include "XMLParser.h"
#include "demon.h"
#include "engine.h"
#include "engine-types.h"


Rule * get_rules(const char *filename);

void free_rules(Rule *rules);

#endif /* RULESPARSER_H */
