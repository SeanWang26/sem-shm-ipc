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
//报警信息共享内存返回数据结构
struct sharedAlarmInfo
{
	char appendData[32];   //用于传递刷卡的卡号
	int  appendDataLength; //卡号的长度
	char appendDataUserName[64];   //用于卡的用户名字
	int  appendDataUserNameLength; //卡的用户名字长度
	int alarmType;		//报警类型0－信号量报警； 1－硬盘满； 2－信号丢失； 3－移动侦测；
						//4－硬盘未格式化； 5－读写硬盘出错； 6－遮挡报警； 7－制式不匹配； 8－非法访问；
					    //9－视频信号异常； 10-录像异常 
	int alarmInputNum;  //报警输入的端口号,即是信号量报警的触发报警输入端口号
	int videoChannel;   //当报警类型为2、3、6、9、10时有效，如videoChannel值为1表示第1个通道报警 
	int diskNum;        //发生报警的硬盘。当报警类型为1，4，5时有效，dwDiskNumber[0]值为1表示1号硬盘异常
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

