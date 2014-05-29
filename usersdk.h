#ifndef JT_USER_SDK_H
#define JT_USER_SDK_H

/*#ifdef WIN32 //windows platform

#ifdef FRONT_USER_MODULE_EXPORTS
#define FRONT_API __declspec(dllexport)
#else
#define FRONT_API __declspec(dllimport)
#endif

#ifndef CALL_TYPE
#define CALL_TYPE	__stdcall  //__cdecl
#endif

#else //linux platform

#define FRONT_API
#ifndef CALL_TYPE
#define CALL_TYPE
#endif

#endif*/

#include "devicetype.h"
#include "command.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef int JT_CALL_TYPE (*jt_init_dl)(void* );

typedef void* JT_CALL_TYPE (*jt_create_device_dl)(unsigned int type);
typedef int JT_CALL_TYPE (*jt_distory_device_dl)(void* handle);

typedef int JT_CALL_TYPE (*jt_login_dl)(void* handle, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_login_ex_dl)(void* handle, int type, struct stLogin_Req *req, struct stLogin_Rsp *rsp);//创建一个新的设备，并登陆
typedef int JT_CALL_TYPE (*jt_logout_dl)(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_logout_ex_dl)(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);//退出登陆，删除设备

typedef int JT_CALL_TYPE (*jt_get_config_dl)(void* handle, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_set_config_dl)(void* handle, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_open_video_stream_dl)(void* handle, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_close_video_stream_dl)(void* handle, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_open_audio_stream_dl)(void* handle, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_close_audio_stream_dl)(void* handle, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_open_alarm_stream_dl)(void* handle, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_close_alarm_stream_dl)(void* handle, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_ptz_control_dl)(void* handle, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_set_system_time_dl)(void* handle, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_start_talk_dl)(void* handle, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_stop_talk_dl)(void* handle, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp);
typedef int JT_CALL_TYPE (*jt_send_talk_data_dl)(void* handle, stSendTalkData_Req *req, stSendTalkData_Rsp *rsp);






JT_FRONT_API int JT_CALL_TYPE jt_init(void* );

JT_FRONT_API void* JT_CALL_TYPE jt_create_device(unsigned int type);
JT_FRONT_API int JT_CALL_TYPE jt_distory_device(void* handle);

JT_FRONT_API int JT_CALL_TYPE jt_login(void* handle, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_login_ex(void* handle, int type, struct stLogin_Req *req, struct stLogin_Rsp *rsp);//创建一个新的设备，并登陆
JT_FRONT_API int JT_CALL_TYPE jt_logout(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_logout_ex(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);//退出登陆，删除设备

JT_FRONT_API int JT_CALL_TYPE jt_get_config(void* handle, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_set_config(void* handle, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_open_video_stream(void* handle, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_close_video_stream(void* handle, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_open_audio_stream(void* handle, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_close_audio_stream(void* handle, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_open_alarm_stream(void* handle, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_close_alarm_stream(void* handle, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_ptz_control(void* handle, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_set_system_time(void* handle, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_start_talk(void* handle, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_stop_talk(void* handle, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_send_talk_data(void* handle, stSendTalkData_Req *req, stSendTalkData_Rsp *rsp);

#ifdef __cplusplus
}
#endif

#endif
