#include "XMLParser.h"

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

// Comparaison de string insensible à la casse
int compare(char *s1, char *s2)
{
    int ls1 = strlen(s1);
    int ls2 = strlen(s2);

    //printf("comparing \"%s\" and \"%s\" : ", s1, s2);

    if(ls1 != ls2)
    {
        //printf("no.\n");
        return -1;
    }
    else
    {
        int i = 0;
        for(; i < ls1 && tolower(s1[i]) == tolower(s2[i]); i++);

        if(i == ls1)
        {
            //printf("yes.\n");
            return 0;
        }
        else
        {
            //printf("no.\n");
            return -1;
        }
    }
}
 
// Fonction de validation d'un arbre DOM à l'aide d'une DTD
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