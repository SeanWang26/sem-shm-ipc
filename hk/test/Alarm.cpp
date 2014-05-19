/*
 * Copyright(C) 2010,Hikvision Digital Technology Co., Ltd 
 * 
 * File   name��Alarm.cpp
 * Discription��
 * Version    ��1.0
 * Author     ��panyd
 * Create Date��2010_3_25
 * Modification History��
 */

#include <stdio.h>
#include <string.h>
#include "public.h"
#include "Alarm.h"

#ifdef _WIN32
#elif defined(__linux__) || defined(__APPLE__)
#include   <unistd.h> 
#endif


void CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
	int i;
	NET_DVR_ALARMINFO_V30 struAlarmInfo;
	memcpy(&struAlarmInfo, pAlarmInfo, sizeof(NET_DVR_ALARMINFO_V30));
	//printf("lCommand is %d, alarm type is %d\n", lCommand, struAlarmInfo.dwAlarmType);
	switch(lCommand) 
	{       
	case COMM_ALARM_V30:
		{
			switch (struAlarmInfo.dwAlarmType)
			{
			case 3: //�ƶ���ⱨ��
				for (i=0; i<16; i++)   //#define MAX_CHANNUM   16  //���ͨ����
				{
					if (struAlarmInfo.byChannel[i] == 1)
					{
						printf("Motion detection %d\n", i+1);
					}
				}       
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}


//Alarm Test
int Demo_Alarm()
{
//     if (Demo_AlarmListen() == HPR_ERROR)
//     {
//         return HPR_ERROR;
//     }

	if (Demo_AlarmFortify() == HPR_ERROR)
	{
		return HPR_ERROR;
	}

    return HPR_OK;
}

//Alarm listen
int Demo_AlarmListen()
{
    int iRet;
    
    //Init
    iRet = NET_DVR_Init();
    if (iRet == FALSE)
    {
        printf("   pyd---Alarm. NET_DVR_Init fail!\n");
        return HPR_ERROR;
    }

    //close
    iRet = NET_DVR_StopListen();
    if (iRet == FALSE)
    {
        printf("   pyd---Alarm. NET_DVR_StopListen fail!%d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    //clean up
    NET_DVR_Cleanup();

    return HPR_ERROR;
}

// Fortify ����
int Demo_AlarmFortify()
{
	LONG lUserID;
	NET_DVR_DEVICEINFO_V30 struDeviceInfo;
	lUserID = NET_DVR_Login_V30("172.9.6.20", 8000, "admin", "12345", &struDeviceInfo);
	if (lUserID < 0)
	{
		printf("Login error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
	}

	//���ñ����ص�����
	NET_DVR_SetDVRMessageCallBack_V30(MessageCallback, NULL);

	//���ò���
	LONG lHandle;
	lHandle = NET_DVR_SetupAlarmChan_V30(lUserID);
	if (lHandle < 0)
	{
		printf("NET_DVR_SetupAlarmChan_V30 error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
	}

#ifdef _WIN32
    Sleep(5000);  //millisecond
#elif  defined(__linux__) || defined(__APPLE__)
    sleep(500);   //second
#endif

	//���������ϴ�ͨ��
	if (!NET_DVR_CloseAlarmChan_V30(lHandle))
	{
		printf("NET_DVR_CloseAlarmChan_V30 error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
	}

	//ע���û�
	NET_DVR_Logout(lUserID);
	//�ͷ�SDK��Դ
	NET_DVR_Cleanup();
	return HPR_OK;
}
