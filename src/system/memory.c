#include "memory.h"
/* Taille minimum à demander à sbrk() */
#define MIN_SIZE 1024
#define ALIGNMENT 8

#include <stdio.h>
/*
Cette gestion de mémoire n'est pas supposée fonctionner en
parallèle avec un autre allocateur
*/
typedef struct Slot Slot;
struct Slot {
	Slot* next; // pointeur vers slot suivant
	size_t size; // en octets
	char free; // 1 si la zone est libre, 0 sinon
	char stoupid[4];
};

static Slot* slotlist = NULL;

#ifdef MEMORY_DEBUG
/*
Affiche l'ensemble des slot et leur état
*/
static void print_memory()
{
	Slot* slot = slotlist;
	printf("==Memory dump !==\n");
	while (slot != NULL)
	{
		printf(" 0x%x | %d | %lu\n", (int)slot, slot->free, (unsigned long)slot->size);
		slot = slot->next;
	}
	printf("\n");
}
#else
#define print_memory()
#endif /* MEMORY_DEBUG */

/*
Affiche une erreur si un slot n'est pas valide d'un point de vue de l'alignement
*/
#ifdef ALIGNMENT_DEBUG
static void check_align(Slot* slot, char* caller)
{
//	printf("%s", caller);
	if((unsigned long)slot%ALIGNMENT != 0)
		printf("Alignment error (%s) : slot 0x%x is at wrong place\n", caller, (int)slot);
	if(slot->size%ALIGNMENT != 0)
		printf("Alignment error (%s) : slot 0x%x has a wrong size (%d)\n", caller, (int)slot, slot->size);
}
#else
#define check_align(...)
#endif /* ALIGNMENT_DEBUG */


/** 
 * 
 * Découpe un slot en deux de manière à ce que la taille
 * du premier soit "size"
 * 
 * @param slot Le slot à découper
 * @param size La nouvelle taille du premier slot
 * 
 * @return slot, le premier argument
 */
static Slot* slice_slot(Slot* slot, size_t size)
{
	// Slot suivant (situé après le header et les données du slot courant)
	Slot* newslot;
//	size = size+size%ALIGNMENT;
	check_align(slot, "slice_slot demande");
	newslot= (Slot*)(sizeof(Slot)+(int)slot+size);
	newslot->next = slot->next;
	newslot->size = slot->size - size - sizeof(Slot);
	newslot->free = 1;

	slot->next = newslot;
	slot->size = size;
	check_align(slot, "slice_slot first");
	check_align(slot->next, "slice_slot second");
	return slot;
}


/** 
 * Fusionne deux slot
 * 
 * @param slot le slot qui sera mergé avec le suivant
 */
static void merge_slots(Slot* slot)
{
	slot->size = slot->size + slot->next->size + sizeof(Slot);
	slot->next = slot->next->next;
	check_align(slot, "merge_slot");
}

/** 
 * Trouve une place pouvant contenir une certaine taille
 * 
 * @param size la taille requise (sans header)
 * 
 * @return Un pointeur vers un slot suffisamment grand
 */
static Slot* find_place(size_t size)
{
	// First fit
	Slot* slot = slotlist;
	while(slot != NULL)
	{
		if(slot->free && slot->size >= size)
		{
			check_align(slot, "find_place");
			return slot;
		}
		slot = slot->next;
	}
	return NULL; // Non trouvé
}


/** 
 * Demande au système plus d'espace
 * 
 * @param size La taille minimale à demander
 * 
 * @return Le slot contenant l'espace requis
 */
static Slot* enlarge_space(size_t size)
{
	size_t new_size = (size < MIN_SIZE) ? MIN_SIZE : size;
	if(slotlist == NULL) /* Premier appel)*/
	{
		slotlist = (Slot*)sbrk(new_size + sizeof(Slot));
		slotlist->next = NULL;
		slotlist->free = 1;
		slotlist->size = new_size;
		check_align(slotlist, "first enlarge");
		return slotlist;
	}
	else                 /* Ajout à la fin de la liste */
	{
		Slot* lastslot = slotlist;
		while(lastslot->next != NULL)
			lastslot = lastslot->next;

		lastslot->next = (Slot*)sbrk(new_size + sizeof(Slot));
		lastslot->next->size = new_size;
		lastslot->next->free = 1;
		if (lastslot->free) // merge avec le precedent s'il est libre
		{
			merge_slots(lastslot);
			check_align(lastslot, "enlarge with merge");
			return lastslot;
		}
		else
		{
			check_align(lastslot->next, "enlarge without merge");
			return lastslot->next;
		}
	}
}

