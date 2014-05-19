/*
* Copyright(C) 2010,Hikvision Digital Technology Co., Ltd 
* 
* File   name£ºCapPicture.cpp
* Discription£º
* Version    £º1.0
* Author     £ºpanyd
* Create Date£º2010_3_25
* Modification History£º
*/

#include "public.h"
#include "CapPicture.h"
#include <stdio.h>

/*******************************************************************
      Function:   Demo_Capture
   Description:   Capture picture.
     Parameter:   (IN)   none 
        Return:   0--success£¬-1--fail.   
**********************************************************************/
int Demo_Capture()
{
    NET_DVR_Init();
    long lUserID;
    //login
    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    lUserID = NET_DVR_Login_V30("172.4.1.162", 8000, "admin", "12345", &struDeviceInfo);
    if (lUserID < 0)
    {
        printf("pyd1---Login error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    //
    NET_DVR_JPEGPARA strPicPara = {0};
    strPicPara.wPicQuality = 2;
    strPicPara.wPicSize = 0;
    int iRet;
    iRet = NET_DVR_CaptureJPEGPicture(lUserID, struDeviceInfo.byStartChan, &strPicPara, "./ssss.jpeg");
    if (!iRet)
    {
        printf("pyd1---NET_DVR_CaptureJPEGPicture error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    //logout
    NET_DVR_Logout_V30(lUserID);
    NET_DVR_Cleanup();

    return HPR_OK;

}