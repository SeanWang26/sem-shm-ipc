#ifndef PLUG_ERROR_H
#define PLUG_ERROR_H

#define SUCCESS							0 

#define DEVICE_NULL_FAILED				-3 //������豸ָ��Ϊ0
#define DEVICE_NO_FOUND				    -4 //δ�ҵ���Ӧ�豸
#define ALLOC_DEVICE_FAILED				-5 //�����豸ʧ��
#define ALLOC_CHANNEL_FAILED			-6 //����ͨ��ʧ��
#define ALLOC_STREAM_FAILED				-7 //������ʧ��

#define INIT_FAILED						-8  //��ʼ��ʧ��
#define LOGIN_FAILED					-9  //��½ʧ��
#define LOGOUT_FAILED					-10 //ע��ʧ��

#define OPEN_VIDEO_STREAM_FAILED		-11  //����Ƶ��ʧ��
#define SET_VIDEO_CALLBACK_FAILED		-12 //������Ƶ�ص�ʧ��
#define CLOSE_VIDEO_STREAM_FAILED		-13  //�ر���Ƶ��ʧ��
#define OPEN_AUDIO_STREAM_FAILED		-14  //�ر���Ƶ��ʧ��

#define CLOSE_AUDIO_STREAM_FAILED       -15  //�ر���Ƶ��ʧ��
#define GET_CONFIG_FAILED               -16  //��ȡ����ʧ��
#define OPEN_ALARM_STREAM_FAILED        -17  //������ʧ��
#define CLOSE_ALARM_STREAM_FAILED       -18  //�ر���ʧ��
#define UNKONWN_PTZ_COMMAND             -19  //δ֪��ptz����
#define NOT_SUPPROT_PTZ_COMMAND         -21  //δ֪��ptz����

#define VIDEO_STREAM_NOT_EXIST          -22  //��Ƶ��������

#define START_TALK_FAILED               -40  //��Ƶ��������
#define STOP_TALK_FAILED                -41  //��Ƶ��������
#define SEND_TALK_DATA_FAILED           -42  //��Ƶ��������

#define	INVALID_CHANNEL_NO				-100 //����ȷ��ͨ����
#define	INVALID_STREAM_NO				-101 //����ȷ������

#define NOT_IMPLEMENT                   -500 //δʵ��
#endif
