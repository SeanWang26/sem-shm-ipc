#include <stdio.h>
#include <unistd.h>
#include "shm.h"
#include "sem.h"

#define PlugClientVerion "0.0"
char* semname = "wwww000000000000000000000";

int main(int argc, char** argv)
{
	printf("frontclient, build time %s, Version %s\n", __TIME__ , PlugClientVerion);

	
	//int shmfd = open_or_create_shm("shm.wooww.sssssss222222222222222222222222222");
	sem_t* handle = open_or_create_sem(semname);

	printf("sem_open handle %p\n", handle);
	sleep(20);

	printf("sem_post handle %p\n", handle);
	remove_sem(semname);
	//close_sem(handle);
	if(sem_post(handle)){ printf("sem_post error handle %p\n", handle); }

	if(sem_post(handle)){ printf("sem_post error handle %p\n", handle); }

	if(sem_post(handle)){ printf("sem_post error handle %p\n", handle); }

	sleep(20);
	close_sem(handle);
		
	
	printf("remove_sem handle %p\n", handle);
	sleep(100);
	
	return 0;
}







