#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <assert.h>

#include <pthread.h>
#include "create_detached_thread.h"
#include "usersdk.h"
#include <signal.h>

int stream_callback(int _callback_type, void* _data, void** _user)
{
	struct st_stream_data* data = (struct st_stream_data*)_data;
	void* user = (void*)*_user;
	
	//printf("stream_callback, data = %p, user %p\n", data, user);

	if(CALLBACK_TYPE_VIDEO_STREAM==_callback_type)
	{
		//printf("[stream_callback]video stream %s\n", __TIME__);
	}
	else if(CALLBACK_TYPE_AUDIO_STREAM==_callback_type)
	{
		//printf("[stream_callback]audio stream %s\n", __TIME__);
	}
	else if(CALLBACK_TYPE_ALARM_STREAM==_callback_type)
	{
		//printf("[stream_callback]alarm stream %s\n", __TIME__);
	}
	else if(CALLBACK_TYPE_VIDEO_STREAM_CLOSEED==_callback_type)
	{	
		//关闭视频流的操作
		printf("[stream_callback]close video stream callback user %p\n", user);
		if(user)
		{
			*_user= NULL;
			delete user;
		}

		return 0;
	}
	else if(CALLBACK_TYPE_VIDEO_STREAM_OPENED==_callback_type)
	{
		//打开视频流的操作
		printf("[stream_callback]opened video stream callback user %p\n", user);
		if(user)
		{
			*_user = NULL;
			delete user;
			return 0;
		}
	}
	else if(CALLBACK_TYPE_AUDIO_STREAM_CLOSEED==_callback_type)
	{
		printf("[stream_callback]close audio stream callback user %p\n", user);
		if(user)
		{
			*_user = NULL;
			delete user;
		}
		
		return 0;
	}
	else if(CALLBACK_TYPE_AUDIO_STREAM_OPENED==_callback_type)
	{
		printf("[stream_callback]open audio stream callback user %p\n", user);
		if(user)
		{
			*_user = NULL;
			delete user;
			return 0;
		}
	}

	return 0;
}

void* handle= NULL;
//int devtype = DEVICE_HK;
//int devtype = DEVICE_DH;
int devtype = DEVICE_XM;



void* func(void *)
{
	printf("func\n");

	int ret = jt_init(NULL);
	
	handle = jt_create_device(devtype);
	//handle = jt_create_device(DEVICE_XM);
	if(handle==NULL)
	{
		printf("jt_create_device %d failed\n", devtype);
	}
	else
	{
		printf("jt_create_device %d ok, %p\n", devtype, handle);
	}

	struct stLogin_Req req = {{"192.168.3.71"}, 34567, {"admin"}, {""}, NULL};
	struct stLogin_Rsp rsp;
	ret = jt_login(handle, &req, &rsp);
	if(ret)
	{
		printf("jt_login failed %d\n", ret);
	}
	else
	{
		printf("jt_login ok %d\n", ret);
	}

	struct stGetConfig_Req reqc;
	struct stGetConfig_Rsp rspc;
	reqc.Channel = 0;
	reqc.Codec = 0;
	reqc.Type = GET_ENCODE_CONFIG;

	jt_get_config(handle, &reqc, &rspc);


	while(1)
	{
		struct stOpenVideoStream_Req req2;
		req2.Channel = 0;
		req2.Codec = 0;
		req2.Callback = stream_callback;
		req2.UserData = new int(1);
		struct stOpenVideoStream_Rsp rsp2;
		jt_open_video_stream(handle, &req2, &rsp2);

		/*struct stOpenAlarmStream_Req req3;
		struct stOpenAlarmStream_Rsp rsp3;
		req3.Callback = stream_callback;
		req3.UserData = new int(2);
		jt_open_alarm_stream(handle, &req3, &rsp3);

		struct stOpenAudioStream_Req req5;
		struct stOpenAudioStream_Rsp rsp5;
		req5.DeviceHandle = (long long)handle;
		req5.Callback = stream_callback;
		req5.UserData = new int(2);
		req5.Channel = 0;
		//jt_open_audio_stream(handle, &req5, &rsp5);
*/

		sleep(5);

		//关闭视频流
		struct stCloseVideoStream_Req req4;
		struct stCloseVideoStream_Rsp rsp4;
		req4.StreamHandle = rsp2.StreamHandle;

		jt_close_video_stream((void*)rsp2.StreamHandle, &req4, &rsp4);
		
		sleep(5);
	}
	
	struct stLogout_Req req9;
	struct stLogout_Rsp rsp9;
	jt_logout(handle, &req9, &rsp9);

	return NULL;
}
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
	char strKillTheSamePortLnvrCmd[255];

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

int main(int argc, char** argv)
{
	printf("time %s\n", __TIME__);

	pthread_t tid; 

	signal(SIGQUIT, HandleException);
	signal(SIGKILL, HandleException);
	signal(SIGFPE, HandleException);
	signal(SIGSEGV, HandleException);
	signal(SIGTERM,HandleException);
	signal(SIGPIPE, SIG_IGN);

	
	create_detached_thread(&tid, func, NULL);
	//func(NULL);

	sleep(20000);
	return 0;
}
