/*
 * Copyright(C) 2010,Custom Co., Ltd 
 *    FileName: Voice.cpp
 * Description: 
 *     Version: 1.0
 *      Author: panyadong
 * Create Date: 2010-6-7
 * Modification History£º
 */

#include "public.h"
#include "Voice.h"
#include <stdio.h>

#ifdef _WIN32
#elif defined(__linux__)
#include   <unistd.h> 

#endif
void CALLBACK fdwVoiceDataCallBack(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, DWORD pUser)
{

}

void CALLBACK fVoiceDataCallBack(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, void* pUser)
{
    static int icount = 0;
    //printf("   pyd---%5d Get voice data. size:%d.\n", icount, dwBufSize);
    icount++;
    //Send data to device after getting data.
    char pSendData[80] = {0};
    NET_DVR_VoiceComSendData(lVoiceComHandle, pSendData, 80);
}

int Demo_Voice()
{
    NET_DVR_Init();
    if (Demo_VoiceTrans() == HPR_ERROR)
    {
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }


    printf("pyd---Test Voice successfully!\n");
    NET_DVR_Cleanup();
    return HPR_OK;
}


int Demo_VoiceTrans()
{
    
    long lUserID;
    //login
    NET_DVR_DEVICEINFO struDeviceInfo;
    lUserID = NET_DVR_Login("172.4.4.221", 8000, "admin", "12345", &struDeviceInfo);
    if (lUserID < 0)
    {
        printf("pyd1---Login error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    while(1)
    {

   
    printf("in NET_DVR_StartVoiceCom_MR \n");
    //start voice
    long lVoiceHanle;
    lVoiceHanle = NET_DVR_StartVoiceCom_MR(lUserID, fdwVoiceDataCallBack, NULL);
	//lVoiceHanle = NET_DVR_StartVoiceCom_MR_V30(lUserID, 1, fVoiceDataCallBack, NULL);
    if (lVoiceHanle < 0)
    {
        printf("pyd---NET_DVR_StartVoiceCom_MR fail! %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        return HPR_ERROR;
    }

    printf("NET_DVR_StartVoiceCom_MR_V30 step 1\n");
    lVoiceHanle = NET_DVR_StartVoiceCom_MR_V30(lUserID, 0, fVoiceDataCallBack, NULL);
    if (lVoiceHanle < 0)
    {
        printf("pyd---NET_DVR_StartVoiceCom_MR_V30 fail %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        return HPR_ERROR;
    }
    
    printf("step 3\n");
#ifdef _WIN32
    Sleep(2000);  //millisecond
#elif  defined(__linux__)
    sleep(2);   //second
#endif

    if (!NET_DVR_StopVoiceCom(lVoiceHanle))
    {
        printf("pyd---NET_DVR_StopVoiceCom fail!\n");
        NET_DVR_Logout(lUserID);
        return HPR_ERROR;
    }
     }

    return HPR_OK;
}





