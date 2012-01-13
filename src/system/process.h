/**
Gestion de multi-tâche maison.
*/
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

typedef void (*fptr_t) (int, int*);
typedef pid_t gpid_t;

/**
Créer une tâche qui lance la fonction fct

\param prio	Priorité de la tâche (nonprisencomptelol)
\param fct	Fonction 
\param argX	Parametre numéro X
*/
gpid_t G_create(fptr_t fct, int prio, int argc, int* argv);

/**
Passe la main à l'ordonnanceur (ne fait rien dans cette première implémentation)
*/
void G_yield();

/**
Arrêt de la tâche
*/
void G_exit(int status);

/**
La tâche s'endort pour le temps indiqué
\param time	Temps en s d'attente
*/
void G_wait(int time);

/**
Réveiller une tâche
\param id	Id de la tâche à réveiller
*/
void G_wakeUp(gpid_t id);

