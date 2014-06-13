#include <stdio.h>
#include <stdlib.h>
#include "showstruct.h"
#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include "./InterProcessCommunication.h"
#define culaoffset(type,before,after)  \
     	int before##after=offsetof(type,after)-offsetof(type,before); \
	while(before##after)  \
	{  \
		if(before##after) \
		{ \
			type st; \
			printf("|%2d   sizeof(%20s) %3zu_____________________\n", before##after, #before, sizeof(st.before)); \
				break;  \
		} \
		else \
			printf("|%d\n",before##after); \
	}

int showstruct()
{
	printf("sizeof(struct stInterProcessCommandRequest)=%zu, sizeof(struct stInterProcessCommandAck)=%zu\n", sizeof(struct stInterProcessCommandRequest),sizeof(struct stInterProcessCommandAck));
	printf("___________________________________________________\n");

	culaoffset(struct stInterProcessCommandRequest,sdkType,type)
	culaoffset(struct stInterProcessCommandRequest,type,usrName);
	culaoffset(struct stInterProcessCommandRequest,usrName,passWard);
	culaoffset(struct stInterProcessCommandRequest,passWard,ipAddress);
	culaoffset(struct stInterProcessCommandRequest,ipAddress,port);
	culaoffset(struct stInterProcessCommandRequest,port,userID);
	culaoffset(struct stInterProcessCommandRequest,userID,semName);
	culaoffset(struct stInterProcessCommandRequest,semName,shmName);
	culaoffset(struct stInterProcessCommandRequest,streamType,videoChannelNum);
	culaoffset(struct stInterProcessCommandRequest,videoChannelNum,videoCodecNum);
	culaoffset(struct stInterProcessCommandRequest,videoCodecNum,audioChannelNum);
	culaoffset(struct stInterProcessCommandRequest,audioChannelNum,semCallbackName);
	culaoffset(struct stInterProcessCommandRequest,semCallbackName,semSynCallbackName);
	culaoffset(struct stInterProcessCommandRequest,semSynCallbackName,shmCallbackName);
	culaoffset(struct stInterProcessCommandRequest,shmCallbackName,year);
	culaoffset(struct stInterProcessCommandRequest,year,month);
	culaoffset(struct stInterProcessCommandRequest,month,day);
	culaoffset(struct stInterProcessCommandRequest,day,hour);
	culaoffset(struct stInterProcessCommandRequest,hour,minute);
	culaoffset(struct stInterProcessCommandRequest,minute,second);
	culaoffset(struct stInterProcessCommandRequest,second,brightness);
	culaoffset(struct stInterProcessCommandRequest,brightness,contrast);
	culaoffset(struct stInterProcessCommandRequest,contrast,saturation);
	culaoffset(struct stInterProcessCommandRequest,saturation,hue);
	culaoffset(struct stInterProcessCommandRequest,hue,PTZCommandType);
	culaoffset(struct stInterProcessCommandRequest,PTZCommandType,PTZNum);
	culaoffset(struct stInterProcessCommandRequest,PTZNum,PTZSpeed);
	culaoffset(struct stInterProcessCommandRequest,PTZSpeed,presetNum);
	culaoffset(struct stInterProcessCommandRequest,presetNum,tourNum);
	culaoffset(struct stInterProcessCommandRequest,tourNum,tourDelayTime);
	culaoffset(struct stInterProcessCommandRequest,tourDelayTime,sequenceGroup);
	culaoffset(struct stInterProcessCommandRequest,sequenceGroup,sequenceNum);
	culaoffset(struct stInterProcessCommandRequest,sequenceNum,alarmOutNum);
	culaoffset(struct stInterProcessCommandRequest,alarmOutNum,alarmOutState);
	culaoffset(struct stInterProcessCommandRequest,alarmOutState,alarmoutLastTime);
	culaoffset(struct stInterProcessCommandRequest,alarmoutLastTime,recordChannel);
	culaoffset(struct stInterProcessCommandRequest,recordChannel,recorderFileType);
	culaoffset(struct stInterProcessCommandRequest,recorderFileType,recordSearchType);
	culaoffset(struct stInterProcessCommandRequest,recordSearchType,startTime);
	culaoffset(struct stInterProcessCommandRequest,startTime,endTime);
	culaoffset(struct stInterProcessCommandRequest,endTime,cardNumber);
	culaoffset(struct stInterProcessCommandRequest,cardNumber,recordFileName);
	culaoffset(struct stInterProcessCommandRequest,recordFileName,startPos);
	return 1;
}
/*
//������Ϣ�����ڴ淵�����ݽṹ
struct sharedAlarmInfo
{
	char appendData[32];   //���ڴ���ˢ���Ŀ���
	int  appendDataLength; //���ŵĳ���
	char appendDataUserName[64];   //���ڿ����û�����
	int  appendDataUserNameLength; //�����û����ֳ���
	int alarmType;		//��������0���ź��������� 1��Ӳ������ 2���źŶ�ʧ�� 3���ƶ���⣻
						//4��Ӳ��δ��ʽ���� 5����дӲ�̳��� 6���ڵ������� 7����ʽ��ƥ�䣻 8���Ƿ����ʣ�
					    //9����Ƶ�ź��쳣�� 10-¼���쳣 
	int alarmInputNum;  //��������Ķ˿ں�,�����ź��������Ĵ�����������˿ں�
	int videoChannel;   //����������Ϊ2��3��6��9��10ʱ��Ч����videoChannelֵΪ1��ʾ��1��ͨ������ 
	int diskNum;        //����������Ӳ�̡�����������Ϊ1��4��5ʱ��Ч��dwDiskNumber[0]ֵΪ1��ʾ1��Ӳ���쳣
};

*/

int showstruct2()
{
	culaoffset(struct sharedAlarmInfo,appendData,appendDataLength);
	culaoffset(struct sharedAlarmInfo,appendDataLength,appendDataUserName);
	culaoffset(struct sharedAlarmInfo,appendDataUserName,appendDataUserNameLength);
	culaoffset(struct sharedAlarmInfo,appendDataUserNameLength,alarmType);
	culaoffset(struct sharedAlarmInfo,alarmType,alarmInputNum);
	culaoffset(struct sharedAlarmInfo,alarmInputNum,videoChannel);
	culaoffset(struct sharedAlarmInfo,videoChannel,diskNum);
	
	return 1;
}

