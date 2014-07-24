#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string>
using namespace std;
#include <pthread.h>
#include "create_detached_thread.h"
#include "usersdk.h"
#include "stringutility.h"
#include "showstruct.h"

#include "jtsimplestackback.h"


int JT_CALL_TYPE stream_callback(int _callback_type, void* _data, void** _user)
{
	struct st_stream_data* data = (struct st_stream_data*)_data;
	int* user = (int*)*_user;
	
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
	else if(CALLBACK_TYPE_DEVICE_INIT==_callback_type)
	{
		printf("[stream_callback]open audio stream callback user %p\n", user);
		if(user)
		{
			*_user = NULL;
			delete user;
			return 0;
		}
	}
	else if(CALLBACK_TYPE_DEVICE_DELETED==_callback_type)
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
//int devtype = DEVICE_XM;

int gdevtype = DEVICE_JN;
char gip[64] = {"192.168.3.116"};
unsigned int gport = 20000;
char guser[64] = {"admin"};
char gpassword[64] = {"admin"};

void* func(void *)
{
	printf("func\n");

	int ret = jt_init(NULL);
	
	handle = jt_create_device(gdevtype);
	//handle = jt_create_device(DEVICE_XM);
	if(handle==NULL)
	{
		printf("jt_create_device %d failed\n", gdevtype);
	}
	else
	{
		printf("jt_create_device %d ok, %p\n", gdevtype, handle);
	}

	//struct stLogin_Req req = {{"192.168.0.171"}, 34567, {"admin"}, {""}, NULL};
	struct stLogin_Req req;// = {{"192.168.0.100"}, 8000, {"admin"}, {"admin"}, NULL};

	strcpy(req.Ip, gip);
	strcpy(req.User, guser);
	strcpy(req.Password, gpassword);
	req.Port = gport;
	
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
	reqc.Channel = 2;
	reqc.Codec = 0;
	reqc.Type = GET_ENCODE_CONFIG;
	jt_get_config(handle, &reqc, &rspc);
	
	reqc.Channel = -1;
	reqc.Codec = 0;
	reqc.Type = GET_EQU_INFO;
	jt_get_config(handle, &reqc, &rspc);

	reqc.Channel = -1;
	reqc.Codec = 0;
	reqc.Type = GET_SUB_EQU_INFO;
	jt_get_config(handle, &reqc, &rspc);
	struct jt_subequ_info *subequ_info = (struct jt_subequ_info *)rspc.Config;
	if(rspc.Size)
	{
		//subequ_info[0].subseq;
	}


	while(1)
	{
		struct stOpenVideoStream_Req req2;
		req2.Channel = 6;
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


		struct stOpenAudioStream_Req req5;
		struct stOpenAudioStream_Rsp rsp5;
		req5.DeviceHandle = (long long)handle;
		req5.Callback = stream_callback;
		req5.UserData = new int(2);
		req5.Channel = 0;
		jt_open_audio_stream(handle, &req5, &rsp5);

#if 0
		//转到预置点
		struct stPTZControl_Req req4;
		struct stPTZControl_Rsp rsp4;		
		req4.Channel = 0;//request->;
		req4.Action = JGOTO_PRESET;//NvrPtzConvert(request->PTZCommandType);
		req4.Speed = 7;
		req4.PresetNum = 1;
		/*memset(&req.SequenceGroup, 0, sizeof(req6.SequenceGroup));
		int i=0;
		for(; i<JPTZ_MAX_SEQUENCE_NUM && i<16 && i<request->sequenceNum; ++i)
		{
			req4.SequenceGroup[i] = request->sequenceGroup[i];	
		}
		req4.SequenceNum = 0;
		req4.TourNum = request->tourNum;
		req4.TourDelayTime = request->tourDelayTime; */

		//jt_ptz_control(handle, &req4, &rsp4);


		//开始巡航
		struct stPTZControl_Req req6;
		struct stPTZControl_Rsp rsp6;		
		req6.Channel = 0;//request->;
		req6.Action = JSTART_LOOP;//NvrPtzConvert(request->PTZCommandType);
		req6.Speed = 7;
		req6.PresetNum = 1;
		/*memset(&req6.SequenceGroup, 0, sizeof(req6.SequenceGroup));
		int i=0;
		for(; i<JPTZ_MAX_SEQUENCE_NUM && i<16 && i<request->sequenceNum; ++i)
		{
			req6.SequenceGroup[i] = request->sequenceGroup[i];	
		}*/
		req6.SequenceNum = 0;
		req6.TourNum = 1;
		req6.TourDelayTime = 5; 

		jt_ptz_control(handle, &req6, &rsp6);

		//停止巡航
		struct stPTZControl_Req req7;
		struct stPTZControl_Rsp rsp7;		
		req7.Channel = 0;//request->;
		req7.Action = JSTOP_LOOP;//NvrPtzConvert(request->PTZCommandType);
		req7.Speed = 7;
		req7.PresetNum = 1;
		/*memset(&req7.SequenceGroup, 0, sizeof(req7.SequenceGroup));
		int i=0;
		for(; i<JPTZ_MAX_SEQUENCE_NUM && i<16 && i<request->sequenceNum; ++i)
		{
			req7.SequenceGroup[i] = request->sequenceGroup[i];	
		}*/
		req7.SequenceNum = 0;
		req7.TourNum = 1;
		req7.TourDelayTime = 5; 

		jt_ptz_control(handle, &req7, &rsp6);
#endif
		//while(1)
			sleep(5);

		//关闭视频流
		struct stCloseVideoStream_Req req0;
		struct stCloseVideoStream_Rsp rsp0;
		req0.StreamHandle = rsp2.StreamHandle;

		jt_close_video_stream((void*)rsp2.StreamHandle, &req0, &rsp0);
		
		sleep(5);
	}
	
	struct stLogout_Req req9;
	struct stLogout_Rsp rsp9;
	jt_logout(handle, &req9, &rsp9);

	return NULL;
}

int main(int argc, char** argv)
{
	showstruct2();

#ifdef _LP64
	printf("%s, time %s, 64bit system\n", argv[0], __TIME__);
#else
	printf("%s, time %s, 32bit system\n", argv[0], __TIME__);
#endif

	needshowstackbackwhencrack();

	FILE *cfgfp = fopen("./plugtest.cfg", "r");
	if(cfgfp)
	{	
		char str[256] = {0};
		fgets(str, 256, cfgfp);

		char* t = str;
		char *ctype = strsep_s(&t, " \t\n\r");
		char *cip = strsep_s(&t, " \t\n\r");
		char *cport = strsep_s(&t, " \t\n\r");
		char *cuser = strsep_s(&t, " \t\n\r");
		char *cpassword = strsep_s(&t, " \t\n\r");

		printf("ctype %s, cip %s, cport %s, cuser %s, cpassword %s\n"
			, ctype, cip, cport, cuser, cpassword);

		if(string(ctype)=="sn") gdevtype = DEVICE_SN;
		else if(string(ctype)=="xm") gdevtype = DEVICE_XM;
		else if(string(ctype)=="hk") gdevtype = DEVICE_HK;
		else if(string(ctype)=="dh") gdevtype = DEVICE_DH;
		else if(string(ctype)=="jn") gdevtype = DEVICE_JN;

		strcpy(gip, cip);
		gport = (unsigned int)atoi(cport);
		strcpy(guser, cuser);
		if(cpassword)
			strcpy(gpassword, cpassword);
		else
			strcpy(gpassword, "");

		printf("gdevtype %d, gip %s, gport %d, guser %s, gpassword %s\n"
			, gdevtype, gip, gport, guser, gpassword);

	}
	
	pthread_t tid;
	//create_detached_thread(&tid, func, NULL);
	func(NULL);

	while(1)sleep(20000);
	return 0;
}
