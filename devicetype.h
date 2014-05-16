#ifndef DEVICE_TYPE_H
#define DEVICE_TYPE_H

//对象定义
#define OBJECT_TYPE_DEVICE	1
#define OBJECT_TYPE_CHANNEL	2
#define OBJECT_TYPE_INPUT	3
#define OBJECT_TYPE_OUTPUT	4
#define OBJECT_TYPE_STREAM	5

//设备类型定义
#define DEVICE_JT              1U	//
#define DEVICE_DH              2U	//大华
#define DEVICE_HK              3U	//海康
#define DEVICE_SN              4U	//索尼
#define DEVICE_XM              5U	//雄迈
#define DEVICE_HH              6U	//黄河
#define DEVICE_JY              7U	//景阳

#define VIDEO_STREAM_DATA           0x0000000000000001
#define AUDIO_STREAM_DATA           0x0000000000000002
#define ALARM_STREAM_DATA           0x0000000000000004
#define RGB_STREAM_DATA             0x0000000000000008

#define VIDEO_ENCODE_UNKNOW                                 0x0008
#define VIDEO_ENCODE_JPEG                                   0x0010
#define VIDEO_ENCODE_HIK_JPEG                               0x0011
#define VIDEO_ENCODE_DAHUA_JPEG                             0x0012

#define VIDEO_ENCODE_VIDEO_MPEG4                            0x0020
#define VIDEO_ENCODE_HIK_MPEG4                              0x0021
#define VIDEO_ENCODE_DAHUA_MPEG4                            0x0022

#define VIDEO_ENCODE_VIDEO_MPEG2                            0x0040
#define VIDEO_ENCODE_HIK_MPEG2                              0x0041

#define VIDEO_ENCODE_VIDEO_H264                             0x0080
#define VIDEO_ENCODE_HIK_H264                               0x0081
#define VIDEO_ENCODE_DAHUA_H264                             0x0082

#define UNKNOWN_FRAME                                        0
#define I_FRAME                                              1
#define B_FRAME                                              2
#define P_FRAME                                              3

#define AUDIO_G711                                        1
#define AUDIO_G711A                                       10

#define ALARM_TYPE_VIDEO_MOTION                          1000 //移动侦测
#define ALARM_TYPE_VIDEO_LOSS                            1001 //视频丢失
#define ALARM_TYPE_VIDEO_BLIND                           1002 //遮挡
#define ALARM_TYPE_INPUT                                 1003 //输入
#define ALARM_TYPE_STORAGE_NOT_EXIST                     1004 //储存不存在


enum{JPTZ_UP=0, JPTZ_RIGHT_UP, JPTZ_RIGHT, JPTZ_RIGHT_DOWN, 
     JPTZ_DOWN, JPTZ_LEFT_DOWN, JPTZ_LEFT, JPTZ_LEFT_UP, 
     JPTZ_STOP,
     JPTZ_PUSH_FAR, JPTZ_PULL_NEAR, 
     JPTZ_IRIS_ADD, JPTZ_IRIS_SUB, 
	 JPTZ_FOCUS_FAR, JPTZ_FOCUS_NEAR , 
     JSET_PRESET, JCLEAR_PRESET,JGOTO_PRESET,
     JADD_TO_LOOP,JDEL_FROM_LOOP,JSTART_LOOP,JSTOP_LOOP,JREMOVE_LOOP,
     JPTZ_RESET
};

struct encode_info
{
	int enable;
	int encodetype;
	int fps;
	int width;
	int height;
	int quality;
	int bitrate;
	int bitratectl;
	int gop;
};

struct st_stream_data
{
	unsigned int streamtype;
	char* pdata;
	int   datalen;
	union 
	{
		struct
		{
			unsigned int encode;
			unsigned int frametype;
			unsigned int width;
			unsigned int height;
			float        fps;
			unsigned int bitrate;
		}video_stream_info;
		struct
		{
			unsigned int  encode;
			unsigned char channel;
			int           samples;
			unsigned char depth;
			unsigned int  reserved;//bitrate,  bps
		}audio_stream_info;
		struct
		{	
			int           channelid;
			int           reason;
		}alarm_stream_info;
	}stream_info;
	unsigned long long llbegintime;
	unsigned long long llrelativetimetick;	//数据相对时间. 100ns
	int year;					// 时标:年
	int month; 					// 时标:月
	int day;					// 时标:日
	int hour;					// 时标:时
	int minute;					// 时标:分
	int second;					// 时标:秒

	unsigned int reserved[3];
};

//如果是停止的回调，用户可能会修改*user的值
typedef int (CALL_TYPE *jt_stream_callback)(int callback_type, void* data, void** user);


//回调事件类型
#define CALLBACK_TYPE_VIDEO_STREAM               1     //视频数据
#define CALLBACK_TYPE_AUDIO_STREAM               2     //音频流数据
#define CALLBACK_TYPE_ALARM_STREAM               3     //报警流数据

#define CALLBACK_TYPE_VIDEO_STREAM_OPENED        10     //视频流流打开了
#define CALLBACK_TYPE_VIDEO_STREAM_CLOSEED       11     //视频流关闭了

#define CALLBACK_TYPE_AUDIO_STREAM_OPENED        12     //音频流打开了
#define CALLBACK_TYPE_AUDIO_STREAM_CLOSEED       13     //音频流关闭了

#define CALLBACK_TYPE_ALARM_STREAM_OPENED        14     //报警流打开了
#define CALLBACK_TYPE_ALARM_STREAM_CLOSEED       15     //报警流关闭了

#define CALLBACK_TYPE_VIDEO_INfO_CHANGED         16     //视频流参数发生变化
#define CALLBACK_TYPE_AUDIO_INfO_CHANGED         17     //音频流参数发生变化

#endif
