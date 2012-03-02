#include "XMLParser.h"

/**
 * Parcours un arbre DOM XML a partir d'un noeud et applique la fonction f à tous
 * ses noeuds enfants (libxml2)
 * \param noeud racine de l'arbre DOM
 * \param f pointeur sur la fonction à executer sur chaque noeud fils
 */
void nodeBrowser(xmlNodePtr noeud, fct_parcours_t f)
{
    xmlNodePtr n;

    for (n = noeud; n != NULL; n = n->next) {
        f(n);
        if ((n->type == XML_ELEMENT_NODE) && (n->children != NULL)) {
            nodeBrowser(n->children, f);
        }
    }
}

/**
 * Retourne un pointeur vers l'attribut d'un nom donné identifié par son nom
 * \param node Noeud xml ou effectuer la recherche
 * \param attrName Nom de l'attribut donc on souhaite récupérer un pointeur
 * \return pointeur vers l'attribut trouvé, NULL sinon
 */
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

/**
 * Compare deux chaines sans être sensible à la casse
 * \param s1 chaine 1
 * \param s2 chaine 2
 * \return 0 si les chaines sont égales, -1 sinon
 */
int compare(char *s1, char *s2)
{
    int ls1 = strlen(s1);
    int ls2 = strlen(s2);

    if(ls1 != ls2)
    {
        return -1;
    }
    else
    {
        int i = 0;
        for(; i < ls1 && tolower(s1[i]) == tolower(s2[i]); i++);

        if(i == ls1)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
}
 
/**
 * Valide un arbre DOM XML avec sa dtd
 * \param doc pointeur de document xml (libxml2)
 * \param fichier_dtd pointeur vers le fichier de DTD
 * \param afficher_erreurs booléen au sens C pour définir sur les erreurs doivent être affichées
 * \return etat définie dans l'enum plus haut
 */
int DTDValidation(xmlDocPtr doc, const char *fichier_dtd, int afficher_erreurs)
{
    int ret;
    xmlDtdPtr dtd;
    xmlValidCtxtPtr vctxt;

    // Traitement de la DTD
    if ((dtd = xmlParseDTD(NULL, (xmlChar *)fichier_dtd)) == NULL) {
        return ERROR_OCCURED;
    }
    // Création du contexte de validation
    if ((vctxt = xmlNewValidCtxt()) == NULL) {
        xmlFreeDtd(dtd);
        return ERROR_OCCURED;
    }
    // Affichage des erreurs de validation
    if (afficher_erreurs) {
        vctxt->userData = (void *) stderr;
        vctxt->error = (xmlValidityErrorFunc) fprintf;
        vctxt->warning = (xmlValidityWarningFunc) fprintf;
    }
    // Validation
    ret = xmlValidateDtd(vctxt, doc, dtd);
    // Libération de la mémoire
    xmlFreeValidCtxt(vctxt);
    xmlFreeDtd(dtd);

    return ret;
}
