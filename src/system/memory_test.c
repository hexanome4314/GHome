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
	pa = (int*)gmalloc(1000);
	pb = (int*)gmalloc(1000);
	pc = (int*)gmalloc(1000);
	pd = (int*)gmalloc(100);
	pe = (int*)gmalloc(100);
	pf = (int*)gmalloc(100);
	gfree(pb);
	pa = (int*)grealloc(pa, 1500);
	gfree(pa);
//	gfree(pb);
	gfree(pc);
	gfree(pd);
	gfree(pe);
	gfree(pf);
	return 0;
}
