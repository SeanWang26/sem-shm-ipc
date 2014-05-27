#ifndef COMMUNICATION_WITH_NVR_H
#define COMMUNICATION_WITH_NVR_H

#include "devicetype.h"

#define  SUCCESS                  0				//成功
#define  UNKNOWN_ERROR   -1		//未知错误
#define  ABNORMAL_STATUS   -2	//exe信令处理超时异常
#define  THREAD_EXCESS   -3		//线程超量

#define  ABNORMAL_RSP  0

#define  STARTUP_REQ 					0
#define  LOGIN_REQ                      1
#define  LOGOUT_REQ 					2 
#define  GET_ENCODE_REQ 				3
#define  OPEN_VIDEO_STREAM_REQ  		4
#define  CLOSE_VIDEO_STREAM_REQ  		5
#define  OPEN_AUDIO_STREAM_REQ  		6
#define  CLOSE_AUDIO_STREAM_REQ  		7
#define  OPEN_ALARM_REQ                 8
#define  CLOSE_ALARM_REQ                9

//获取配置的子命令
#define GET_ENCODE_CONFIG               1//获取编码配置

//#pragma pack(4)
struct stHead
{
	unsigned int Size;
	unsigned int Sequence;
	unsigned int Type;
	unsigned int Result;
};

//打开设备
struct stLogin_Req
{
	char					Ip[32];
	unsigned int			Port;
	char					User[32];
	char					Password[32];
	jt_stream_callback      EventCallback;
};
struct stLogin_Rsp
{
	long long				DeviceHandle;
};

//关闭设备
struct stLogout_Req
{
	long long				DeviceHandle;
};
struct stLogout_Rsp
{
	long long				DeviceHandle;
};

//获取配置信息
struct stGetConfig_Req
{
	long long				DeviceHandle;
	unsigned int			Type;
	int	      		        Channel;
	int                     Codec;
};
struct stGetConfig_Rsp
{
	unsigned int			Type;
	unsigned int			Channel;
	unsigned int			Size;
	char*					Config;
};

//设置配置信息
struct stSetConfig_Req
{
	long long				DeviceHandle;
	unsigned int			Type;
	int	      		        Channel;
	int                     Codec;
	unsigned int			Size;
	char*					Config;
};
struct stSetConfig_Rsp
{
	long long				DeviceHandle;
	unsigned int			Type;
	int	      		        Channel;
	int                     Codec;
};

//打开视频流
struct stOpenVideoStream_Req
{
	long long               DeviceHandle;
	int						Channel;
	int                     Codec;
	jt_stream_callback      Callback;
	jt_stream_callback      Callback2;
	void*                   UserData;
};
struct stOpenVideoStream_Rsp
{
	long long		DeviceHandle;
	long			StreamHandle;
};

//关闭视频流
struct stCloseVideoStream_Req
{
	long long				DeviceHandle;
	long					StreamHandle;
	unsigned int			Channel;
	unsigned int			Codec;
};
struct stCloseVideoStream_Rsp
{
	long long				DeviceHandle;
	long					StreamHandle;
};

//打开音频流
struct stOpenAudioStream_Req
{
	long long				DeviceHandle;
	unsigned int			Channel;
	jt_stream_callback      Callback;
	void*                   UserData;
};
struct stOpenAudioStream_Rsp
{
	long long				DeviceHandle;
	long					ChannelHandle;
};
//关闭音频流
struct stCloseAudioStream_Req
{
	long long				DeviceHandle;
	long                    ChannelHandle;
	unsigned int			Channel;
};
struct stCloseAudioStream_Rsp
{
	long long				DeviceHandle;
	unsigned int			Channel;
};

//开启报警流
struct stOpenAlarmStream_Req
{
	long long				DeviceHandle;
	jt_stream_callback      Callback;
	void*                   UserData;	
};
struct stOpenAlarmStream_Rsp
{
	long long				DeviceHandle;
};
//关闭报警流
struct stCloseAlarmStream_Req
{
	long long				DeviceHandle;
};
struct stCloseAlarmStream_Rsp
{
	long long				DeviceHandle;
};

//开启对讲
struct stStartTalk_Req
{
	long long				DeviceHandle;
	int						Channel;
	jt_talk_callback        TalkCallback;            
};
struct stStartTalk_Rsp
{
	long long				DeviceHandle;
	void*                   TalkHandle;
	int						Channel;
};
//关闭对讲
struct stStopTalk_Req
{
	long long				DeviceHandle;
	int						Channel;
};
struct stStopTalk_Rsp
{
	long long				DeviceHandle;
	int						Channel;
};

