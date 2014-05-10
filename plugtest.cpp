#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include "create_detached_thread.h"
#include "usersdk.h"

int stream_callback(int _callback_type, void* _data, void** _user)
{

	struct st_stream_data* data = (struct st_stream_data*)_data;
	void* user = (void*)*_user;
	
	printf("stream_callback, data = %p, user %p\n", data, user);

	if(CALLBACK_TYPE_VIDEO_STREAM==_callback_type)
	{
		printf("video stream %s\n", __TIME__);
	}
	else if(CALLBACK_TYPE_AUDIO_STREAM==_callback_type)
	{
		printf("audio stream %s\n", __TIME__);
	}
	else if(CALLBACK_TYPE_ALARM_STREAM==_callback_type)
	{
		printf("alarm stream %s\n", __TIME__);
	}
	else if(CALLBACK_TYPE_VIDEO_STREAM_CLOSEED==_callback_type)
	{	
		//关闭视频流的操作
		printf("close video stream callback\n");
		/*if(info)
		{
			*user = NULL;
			delete info;
		}*/

		return 0;
	}
	else if(CALLBACK_TYPE_VIDEO_STREAM_OPENED==_callback_type)
	{
		printf("opened video stream callback\n");
		/*if(info)
		{
			*user = NULL;
			delete info;
			return 0;
		}*/
	}
	else if(CALLBACK_TYPE_AUDIO_STREAM_CLOSEED==_callback_type)
	{
		printf("close audio stream callback\n");
		/*if(info)
		{
			*user = NULL;
			delete info;
		}*/
		
		return 0;
	}
	else if(CALLBACK_TYPE_AUDIO_STREAM_OPENED==_callback_type)
	{
		printf("open audio stream callback\n");
		/*if(info)
		{
			*user = NULL;
			delete info;
			return 0;
		}*/
	}

	return 0;
}

void* handle= NULL;
int devtype = DEVICE_DH;

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

	
	struct stLogin_Req req = {{"192.168.3.65"}, 37777, {"admin"}, {"admin"}, NULL};
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

	while(1)
	{
		struct stOpenVideoStream_Req req2;
		req2.Channel = 1;
		req2.Codec = 0;
		req2.Callback = stream_callback;
		req2.UserData = new int(1);
		struct stOpenVideoStream_Rsp rsp2;
		jt_open_video_stream(handle, &req2, &rsp2);

		struct stOpenAlarmStream_Req req3;
		struct stOpenAlarmStream_Rsp rsp3;
		req3.Callback = stream_callback;
		req3.UserData = new int(2);
		jt_open_alarm_stream(handle, &req3, &rsp3);

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

int main(int argc, char** argv)
{
	printf("time %s\n", __TIME__);

	pthread_t tid; 
	create_detached_thread(&tid, func, NULL);

	sleep(20000);
	return 0;
}