void* gmalloc(size_t size)
{
	Slot* slot;
	if (size%ALIGNMENT != 0)
		size = size + ALIGNMENT - size%ALIGNMENT;

	slot = find_place(size);
#ifdef MEMORY_DEBUG
	printf("slot size %d\n", sizeof(Slot));
	printf("allocation of %lu\n",(unsigned long)size);
#endif
	if (size == 0)
		return NULL;
	if (slot == NULL) // Pas de place
	{
		slot = enlarge_space(size); // Demande de plus d'espace
		if (slot == NULL) // Pas de place du tout
			return NULL;
	}
	if (slot->size > size + sizeof(Slot)) // find_place et enlarge_space ne découpent pas
	{
		check_align(slot, "gmalloc slice");
		slice_slot(slot, size);
	}
	slot->free = 0;
#ifdef MEMORY_DEBUG
	printf("allocation of %lu to 0x%x\n", (unsigned long)size, slot+1);
	print_memory();
#endif /* MEMORY_DEBUG */
#ifdef ALIGNMENT_DEBUG
	if(((unsigned long)(slot+1))%ALIGNMENT != 0)
		printf("Alignment error at address %d\n", slot+1);
#endif
	check_align(slot, "malloc");
	return slot+1;
}

void gfree(void* ptr)
{
	Slot* slot = slotlist;
	Slot* prev_slot = NULL;
	while(slot+1 != ptr)
	{
		if (slot == NULL)
			return; // not found...
		prev_slot = slot;
		slot = slot->next;
	}
	slot->free = 1;
	if (slot->next && slot->next->free)
		merge_slots(slot);
	if (prev_slot && prev_slot->free)
		merge_slots(prev_slot);
#ifdef MEMORY_DEBUG
	printf("freeing 0x%x\n",(int)ptr);
#endif /* MEMORY_DEBUG */
}

void* gcalloc(size_t nmemb, size_t size)
{
	char* ptr = malloc(nmemb*size);
	int i;
	if (size == 0 || nmemb == 0)
		return NULL;
	for(i=0; i<nmemb*size; i++)
		*(ptr+i) = 0;
	return ptr;
}


void* grealloc(void* ptr, size_t size)
{
	Slot* slot = slotlist;
	char* new_area;
	char* old_area = ptr;
	int i;
	int cpy_size;
	if (size%ALIGNMENT != 0)
		size = size + ALIGNMENT - size%ALIGNMENT;
	if (ptr == NULL)
		return gmalloc(size);
	if (size == 0)
	{
		gfree(ptr);
		return NULL;
	}
	while(slot+1 != ptr)
	{
		if (slot == NULL)
			return NULL; // not found...
		slot = slot->next;
	}
	// Test si c'est du raccourcissement avec la place d'ajouter un slot
	// bog
	/*if (slot->size - size > sizeof(Slot))
	{
		slice_slot(slot, size);
		slot->next->free = 1;
		if (slot->next->next && slot->next->next->free)
			merge_slots(slot->next);
		return slot+1;
	}
	// Raccourcissement sans la place d'ajouter un slot
	else if(slot->size >= size)
	{
		return(slot+1);
		}*/
	// Test si il n'y a pas besoin de le déplacer (bug?)
/*	if (slot->next && slot->next->free && slot->next->size+sizeof(Slot) >= size - slot->size)
	{
		merge_slots(slot);
		if(slot->size > size+sizeof(Slot))
			slice_slot(slot, size);
		return slot+1;
		}*/
	new_area = gmalloc(size);
	cpy_size = size < slot->size ? size : slot->size;
	for(i=0; i<cpy_size; i++)
	{
		*(new_area+i) = *(old_area+i);
	}
	gfree(slot);
	return new_area;
}

// Pour remplacer la libc (à charger avec LD_PRELOAD)
#ifdef OVERRIDE_STDLIB
void *malloc(size_t size)
{
	return gmalloc(size);
}

void free(void* ptr)
{
	return gfree(ptr);
}

void* calloc(size_t nmemb, size_t size)
{
	return gcalloc(nmemb, size);
}

void* realloc(void* ptr, size_t size)
{
	return grealloc(ptr, size);
}
#endif /*OVERRIDE_STDLIB*/
