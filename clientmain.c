#include <stdio.h>
#include <unistd.h>
#include "shm.h"
#include "sem.h"
#include "vsem.h"

#define PlugClientVerion "0.0"
char* semname = "wwww000000000000000000000";
int semid2 = -1;
int semid3 = -1;

int main(int argc, char** argv)
{
	printf("frontclient, build time %s, Version %s\n", __TIME__ , PlugClientVerion);

	semid2  = open_or_create_vsem("xxxxxxxxdddd0");
	//close_vsem(semid2);


	semid3	= open_or_create_vsem("xxxxxxxxddddssss0");
	//close_vsem(semid3);
	
	//return 0;

	while(1)
	{
		wait_vsem(semid2);
		printf("semid2 get ack\n");
		post_vsem(semid3);
	}

	
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







