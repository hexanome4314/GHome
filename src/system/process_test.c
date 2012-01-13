#include "process.h"
#include <stdio.h>
/**
Exemple rapide d'utilisation de notre lib de multitache
*/

void test_process(int argc, int* argv)
{
	int i;
	pid_t pid = getpid();
	for (i=0; i<argv[0]; i++)
	{
		printf("im the child\n");
		sleep(2);
	}
	G_exit(0);
}

int main()
{
	int argv[] = {3}; // arguments à la fonction
	int status;
	gpid_t child = G_create(&test_process, 10, 1, argv);
	sleep(1);
	printf("im the main\n");
	waitpid(child, &status, 0);
	return 0;
}
