#ifndef PLUG_ERROR_H
#define PLUG_ERROR_H

#define SUCCESS							0 

#define DEVICE_NULL_FAILED				-3 
#define DEVICE_NO_FOUND				    -4 
#define ALLOC_DEVICE_FAILED				-5
#define ALLOC_CHANNEL_FAILED			-6
#define ALLOC_STREAM_FAILED				-7  //

#define INIT_FAILED						-8   //��ʼ��ʧ��
#define LOGIN_FAILED					-9  //��½ʧ��
#define LOGOUT_FAILED					-10 //

#define OPEN_VIDEO_STREAM_FAILED		-11  //����Ƶ��ʧ��
#define SET_VIDEO_CALLBACK_FAILED		-12 //������Ƶ�ص�ʧ��
#define CLOSE_VIDEO_STREAM_FAILED		-13  //�ر���Ƶ��ʧ��
#define CLOSE_AUDIO_STREAM_FAILED       -14  //�ر���Ƶ��ʧ��
#define GET_CONFIG_FAILED               -15  //��ȡ����ʧ��
#define OPEN_ALARM_STREAM_FAILED        -16  //������ʧ��
#define CLOSE_ALARM_STREAM_FAILED       -17  //�ر���ʧ��
#define UNKONWN_PTZ_COMMAND             -18  //δ֪��ptz����

#define	INVALID_CHANNEL_NO				-100 //����ȷ��ͨ����
#define	INVALID_STREAM_NO				-101 //����ȷ������

#endif
