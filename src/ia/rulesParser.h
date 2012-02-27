#ifndef RULESPARSER_H
#define RULESPARSER_H
#include <stdio.h>
#include <stdlib.h>
#include "XMLParser.h"
#include "core.h"
#include "engine.h"
#include "engine-types.h"
#include "fields.h"

Rule * get_rules(const char *filename, infos_sensor *sensor);

void free_rules(Rule *rules);

/**
 * Ecrit un fichier de règles sur une sortie
 * \param rules les règles à écrire
 * \param output la sortie à utiliser
 */
void ftraceRules(Rule *rules, int output);

#endif /* RULESPARSER_H */
