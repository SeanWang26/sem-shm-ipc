#ifndef PLUG_ERROR_H
#define PLUG_ERROR_H

#define SUCCESS							0 

#define DEVICE_NULL_FAILED				-3 
#define DEVICE_NO_FOUND				    -4 
#define ALLOC_DEVICE_FAILED				-5
#define ALLOC_CHANNEL_FAILED			-6
#define ALLOC_STREAM_FAILED				-7  //

#define INIT_FAILED						-8   //初始化失败
#define LOGIN_FAILED					-9  //登陆失败
#define LOGOUT_FAILED					-10 //

#define OPEN_VIDEO_STREAM_FAILED		-11  //打开视频流失败
#define SET_VIDEO_CALLBACK_FAILED		-12 //设置视频回调失败
#define CLOSE_VIDEO_STREAM_FAILED		-13  //关闭视频流失败
#define CLOSE_AUDIO_STREAM_FAILED       -14  //关闭音频流失败
#define GET_CONFIG_FAILED               -15  //获取配置失败
#define OPEN_ALARM_STREAM_FAILED        -16  //开报警失败
#define CLOSE_ALARM_STREAM_FAILED       -17  //关报警失败
#define UNKONWN_PTZ_COMMAND             -18  //未知的ptz命令

#define	INVALID_CHANNEL_NO				-100 //不正确的通道号
#define	INVALID_STREAM_NO				-101 //不正确的流号

#endif
