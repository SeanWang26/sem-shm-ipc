#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "shm.h"
#include "sem.h"
#include "vsem.h"

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

int semid2 = -1;
int semid3 = -1;

void* task_work2(void *cmd)
{
	printf("%s\n", (char*)cmd);
	int cnt = 0 ;
	while(1)
	{	
		wait_vsem(semid2);
		
		printf("semid2 get ack %d\n", ++cnt);

		post_vsem(semid3);

	}
int open_vsem(const char *filename);

int waittimeout_vsem(int semid, struct timespec *timeout);
int close_vsem(int semid);
int remove_vsem(const char * filename);
	
	return 0;
}

int create_detached_thread(pthread_t *tid, void* (*func)(void *), void* arg)
{
	int res = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	res = pthread_create(tid, &attr, func, arg);
	pthread_attr_destroy(&attr);

	return res;
}

int post_task()
{
	pthread_t tid = 0;
	return	create_detached_thread(&tid, task_work2, (void*)"printf");
}


int main(int argc, char** argv)
{
	printf("frontplug, build time %s, Version %s\n", __TIME__ , PlugVerion);

	semid2  = open_or_create_vsem("xxxxxxxxdddd");
	semid3	= open_or_create_vsem("xxxxxxxxddddssss");

	post_task();

	while(1)
	{
		//post_vsem(semid2);
		
		wait_vsem(semid3);
		printf("semid3 get ack\n");
	}
	
	sleep(1000);
	return 0;
}






























