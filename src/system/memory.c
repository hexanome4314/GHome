#include "memory.h"

void* gmalloc(size_t size)
{
	return malloc(size);
}

void free(void* ptr)
{
	free(ptr);
}
