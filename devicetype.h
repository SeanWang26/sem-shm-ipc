#ifndef DEVICE_TYPE_H
#define DEVICE_TYPE_H

#if (defined(WIN32) || defined(WIN64))

#ifdef JT_FRONT_USER_MODULE_EXPORTS
#define JT_FRONT_API __declspec(dllexport)
#else
#define JT_FRONT_API __declspec(dllimport)
#endif

#ifndef JT_CALL_TYPE
#define JT_CALL_TYPE	__stdcall  //__cdecl
#endif

#else //linux platform

#ifndef JT_FRONT_API
#define JT_FRONT_API
#endif
#ifndef JT_CALL_TYPE
#define JT_CALL_TYPE
#endif

#endif


//������
#define OBJECT_TYPE_DEVICE	1
#define OBJECT_TYPE_CHANNEL	2
#define OBJECT_TYPE_INPUT	3
#define OBJECT_TYPE_OUTPUT	4
#define OBJECT_TYPE_STREAM	5

//�豸���Ͷ���
#define DEVICE_JT              1U	//
#define DEVICE_DH              2U	//��
#define DEVICE_HK              3U	//����
#define DEVICE_SN              4U	//����
#define DEVICE_XM              5U	//����
#define DEVICE_HH              6U	//�ƺ�
#define DEVICE_JY              7U	//����

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

#define VIDEO_ENCODE_UNKOWN                                 0xF000


#define UNKNOWN_FRAME                                        0
#define I_FRAME                                              1
#define B_FRAME                                              2
#define P_FRAME                                              3

#define AUDIO_G711                                       1
#define AUDIO_G711A                                      10
#define AUDIO_G711U                                      11
#define AUDIO_OGG_VORBIT                                 12
#define AUDIO_G276                                       13
#define AUDIO_AAC                                        14

#define AUDIO_ENCODE_UNKOWN                              9999


#define ALARM_TYPE_VIDEO_MOTION                          1000 //�ƶ����
#define ALARM_TYPE_VIDEO_LOSS                            1001 //��Ƶ��ʧ
#define ALARM_TYPE_VIDEO_BLIND                           1002 //�ڵ�
#define ALARM_TYPE_INPUT                                 1003 //����
#define ALARM_TYPE_STORAGE_NOT_EXIST                     1004 //���治����
#define ALARM_TYPE_UNKNOWN                               9999 //δ֪


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
	int bitratectl;//�ɱ�???
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
			int           frequency;
			int           bitrate;
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
	unsigned long long llrelativetimetick;	//�������ʱ��. 100ns
	int year;					// ʱ��:��
	int month; 					// ʱ��:��
	int day;					// ʱ��:��
	int hour;					// ʱ��:ʱ
	int minute;					// ʱ��:��
	int second;					// ʱ��:��

	unsigned int reserved[3];
};

//�����ֹͣ�Ļص����û����ܻ��޸�*user��ֵ
typedef int (JT_CALL_TYPE *jt_stream_callback)(int callback_type, void* data, void** user);

//�����ֹͣ�Ļص����û����ܻ��޸�*user��ֵ
typedef void* (JT_CALL_TYPE *jt_talk_callback)(int callback_type, void* data, void** user);


//�ص��¼�����
#define CALLBACK_TYPE_VIDEO_STREAM               1     //��Ƶ����
#define CALLBACK_TYPE_AUDIO_STREAM               2     //��Ƶ������
#define CALLBACK_TYPE_ALARM_STREAM               3     //����������

#define CALLBACK_TYPE_VIDEO_STREAM_OPENED        10     //��Ƶ��������
#define CALLBACK_TYPE_VIDEO_STREAM_CLOSEED       11     //��Ƶ���ر���

#define CALLBACK_TYPE_AUDIO_STREAM_OPENED        12     //��Ƶ������
#define CALLBACK_TYPE_AUDIO_STREAM_CLOSEED       13     //��Ƶ���ر���

#define CALLBACK_TYPE_ALARM_STREAM_OPENED        14     //����������
#define CALLBACK_TYPE_ALARM_STREAM_CLOSEED       15     //�������ر���

#define CALLBACK_TYPE_VIDEO_INfO_CHANGED         16     //��Ƶ�����������仯
#define CALLBACK_TYPE_AUDIO_INfO_CHANGED         17     //��Ƶ�����������仯

#define CALLBACK_TYPE_TALK_OPNED                 20     //�Խ���ʼ��
#define CALLBACK_TYPE_TALK_CLOSED                21     //�Խ�������
#define CALLBACK_TYPE_TALK_THREAD_TERMAL         22     //�Խ��߳��ս���

#define CALLBACK_TYPE_DEVICE_INIT                98     //�豸��ɾ����
#define CALLBACK_TYPE_DEVICE_DELETED             99     //�豸��ɾ����
#endif
