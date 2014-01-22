#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "xmmanager.h"
#include <string.h>

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

int _real_staream_callback(stream* stm, const void *pFrame, unsigned int user)
{

	return 0;
}

int getxmvefeo(const char* Ip, unsigned int Port, const char* Name, const char* Password)
{
	device *dev = (device *)xm_alloc_device();
	if(dev==NULL)
	{
		printf("xm_alloc_device failed\n");
	}
	
	dev->ops->init(dev);
	{
		stLogin_Req req;
		strcpy(req.Ip, Ip);
		req.Port = Port;
		strcpy(req.User, Name);
		strcpy(req.Password, Password);
		
		stLogin_Rsp rsp;
		if(dev->ops->login(dev, &req, &rsp))
		{
			printf("login failed\n");
		}
	}

	{
		struct stGetConfig_Req req = {0,GET_ENCODE_CONFIG,0};
		struct stGetConfig_Rsp rsp;
		if(dev->ops->get_config(dev, &req, &rsp))
		{
			printf("get_config failed\n");
		}
	}

	/*{
		struct stOpenVideoStream_Req req = {0,0,0,(void*)_real_staream_callback};
		struct stOpenVideoStream_Rsp rsp;

		if(dev->ops->open_video_stream(dev, &req, &rsp))
		{
			printf("open_video_stream failed\n");
		}
	}*/

	{
		struct stOpenAudioStream_Req req = {0,0};
		struct stOpenAudioStream_Rsp rsp;

		if(dev->ops->open_audio_stream(dev, &req, &rsp))
		{
			printf("open_audio_stream failed\n");
		}
	}

	
	{
		struct stOpenAlarmStream_Req req = {0};
		struct stOpenAlarmStream_Rsp rsp;
		if(dev->ops->open_alarm_stream(dev, &req, &rsp))
		{
			printf("logout failed\n");
		}
	}

}

int main(int argc, char** argv)
{
	printf("frontplug, build time %s, Version %s\n", __TIME__ , PlugVerion);


	/*device *dev = (device *)xm_alloc_device();
	if(dev==NULL)
	{
		printf("xm_alloc_device failed\n");
	}
	
	dev->ops->init(dev);
	{
		stLogin_Req req = {"192.168.1.238", 34567, "user", "user"};
		stLogin_Rsp rsp;
		if(dev->ops->login(dev, &req, &rsp))
		{
			printf("login failed\n");
		}
	}


	{
		struct stOpenVideoStream_Req req = {0,0,0,0};
		struct stOpenVideoStream_Rsp rsp;

		if(dev->ops->open_video_stream(dev, &req, &rsp))
		{
			printf("logout failed\n");
		}

	}*/

	getxmvefeo("192.168.1.238", 34567, "user", "user");
	/*getxmvefeo("192.168.1.78", 34567, "admin", "");
	getxmvefeo("192.168.1.76", 34567, "user", "user");
	getxmvefeo("192.168.1.10", 34567, "admin", "");
	getxmvefeo("192.168.1.79", 34567, "admin", "");
	getxmvefeo("192.168.1.238", 34567, "user", "user");

	getxmvefeo("192.168.1.238", 34567, "user", "user");
	getxmvefeo("192.168.1.78", 34567, "admin", "");
	getxmvefeo("192.168.1.76", 34567, "user", "user");
	getxmvefeo("192.168.1.10", 34567, "admin", "");
	getxmvefeo("192.168.1.79", 34567, "admin", "");
	getxmvefeo("192.168.1.238", 34567, "user", "user");
*/

	while(1)
		sleep(1111);

	{
		stLogout_Req req;
		stLogout_Rsp rsp;
		//if(dev->ops->logout(dev, &req, &rsp))
		{
			printf("logout failed\n");
		}
	}






	return 0;

	semid2  = open_or_create_vsem("xxxxxxxxdddd0");
	semid3	= open_or_create_vsem("xxxxxxxxddddssss0");

	//post_task();




	while(1)
	{
		post_vsem(semid2);
		
		wait_vsem(semid3);
		printf("semid3 get ack\n");
	}
	
	sleep(1000);
	return 0;
}






























