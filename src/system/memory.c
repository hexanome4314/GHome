#include "memory.h"

// Taille minimum à demander à sbrk()
#define MIN_SIZE 1024

// Récupérer l'adresse des données d'un slot
#define DATAS(slot) (void*)((char*)slot+sizeof(Slot))

/*
Cette gestion de mémoire n'est pas supposée fonctionner en
parallèle avec un autre allocateur
*/
typedef struct Slot Slot;
struct Slot {
	Slot* next; // pointeur vers slot suivant
	size_t size; // en octets
	char free; // 1 si la zone est libre, 0 sinon
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
#endif /* MEMORY_DEBUG */

/*
Découpe un slot en deux de manière à ce que la taille
du premier soit "size"
Retourne le premier argument (slot)
*/
static Slot* slice_slot(Slot* slot, size_t size)
{
	// Slot suivant (situé après le header et les données du slot courant)
	Slot* newslot = (Slot*)(sizeof(Slot)+(int)slot+size);
	newslot->next = slot->next;
	newslot->size = slot->size - size - sizeof(Slot);
	newslot->free = 1;

	slot->next = newslot;
	slot->size = size;
	slot->free = 1;

	return slot;
}

/*
Fusionne le slot en paramêtre avec le suivant
 */
static void merge_slots(Slot* slot)
{
	slot->size = slot->size + slot->next->size + sizeof(Slot);
	slot->next = slot->next->next;
}

/*
Trouve une place pouvant contenir "size" octets
*/
static Slot* find_place(size_t size)
{
	// First fit
	Slot* slot = slotlist;
	while(slot != NULL)
	{
		if(slot->free && slot->size >= size)
			return slot;
		slot = slot->next;
	}
	return NULL; // Non trouvé
}

/*
Demande au système de plus d'espace
Retourne un slot ayant au moins size octets
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
			return lastslot;
		}
		else
			return lastslot->next;
	}
}

void* gmalloc(size_t size)
{
	Slot* slot = find_place(size);
#ifdef MEMORY_DEBUG
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
		slice_slot(slot, size);
	slot->free = 0;
#ifdef MEMORY_DEBUG
	printf("allocation of %lu to 0x%x\n", (unsigned long)size, (int)DATAS(slot));
	print_memory();
#endif /* MEMORY_DEBUG */
	return DATAS(slot);
}

void gfree(void* ptr)
{
	Slot* slot = slotlist;
	Slot* prev_slot = NULL;
	while(DATAS(slot) != ptr)
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
	if (ptr == NULL)
		return gmalloc(size);
	if (size == 0)
	{
		gfree(ptr);
		return NULL;
	}
	while(DATAS(slot) != ptr)
	{
		if (slot == NULL)
			return NULL; // not found...
		slot = slot->next;
	}
	//old_area = ptr;
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
