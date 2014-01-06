#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "shm.h"
#include "sem.h"

#define PlugVerion "0.0"
char* semname = "wwww000000000000000000000";

void* task_work(void *cmd)
{
	printf("%s\n", (char*)cmd);

	sem_t* handle = open_sem(semname);	
	printf("open_or_create_sem handle %p\n", handle);

	while(1)
	{
		printf("sem_wait handle %p\n", handle);
		if(sem_wait(handle))
		{
			printf("sem_wait failed %p\n", handle);
		}
	}

	
	printf("sem_wait out %p\n", handle);
	
	return 0;
}

int post_task()
{
	pthread_t tid = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, task_work, (void*)"printf");
	pthread_attr_destroy(&attr);

	return 0;
}
int main(int argc, char** argv)
{
	printf("frontplug, build time %s, Version %s\n", __TIME__ , PlugVerion);



	post_task();

	sleep(1000);
	return 0;
}




































