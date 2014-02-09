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
			printf("|%2d   sizeof(%20s) %3d_____________________\n",before##after, #before, sizeof(st.before)); \
				break;  \
		} \
		else \
			printf("|%d\n",before##after); \
	}

int showstruct()
{
	printf("sizeof()%d, %d\n", sizeof(struct stInterProcessCommandRequest),sizeof(struct stInterProcessCommandAck));
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
