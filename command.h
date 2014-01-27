#ifndef COMMUNICATION_WITH_NVR_H
#define COMMUNICATION_WITH_NVR_H

const int SUCCESS = 0;				//�ɹ�
const int UNKNOWN_ERROR = -1;		//δ֪����
const int ABNORMAL_STATUS = -2;		//exe�����ʱ�쳣
const int THREAD_EXCESS = -3;		//�̳߳���

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

//���豸
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

//�ر��豸
struct stLogout_Req
{
	long long				DeviceHandle;
};
struct stLogout_Rsp
{
	long long				DeviceHandle;
};

//��ȡ������Ϣ
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

//����Ƶ��
struct stOpenVideoStream_Req
{
	long long               DeviceHandle;
	int						Channel;
	int                     Codec;
	void*                   Callback;
	void*                   UserData;
};
struct stOpenVideoStream_Rsp
{
	long long		DeviceHandle;
	long			StreamHandle;
};

//�ر���Ƶ��
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

//����Ƶ��
struct stOpenAudioStream_Req
{
	long long				DeviceHandle;
	unsigned int			Channel;
	void*                   Callback;
	void*                   UserData;
};
struct stOpenAudioStream_Rsp
{
	long long				DeviceHandle;
	long					ChannelHandle;
};
//�ر���Ƶ��
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

//����������
struct stOpenAlarmStream_Req
{
	long long				DeviceHandle;
	void*                   Callback;
	void*                   UserData;	
};
struct stOpenAlarmStream_Rsp
{
	long long				DeviceHandle;
};
//�رձ�����
struct stCloseAlarmStream_Req
{
	long long				DeviceHandle;
};
struct stCloseAlarmStream_Rsp
{
	long long				DeviceHandle;
};

//�����Խ�
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
//�رնԽ�
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

//����
struct stReboot_Req
{
	long long				DeviceHandle;
};
struct stReboot_Rsp
{
	long long				DeviceHandle;
};
//�趨ʱ��
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

//��̨����
typedef enum {PTZ_LEFT,PTZ_LEFT_UP,PTZ_UP,PTZ_UP_RIGHT,PTZ_RIGHT,PTZ_DOWN_RIGHT,PTZ_DOWN,PTZ_DOWN_LEFT,PTZ_ABSOLUTE_POS,PTZ_STOP_MOVE,\
PTZ_RESET, PTZ_PUSH_FAR, PTZ_PULL_NEAR,PTZ_IRIS_SUB,PTZ_IRIS_ADD,PTZ_FOCUS_FAR,PTZ_FOCUS_NEAR}PTZ_DIRECTION;

//PTZ����
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

//�������
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
	//�������֪ͨ
	//AddCommander(Startup_Req);

	//�򿪹ر�
	AddCommander(Login_Req);
	AddCommander(Login_Rsp);
	AddCommander(Logout_Req);
	AddCommander(Logout_Rsp);
	//��ȡ������Ϣ
	AddCommander(GetConfig_Req);
	AddCommander(GetConfig_Rsp);
	//�򿪹ر���Ƶ��
	AddCommander(OpenVideoStream_Req);
	AddCommander(OpenVideoStream_Rsp);
	AddCommander(CloseVideoStream_Req);
	AddCommander(CloseVideoStream_Rsp);
	//�򿪹ر���Ƶ��
	AddCommander(OpenAudioStream_Req);
	AddCommander(OpenAudioStream_Rsp);
	AddCommander(CloseAudioStream_Req);
	AddCommander(CloseAudioStream_Rsp);
	//�򿪹رձ�����
	AddCommander(OpenAlarmStream_Req);
	AddCommander(OpenAlarmStream_Rsp);
	AddCommander(CloseAlarmStream_Req);
	AddCommander(CloseAlarmStream_Rsp);
	//�򿪹رնԽ�
	AddCommander(StartTalk_Req);
	AddCommander(StartTalk_Rsp);
	AddCommander(StopTalk_Req);
	AddCommander(StopTalk_Rsp);
	//�����豸
	AddCommander(Reboot_Req);
	AddCommander(Reboot_Rsp);
	//����ʱ��
	AddCommander(SetTime_Req);
	AddCommander(SetTime_Rsp);
	//PTZ����
	AddCommander(PTZControl_Req);
	AddCommander(PTZControl_Rsp);
	//�������
	AddCommander(SetAlarmOut_Req);
	AddCommander(SetAlarmOut_Rsp);
};

struct stCommander
{
	AddCommander(Head);
	union unCommander Cmd;
};

















#endif
