#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdlib.h>
#include <unistd.h>
#ifdef MEMORY_DEBUG
#include <stdio.h>
#endif /* MEMORY_DEBUG */
#ifdef ALIGNMENT_DEBUG
#include <stdio.h>
#endif /* ALIGNMENT_DEBUG */

/**
Allocation mémoire
\param: size	Taille à allouer
\return Emplacement alloué
\see malloc
*/
void* gmalloc(size_t size);

/**
Libération de mémoire
\param: ptr	Emplacement de la mémoire allouée avec gmalloc
\see free
*/
void gfree(void* ptr);

/**
Allocation de nmemb membre de taille size, et initialisation à 0
\param: nmemb	Nombre de membre
        size	Taille de chaque membre
\return Emplacement alloué
\see gcalloc
*/
void* gcalloc(size_t nmemb, size_t size);

/**
Reallocation d'une zone à une taille différente avec copie des
anciennes valeurs
\param: ptr	pointeur de l'ancienne zone
        size	nouvelle taille
\return Nouvel emplacement
\see realloc
*/
void* grealloc(void *ptr, size_t size);

#ifdef OVERRIDE_STDLIB
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
#endif /* OVERRIDE_STDLIB */

#endif /* __MEMORY_H__ */
