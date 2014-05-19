   /*
* Copyright(C) 2010,Hikvision Digital Technology Co., Ltd 
* 
* File   name£ºGetStream.cpp
* Discription£º
* Version    £º1.0
* Author     £ºpanyd
* Create Date£º2010_3_25
* Modification History£º
*/

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include   <unistd.h> 
#endif

#include "HCNetSDK.h"
#include "public.h"
#include <stdio.h>
#include <time.h>


void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void* dwUser)
{
    printf("pyd---(private_v30)Get data,the size is %d,%d.\n", time(NULL), dwBufSize);
}

void CALLBACK g_HikDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser)
{
    printf("pyd---(private)Get data,the size is %d.\n", dwBufSize);
}

void CALLBACK g_StdDataCallBack(int lRealHandle, unsigned int dwDataType, unsigned char *pBuffer, unsigned int dwBufSize, unsigned int dwUser)
{
    printf("pyd---(rtsp)Get data,the size is %d.\n", dwBufSize);
}



/*******************************************************************
      Function:   Demo_GetStream
   Description:   preview(no "_V30")
     Parameter:   (IN)   none  
        Return:   0--successful£¬-1--fail¡£   
**********************************************************************/
int Demo_GetStream()
{

    NET_DVR_Init();
    long lUserID;
    //login
    NET_DVR_DEVICEINFO struDeviceInfo;
    lUserID = NET_DVR_Login("172.2.87.106", 8000, "admin", "12345", &struDeviceInfo);
    if (lUserID < 0)
    {
        printf("pyd1---Login error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    //Set callback function of getting stream.
    long lRealPlayHandle;
    NET_DVR_CLIENTINFO ClientInfo = {0};
#if (defined(_WIN32) || defined(_WIN_WCE))
    ClientInfo.hPlayWnd     = NULL;
#elif defined(__linux__)
    ClientInfo.hPlayWnd     = 0;
#endif

    ClientInfo.lChannel     = 1;  //channel NO
    //ClientInfo.lLinkMode  = 0x40000000; //Record when breaking network.
    ClientInfo.lLinkMode    = 0;
    ClientInfo.sMultiCastIP = NULL;

    lRealPlayHandle = NET_DVR_RealPlay(lUserID, &ClientInfo);
    if (lRealPlayHandle < 0)
    {
        printf("pyd1---NET_DVR_RealPlay_V30 error\n");
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }
    
    //Set callback function of getting stream.
    int iRet;
    iRet = NET_DVR_SetRealDataCallBack(lRealPlayHandle, g_HikDataCallBack, 0);
    if (!iRet)
    {
        printf("pyd1---NET_DVR_RealPlay_V30 error\n");
        NET_DVR_StopRealPlay(lRealPlayHandle);
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();  
        return HPR_ERROR;
    }


#ifdef _WIN32
    Sleep(5000);  //millisecond
#elif  defined(__linux__)
    sleep(500);   //second
#endif

    //stop
    NET_DVR_StopRealPlay(lRealPlayHandle);
    NET_DVR_Logout(lUserID);
    NET_DVR_Cleanup();
    return HPR_OK;

}

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    char tempbuf[256] = {0};
    switch(dwType) 
    {
    case EXCEPTION_RECONNECT:			//Ô¤ÀÀÊ±ÖØÁ¬
        printf("pyd----------reconnect--------%d\n", time(NULL));
        break;
    default:
        break;
    }
};


/*******************************************************************
      Function:   Demo_GetStream_V30
   Description:   preview(_V30)
     Parameter:   (IN)   none  
        Return:   0--successful£¬-1--fail¡£   
**********************************************************************/
int Demo_GetStream_V30(LONG lUserID)
{
    //Set callback function of getting stream.
    long lRealPlayHandle;
    NET_DVR_CLIENTINFO ClientInfo = {0};
#if (defined(_WIN32) || defined(_WIN_WCE)) || defined(__APPLE__)
    ClientInfo.hPlayWnd     = NULL;
#elif defined(__linux__)
   ClientInfo.hPlayWnd     = 0;
#endif

    ClientInfo.lChannel     = 2;  //channel NO.
    //ClientInfo.lLinkMode    = 0x40000000; //Record when breaking network.
    ClientInfo.lLinkMode    = 0;
    ClientInfo.sMultiCastIP = NULL;

    lRealPlayHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo, g_RealDataCallBack_V30, NULL, 0);
    //lRealPlayHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo, NULL, NULL, 0);
    if (lRealPlayHandle < 0)
    {
        printf("pyd1---NET_DVR_RealPlay_V30 error\n");
        return HPR_ERROR;
    }

    //Set rtsp callback function of getting stream.
    //NET_DVR_SetStandardDataCallBack(lRealPlayHandle, g_StdDataCallBack, 0);

#ifdef _WIN32
    Sleep(3000);  //millisecond
#elif  defined(__linux__) ||defined(__APPLE__)
    sleep(5);    //second
#endif

    //stop
    //NET_DVR_StopRealPlay(lRealPlayHandle);
	//NET_DVR_Logout(lUserID);
	//NET_DVR_Logout_V30(lUserID);
	//cleanup
	NET_DVR_Cleanup();
   
    return HPR_OK;
}



void Demo_SDK_Ability()
{
    NET_DVR_Init();
    NET_DVR_SDKABL struSDKABL = {0};
    if (NET_DVR_GetSDKAbility(&struSDKABL))
    {
        printf("SDK Max: %d\n", struSDKABL.dwMaxRealPlayNum);
        NET_DVR_Cleanup();
        return;
    }

    NET_DVR_Cleanup();
    return;
}

void Demo_SDK_Version()
{
    unsigned int uiVersion = NET_DVR_GetSDKBuildVersion();

    char strTemp[1024] = {0};
    sprintf(strTemp, "HCNetSDK V%d.%d.%d.%d\n", \
        (0xff000000 & uiVersion)>>24, \
        (0x00ff0000 & uiVersion)>>16, \
        (0x0000ff00 & uiVersion)>>8, \
        (0x000000ff & uiVersion));
    printf(strTemp);
}

