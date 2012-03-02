#ifndef RULESPARSER_H
#define RULESPARSER_H
#include <stdio.h>
#include <stdlib.h>
#include "XMLParser.h"
#include "core.h"
#include "engine.h"
#include "engine-types.h"
#include "fields.h"

/**
 * Parse le fichier XML de règles et les intègre dans la structure Rule afin 
 * qu'elles soient exploitables par le core.
 * Le fichier est validé par le dtd
 * La librairie libxml2 est utilisé en DOM
 * \param filename le fichier xml de règles à parser
 * \param sensor les capteurs reconnus par le core afin de les associer aux règles
 * \return liste de règles dans la structure du code
 */
Rule * get_rules(const char *filename, infos_sensor *sensor);

/**
 * Libère la mémoire allouée par le parsing XML à partir d'une liste Rule.
 * (Notamment les xml_free de la librairie libxml2)
 * \param rules les règles à nettoyer
 */
void free_rules(Rule *rules);

/**
 * Ecrit un fichier de règles sur une sortie
 * \param rules les règles à écrire
 * \param output la sortie à utiliser
 */
void ftraceRules(Rule *rules, int output);

#endif /* RULESPARSER_H */
