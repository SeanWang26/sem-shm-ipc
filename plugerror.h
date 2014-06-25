#ifndef PLUG_ERROR_H
#define PLUG_ERROR_H

#define SUCCESS							0 

#define DEVICE_NULL_FAILED				-3 //传入的设备指针为0
#define DEVICE_NO_FOUND				    -4 //未找到对应设备
#define ALLOC_DEVICE_FAILED				-5 //分配设备失败
#define ALLOC_CHANNEL_FAILED			-6 //分配通道失败
#define ALLOC_STREAM_FAILED				-7 //分配流失败

#define INIT_FAILED						-8  //初始化失败
#define LOGIN_FAILED					-9  //登陆失败
#define LOGOUT_FAILED					-10 //注销失败

#define OPEN_VIDEO_STREAM_FAILED		-11  //打开视频流失败
#define SET_VIDEO_CALLBACK_FAILED		-12 //设置视频回调失败
#define CLOSE_VIDEO_STREAM_FAILED		-13  //关闭视频流失败
#define OPEN_AUDIO_STREAM_FAILED		-14  //打开音频流失败

#define CLOSE_AUDIO_STREAM_FAILED       -15  //关闭音频流失败
#define GET_CONFIG_FAILED               -16  //获取配置失败
#define OPEN_ALARM_STREAM_FAILED        -17  //开报警失败
#define CLOSE_ALARM_STREAM_FAILED       -18  //关报警失败
#define UNKONWN_PTZ_COMMAND             -19  //未知的ptz命令
#define NOT_SUPPROT_PTZ_COMMAND         -21  //未知的ptz命令

#define VIDEO_STREAM_NOT_EXIST          -22  //视频流不存在

#define START_TALK_FAILED               -40  //开对讲失败
#define STOP_TALK_FAILED                -41  //关对讲失败
#define SEND_TALK_DATA_FAILED           -42  //发送对讲数据失败

#define	INVALID_CHANNEL_NO_FAILED		-100 //不正确的通道号
#define	INVALID_STREAM_NO_FAILED		-101 //不正确的流号
#define	GET_VEDIO_EFFECT_FAILED		    -102 //不正确的通道号

#define	SET_SYSTEM_TIME_FAILED	        -151 //设置系统时间失败




#define	NOT_SURPORT	                    -499 //试图启动设备没有的功能
#define NOT_IMPLEMENT                   -500 //未实现
#endif
