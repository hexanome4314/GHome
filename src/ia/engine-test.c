#include <stdio.h>
#include "engine.h"
#include "memory.h"
#include "ios_api.h"

void handler0(unsigned int field, float val)
{
	if(field == 2)
		printf ("Something happened (field %d, val %f)\n", field, val);
	apply_actions(0, field, val);
}

void handler1(unsigned int field, float val)
{
	if(field == 2)
		printf("The second one is noisy\n");
	apply_actions(1, field, val);
}
int main(int argc, char *argv[])
{
	int major;
	int fd1;
	int fd2;
	float value;
	load_xml();
	if (ios_init() != 0)
	{
		printf("Erreur au lancement de l'ios");
		return -1;
	}
	major = ios_install_driver("libdriver.so.1.0.1", "127.0.0.1", 8080);
	printf("Major : %d\n", major);
	fd1 = ios_add_device(major, 1);
	printf ("Fd : %d\n", fd1);
	ios_attach_handler(fd1, handler0);
	sleep(5);

	fd2 = ios_add_device(major, 2);
	printf("Fd2 : %d\n", fd2);
	ios_attach_handler(fd2, handler1);
	sleep(5);

	ios_remove_device(fd1);
	ios_remove_device(fd2);
	sleep(1);
	ios_uninstall_driver(major);
	ios_release();
	return 0;
}
