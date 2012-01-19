/*
Test du bon fonctionnement de la gestion de mémoire
Il n'y a pour l'instant pas grand chose ici...
*/
#include "memory_test.h"
#include <stdio.h>
#include "memory.h"
int main()
{
//	int a=42;
	int* pa;
	int* pb;
	int* pc;
	int* pd;
	int* pe;
	int* pf;
	pb = gmalloc(1000);
	pc = gmalloc(1000);
	pd = gmalloc(100);
	pe = gmalloc(100);
	pf = gmalloc(100);
	gfree(pa);
	gfree(pb);
	gfree(pc);
	gfree(pd);
	gfree(pe);
	gfree(pf);
	return 0;
}
