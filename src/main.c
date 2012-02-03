#include <stdio.h>

#include "memory.h"

int main(int argc, char **argv)
{
	char* a = gmalloc(10);
	a = "salut";
	printf("%s\n", a);
	gfree(a);
	return(0);
}
