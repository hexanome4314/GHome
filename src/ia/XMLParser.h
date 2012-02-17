#ifndef __XMLPARSER_H
#define __XMLPARSER_H

#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>
#include <string.h>
#include <ctype.h>

typedef void (*fct_parcours_t)(xmlNodePtr);

void nodeBrowser(xmlNodePtr noeud, fct_parcours_t f);
xmlAttrPtr getAttrByName(xmlNodePtr node, char *attrName);

int compare(char *s1, char *s2);


enum {
    ERROR_OCCURED = -1, // Une erreur est survenue pendant la validation
    NOT_VALID = 0,      // Le document n'est pas valide
    VALID = 1           // Le document est valide
};

int DTDValidation(xmlDocPtr doc, const char *fichier_dtd, int afficher_erreurs);

#endif /*__XMLPARSER_H */
