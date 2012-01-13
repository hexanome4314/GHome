#include "process.h"
#include <stdio.h>

gpid_t G_create(fptr_t fct, int prio, int argc, int* argv)
{
	pid_t pid = fork();
	if(pid < 0) /* error */
		return pid;
	else if(pid == 0) {
		(*fct)(argc, argv);
		exit(0);
	}
	return pid;
}

void G_yield()
{
}

void G_exit(int status)
{
	exit(status);
}

void G_wait(int time)
{
	sleep(time);
}

void G_wakeUp(gpid_t id)
{
	/* pas d'équivalent ?*/
}
