#include <time.h>
#include <stdio.h>
#include "demon.h"

int main(){
	init_demon();
	printf("fd : %d\n", get_fd_by_name("bouton"));
	for(;;){
		sleep(5);
		puts("alive");
	}
}