struct stSendTalkData_Req
{
	long long				DeviceHandle;
	int						Channel;
	unsigned char*          Data;
	int                     DataLen;
};

struct stSendTalkData_Rsp
{
	long long				DeviceHandle;
	int						Channel;
};

//重启
struct stReboot_Req
{
	long long				DeviceHandle;
};
struct stReboot_Rsp
{
	long long				DeviceHandle;
};
//设定时间
struct stSetTime_Req
{
	long long				DeviceHandle;
	int						year;
	int						month;
	int						day;
	int						hour;
	int						minute;
	int						second;
};
struct stSetTime_Rsp
{
	long long				DeviceHandle;
	int						year;
	int						month;
	int						day;
	int						hour;
	int						minute;
	int						second;
};

enum { PTZ_MOVE=0, PTZ_STOP, PTZ_TOUR_START, PTZ_TOUR_STOP, SET_DEFAULT_POS, GOTO_PRESET, SET_PRESET, CLEAR_PRESET, SET_TOUR_LIST, AUTO_FOUS};

//云台方向
/*typedef enum {PTZ_LEFT,PTZ_LEFT_UP,PTZ_UP,PTZ_UP_RIGHT,PTZ_RIGHT,PTZ_DOWN_RIGHT,PTZ_DOWN,PTZ_DOWN_LEFT,PTZ_ABSOLUTE_POS,PTZ_STOP_MOVE,\
	PTZ_RESET, PTZ_PUSH_FAR, PTZ_PULL_NEAR,PTZ_IRIS_SUB,PTZ_IRIS_ADD,PTZ_FOCUS_FAR,PTZ_FOCUS_NEAR}PTZ_DIRECTION;
*/
//PTZ控制
struct stPTZControl_Req
{
	long long				DeviceHandle;
	int						Channel;
	unsigned int			Action;
	int						Direction;
	int						Speed;
	int						TourDelayTime;
	int						TourNum;
	int						PresetNum;
	char					PresetName[32];
	int						SequenceGroup[4];
	int						SequenceNum;
	int						Preset;
};
struct stPTZControl_Rsp
{
	long long				DeviceHandle;
	int						Channel;
	unsigned int			Action;
	int						Speed;
	int						Preset;					
};

//报警输出
struct stSetAlarmOut_Req
{
	long long				DeviceHandle;
	int						AlarmOutNum;
	int						Enable;
	int						AlarmOutState;
};
struct stSetAlarmOut_Rsp
{
	long long				DeviceHandle;
	int						AlarmOutNum;
	int						Enable;
	int						AlarmOutState;
};

#define AddCommander(STRUCT) struct	st##STRUCT	STRUCT

union unCommander
{
	//启动完毕通知
	//AddCommander(Startup_Req);

	//打开关闭
	AddCommander(Login_Req);
	AddCommander(Login_Rsp);
	AddCommander(Logout_Req);
	AddCommander(Logout_Rsp);
	//获取编码信息
	AddCommander(GetConfig_Req);
	AddCommander(GetConfig_Rsp);
	//打开关闭视频流
	AddCommander(OpenVideoStream_Req);
	AddCommander(OpenVideoStream_Rsp);
	AddCommander(CloseVideoStream_Req);
	AddCommander(CloseVideoStream_Rsp);
	//打开关闭音频流
	AddCommander(OpenAudioStream_Req);
	AddCommander(OpenAudioStream_Rsp);
	AddCommander(CloseAudioStream_Req);
	AddCommander(CloseAudioStream_Rsp);
	//打开关闭报警流
	AddCommander(OpenAlarmStream_Req);
	AddCommander(OpenAlarmStream_Rsp);
	AddCommander(CloseAlarmStream_Req);
	AddCommander(CloseAlarmStream_Rsp);
	//打开关闭对讲
	AddCommander(StartTalk_Req);
	AddCommander(StartTalk_Rsp);
	AddCommander(StopTalk_Req);
	AddCommander(StopTalk_Rsp);
	//重启设备
	AddCommander(Reboot_Req);
	AddCommander(Reboot_Rsp);
	//设置时间
	AddCommander(SetTime_Req);
	AddCommander(SetTime_Rsp);
	//PTZ控制
	AddCommander(PTZControl_Req);
	AddCommander(PTZControl_Rsp);
	//报警输出
	AddCommander(SetAlarmOut_Req);
	AddCommander(SetAlarmOut_Rsp);
};

struct stCommander
{
	AddCommander(Head);
	union unCommander Cmd;
};

















#endif
