#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <execinfo.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

//#include "create_detached_thread.h"
#include "jtprintf.h"

#include "commnvr.h"
#include "showstruct.h"


#define PlugVerion "0.0"

/*
char* semname = "wwww000000000000000000000";

int getxmvefeo(const char* Ip, unsigned int Port, const char* Name, const char* Password);

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
	getxmvefeo("192.168.1.74", 34567, "admin", "");



	printf("%s\n", (char*)cmd);
	int cnt = 0 ;
	while(1)
	{	sleep(11111);
		wait_vsem(semid2);
		
		printf("semid2 get ack %d\n", ++cnt);

		post_vsem(semid3);

	}
	
	return 0;
}


void post_task()
{
	//pthread_t tid = 0;
//	return	create_detached_thread(&tid, task_work2, (void*)"printf");
}

int _real_staream_callback(stream* stm, const void *pFrame, unsigned int user)
{
	printf("_real_staream_callback\n");
	return 0;
}

int getxmvefeo(const char* Ip, unsigned int Port, const char* Name, const char* Password)
{
	device *dev = (device *)xm_alloc_device();
	if(dev==NULL)
	{
		printf("xm_alloc_device failed\n");
	}
	
	//dev->ops->init(dev);

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
		stLogin_Req req;
		strcpy(req.Ip, Ip);
		req.Port = Port;
		strcpy(req.User, Name);
		strcpy(req.Password, Password);
		
		stLogin_Rsp rsp;
		if(dev->ops->login(dev, &req, &rsp))
		{
			printf("login failed\n");
			exit(0);
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


	while(1)
	{
		struct stOpenVideoStream_Req req = {0,0,0,(void*)_real_staream_callback};
		struct stOpenVideoStream_Rsp rsp;

		if(dev->ops->open_video_stream(dev, &req, &rsp))
		{
			printf("open_video_stream failed\n");
		}

		sleep(5);
		struct stCloseVideoStream_Req req2 = {0,0,0,0};
		struct stCloseVideoStream_Rsp rsp2;

		req2.StreamHandle = rsp.StreamHandle;
		printf("req2.StreamHandle %d\n", req2.StreamHandle);
		if(dev->ops->close_video_stream(dev, &req2, &rsp2))
		{
			printf("close_video_stream failed\n");
		}
		
		sleep(5);

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
*/

void ShowSignalType(int value)
{
	switch(value)
	{
		case 2:
			printf("\n SIGINT : 点击了Ctrl+C 关闭\n");
		break;
		case 3:
			printf("\n SIGQUIT \n");
		break;
		case 8:
			printf("\n SIGFPE : 浮点异常\n");
		break;
		case 9:
			printf("\n SIGKILL \n");
		break;
		case 11:
			printf("\n SIGSEGV : 段错误\n");
		break;
		case 13:
			printf("\n SIGPIPE : 管道异常\n");
		break;
		case 15:
			printf("\n SIGTERM : 被另一个进程关闭\n");
		break;
		default:
			printf("\n Unknow type %d \n", value);
	}
	
}

void HandleException(int value)
{
	printf("\n#########handle_exception  catch ERROR %d ,"
		" show use $kill -l  #########\n",value);
	void *stack_p[10];
	char **stack_info;
	int size = 0;

	ShowSignalType(value);
	
	size = backtrace(stack_p, sizeof(stack_p));
	stack_info = backtrace_symbols(stack_p, size);
	
	printf("%d stack frames.\n", size);

	for(int i = 0; i < size; i++)
	{
		printf ("%s\n", stack_info[i]);
	}

	//在守护里再输出
	fprintf(stderr, "\n#########handle_exception  catch ERROR %d ,"
		" show use $kill -l  #########\n",value);
	fprintf(stderr, "%d 行 stack frames.\n", size);
	
	for(int i = 0; i < size; i++)
	{
		fprintf(stderr,"%s\n", stack_info[i]);
	}

	if(stack_info)
	{
		free(stack_info);
		stack_info = NULL;
	}

	//ShowMemInfo();
	fflush(stdout);

	if(value == SIGINT || value == SIGQUIT
		|| value == SIGFPE || value == SIGKILL
		|| value == SIGSEGV || value == SIGTERM)
	{	
		//printf("kill -9 $(lsof -i:%d | sed -n '2p' | awk '{print $2}') \n", gLoginPort);
		//sprintf(strKillTheSamePortLnvrCmd, "kill -9 $(lsof -i:%d | sed -n '2p' | awk '{print $2}')", gLoginPort);
		//system(strKillTheSamePortLnvrCmd);
		assert(false);
	}
	
}

#include <time.h>
int main(int argc, char** argv)
{
#ifdef _LP64
		jtprintf("frontplug, build time %s, Version %s, %s, 64bit system, sizeof(time_t) %d\n"
			, __TIME__ , PlugVerion, argv[0], sizeof(time_t));
#else
		jtprintf("frontplug, build time %s, Version %s, %s, 32bit system, sizeof(time_t) %d\n"
		, __TIME__ , PlugVerion, argv[0], sizeof(time_t));
#endif

	char cmdpath[256] = {0};
	strcpy(cmdpath, argv[0]);
	int plen = strlen(cmdpath);
	for(; plen>=0; --plen)	
	{
		if(cmdpath[plen] == '/')
		{
			cmdpath[plen] = 0;
			break;
		}
	}

	if(plen<0)
	{
		jtprintf("get cmdpath failed\n");
		return -1;
	}

	if(chdir(cmdpath))
	{
		jtprintf("set current dir failed , errno %d, %s\n", errno, strerror(errno));
	}

	char curpath[1024] = {0};
	getcwd(curpath, 1024);
	jtprintf("current dir %s\n", curpath);

	int exenamelen = strlen(argv[0]);
	int exetype = 0;
	for(int i=exenamelen; i>0; --i)
	{
		if('/'== argv[0][i])
		{
			if(!memcmp((void*)(&(argv[0][i])+1), "zhamponxm_SDK.exe", sizeof("zhamponxm_SDK.exe")))
			{
				exetype = 34;
			}                                            
			else if(!memcmp((void*)(&(argv[0][i])+1), "zhdahua_SDK.exe", sizeof("zhdahua_SDK.exe")))
			{
				exetype = 3;
			}
			else if(!memcmp((void*)(&(argv[0][i])+1), "zhhaikang_SDK.exe", sizeof("zhhaikang_SDK.exe")))
			{
				exetype = 4;
			}

			jtprintf("it is %d\n", exetype);
		}
	}

	if(0==exetype)
	{
		jtprintf("unknown front exe\n");
		return -1;
	}

	int exeindex = 1;
	if(argc>1)
	{
		exeindex = atoi(argv[1]);
		jtprintf("exeindex %d\n", exeindex);
	}

	signal(SIGQUIT, HandleException);
	signal(SIGKILL, HandleException);
	signal(SIGFPE, HandleException);
	signal(SIGSEGV, HandleException);
	signal(SIGTERM,HandleException);
	signal(SIGPIPE, SIG_IGN);

	comm_nvr_init(exetype, exeindex);

	return 0;
}






























