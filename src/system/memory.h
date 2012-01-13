/**
Allocation mémoire
\param: size	Taille à allouer
\return Emplacement alloué
\see malloc
*/

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdlib.h>

void* gmalloc(size_t size);

/**
Libération de mémoire
\param: ptr	Emplacement de la mémoire allouée avec gmalloc
\see free
*/
void gfree(void* ptr);

#endif /* __MEMORY_H__ */
