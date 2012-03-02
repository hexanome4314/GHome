#ifndef __XMLPARSER_H
#define __XMLPARSER_H

/*
 * Ce code ajoute une couche pratique à la libxml2
 */

#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>
#include <string.h>
#include <ctype.h>

// Type de la fonction a executer sur chaque noeud DOM xml
typedef void (*fct_parcours_t)(xmlNodePtr);

// Etat de la validation XML
enum {
    ERROR_OCCURED = -1, // Une erreur est survenue pendant la validation
    NOT_VALID = 0,      // Le document n'est pas valide
    VALID = 1           // Le document est valide
};

/**
 * Parcours un arbre DOM XML a partir d'un noeud et applique la fonction f à tous
 * ses noeuds enfants (libxml2)
 * \param noeud racine de l'arbre DOM
 * \param f pointeur sur la fonction à executer sur chaque noeud fils
 */
void nodeBrowser(xmlNodePtr noeud, fct_parcours_t f);

/**
 * Retourne un pointeur vers l'attribut d'un nom donné identifié par son nom
 * \param node Noeud xml ou effectuer la recherche
 * \param attrName Nom de l'attribut donc on souhaite récupérer un pointeur
 * \return pointeur vers l'attribut trouvé, NULL sinon
 */
xmlAttrPtr getAttrByName(xmlNodePtr node, char *attrName);

/**
 * Compare deux chaines sans être sensible à la casse
 * \param s1 chaine 1
 * \param s2 chaine 2
 * \return 0 si les chaines sont égales, -1 sinon
 */
int compare(char *s1, char *s2);

/**
 * Valide un arbre DOM XML avec sa dtd
 * \param doc pointeur de document xml (libxml2)
 * \param fichier_dtd pointeur vers le fichier de DTD
 * \param afficher_erreurs booléen au sens C pour définir sur les erreurs doivent être affichées
 * \return etat définie dans l'enum plus haut
 */
int DTDValidation(xmlDocPtr doc, const char *fichier_dtd, int afficher_erreurs);

#endif /*__XMLPARSER_H */
