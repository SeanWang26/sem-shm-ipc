#ifndef COMMUNICATION_WITH_NVR_H
#define COMMUNICATION_WITH_NVR_H

const int SUCCESS = 0;				//成功
const int UNKNOWN_ERROR = -1;		//未知错误
const int ABNORMAL_STATUS = -2;		//exe信令处理超时异常
const int THREAD_EXCESS = -3;		//线程超量

const int ABNORMAL_RSP=0;

const int STARTUP_REQ=					0;
const int LOGIN_REQ=					1;
const int LOGOUT_REQ=					2; 
const int GET_ENCODE_REQ=				3;
const int OPEN_VIDEO_STREAM_REQ =		4;
const int CLOSE_VIDEO_STREAM_REQ =		5;
const int OPEN_AUDIO_STREAM_REQ =		6;
const int CLOSE_AUDIO_STREAM_REQ =		7;
const int OPEN_ALARM_REQ =		8;
const int CLOSE_ALARM_REQ =		9;

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

//获取码流信息
struct stGetEncode_Req
{
	long long				DeviceHandle;
	unsigned int			Channel;
};
struct stGetEncode_Rsp
{
	unsigned int			CodecNumber;
};

//打开视频流
struct stOpenVideoStream_Req
{
	long long				DeviceHandle;
	unsigned int			Channel;
	unsigned int			Codec;
	void*					Callback;
};
struct stOpenVideoStream_Rsp
{
	unsigned int			StreamId;
};

//关闭视频流
struct stCloseVideoStream_Req
{
	long long				DeviceHandle;
	unsigned int			Channel;
	unsigned int			Codec;
};
struct stCloseVideoStream_Rsp
{
	long long				DeviceHandle;
	unsigned int			StreamId;
};

//打开音频流
struct stOpenAudioStream_Req
{
	long long				DeviceHandle;
	unsigned int			Channel;
};
struct stOpenAudioStream_Rsp
{
	long long				DeviceHandle;
	unsigned int			Channel;
};
//关闭音频流
struct stCloseAudioStream_Req
{
	long long				DeviceHandle;
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
};
struct stStartTalk_Rsp
{
	long long				DeviceHandle;
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
	AddCommander(GetEncode_Req);
	AddCommander(GetEncode_Rsp);
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