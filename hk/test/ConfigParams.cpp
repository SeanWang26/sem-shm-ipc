/*
* Copyright(C) 2010,Hikvision Digital Technology Co., Ltd 
* 
* File   name£ºConfigParams.cpp
* Discription£º
* Version    £º1.0
* Author     £ºpanyadong
* Create Date£º2010_3_29
* Modification History£º
*/

#include "ConfigParams.h"
#include <stdio.h>
#include "public.h"


#include <iostream>
using namespace std;

//#define DEMO_DVRCONFIG_IP "172.2.87.101"
#define DEMO_DVRCONFIG_IP "172.4.4.221"

/*******************************************************************
      Function:   Demo_ConfigParams
   Description:   Set params.
     Parameter:   (IN)   none  
        Return:   0--successful£¬-1--fail¡£   
**********************************************************************/
int Demo_ConfigParams(LONG lUserID)
{
    //Demo_CompressParams_V30(lUserID);

    //Demo_GetDeviceState(lUserID);

    //Demo_DiskRaidInfo(lUserID);

    //if (Demo_CompressParams_V30() == HPR_ERROR)
    //{
    //    return HPR_ERROR;
    //}
    //Demo_Synchronous_IPC(lUserID);
    //Demo_IPCPasswd(lUserID);
    //Demo_GetDeviceNetUsingInfo(lUserID);
    //Demo_SetIPCNet(lUserID);

    //picture
    //Demo_PictureParams(lUserID);

    //IPC configure v40
    Demo_IPParaCfg_v31(lUserID);

    //IPC configure v40
    //Demo_IPParaCfg_V40(lUserID);

    //channel record status
    //Demo_ChannelRecordStatus(lUserID);

    return HPR_OK;
}

//NET_DVR_GET_COMPRESSCFG
int Demo_CompressParams()
{
    NET_DVR_Init();
    long lUserID;
    //login
    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    lUserID = NET_DVR_Login_V30(DEMO_DVRCONFIG_IP, 8000, "admin", "12345", &struDeviceInfo);
    if (lUserID < 0)
    {
        printf("pyd1---Login error\n");
        return HPR_ERROR;
    }

    int iRet;
    //set params
    DWORD uiReturnLen;
    NET_DVR_COMPRESSIONCFG struParams = {0};
    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_COMPRESSCFG, struDeviceInfo.byStartChan, \
        &struParams, sizeof(NET_DVR_COMPRESSIONCFG), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_COMPRESSCFG_V30 error.\n");
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }
    printf("pyd---size is %d\n", sizeof(NET_DVR_COMPRESSIONCFG_V30));

    //stop a minute.
    char cUserChoose;
    printf("pyd---press any key to set :");
    cin>>cUserChoose;

    //Set params.
    struParams.struRecordPara.dwVideoBitrate = 2;
    iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_COMPRESSCFG, struDeviceInfo.byStartChan, \
        &struParams, sizeof(NET_DVR_COMPRESSIONCFG));
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_SET_COMPRESSCFG_V30 error.\n");
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }

    printf("pyd---press any key to get :");
    cin>>cUserChoose;
    //Get params.
    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_COMPRESSCFG, struDeviceInfo.byStartChan, \
        &struParams, sizeof(NET_DVR_COMPRESSIONCFG), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_COMPRESSCFG_V30 error.\n");
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }
    printf("pyd---press any key to stop:");
    cin>>cUserChoose;

    printf("pyd---%d\n", struParams.struRecordPara.dwVideoBitrate);
    NET_DVR_Logout_V30(lUserID);
    NET_DVR_Cleanup();
    return HPR_OK;
}

//NET_DVR_GET_COMPRESSCFG_V30
int Demo_CompressParams_V30(LONG lUserID)
{
    int iRet;
    //set params
    DWORD uiReturnLen;
    NET_DVR_COMPRESSIONCFG_V30 struParams = {0};
    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_COMPRESSCFG_V30, 33, \
        &struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_COMPRESSCFG_V30 error.\n");
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }
    printf("pyd---size is %d\n", sizeof(NET_DVR_COMPRESSIONCFG_V30));

    //stop a minute.
    char cUserChoose;
    printf("pyd---press any key to set :");
    cin>>cUserChoose;

    //Set params.
    struParams.struNormHighRecordPara.dwVideoBitrate = 2;
    iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_COMPRESSCFG_V30, 33, \
        &struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30));
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_SET_COMPRESSCFG_V30 error.\n");
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }

    printf("pyd---press any key to get :");
    cin>>cUserChoose;
    //Get params.
    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_COMPRESSCFG_V30, 33, \
        &struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_COMPRESSCFG_V30 error.\n");
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }
    printf("pyd---press any key to stop:");
    cin>>cUserChoose;

    printf("pyd---%d\n", struParams.struNormHighRecordPara.dwVideoBitrate);
    NET_DVR_Logout_V30(lUserID);
    NET_DVR_Cleanup();
    return HPR_OK;
}

//NET_DVR_GET_DVR_SYNCHRONOUS_IPC
int Demo_Synchronous_IPC(LONG lUserID)
{
    int iRet = -1;
    DWORD uiReturnLen = 0;
    LONG lChannel = 0;

    NET_DVR_SYNCHRONOUS_IPC struParams = {0};
    struParams.dwSize = sizeof(struParams);

    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_DVR_SYNCHRONOUS_IPC, lChannel, \
        &struParams, sizeof(NET_DVR_SYNCHRONOUS_IPC), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_DVR_SYNCHRONOUS_IPC error.%d\n",  NET_DVR_GetLastError());
        return HPR_ERROR;
    }
    //printf("pyd---size is %d\n", sizeof(NET_DVR_COMPRESSIONCFG_V30));



    struParams.byEnable = !struParams.byEnable;
    //Set params.
    iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_DVR_SYNCHRONOUS_IPC, lChannel, \
        &struParams, sizeof(NET_DVR_SYNCHRONOUS_IPC));
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_SET_COMPRESSCFG_V30 error.%d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    //Get params.
    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_DVR_SYNCHRONOUS_IPC, lChannel, \
        &struParams, sizeof(NET_DVR_SYNCHRONOUS_IPC), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_DVR_SYNCHRONOUS_IPC error.%d\n",  NET_DVR_GetLastError());
        return HPR_ERROR;
    } 

    printf("pyd---byEnable:%d\n", struParams.byEnable);

    return HPR_OK;
}

/**  @fn  int Demo_IPCPasswd(LONG lUserID)
 *   @brief NET_DVR_SET_DVR_IPC_PASSWD
 *   @param (IN) LONG lUserID  
 *   @return 0--success£¬-1--fail.   
 */
int Demo_IPCPasswd(LONG lUserID)
{
    int iRet = -1;
    LONG lChannel = 33;

    NET_DVR_IPC_PASSWD struParams = {0};
    struParams.dwSize = sizeof(NET_DVR_IPC_PASSWD);
    strcpy(struParams.sOldPasswd, "12345");
    strcpy(struParams.sNewPasswd, "12345");

    iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_DVR_IPC_PASSWD, lChannel, &struParams, sizeof(NET_DVR_IPC_PASSWD));
    if (!iRet)
    {
        printf("pyd---Set NET_DVR_SET_DVR_IPC_PASSWD error:%d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    return HPR_OK;
}

int Demo_GetDeviceNetUsingInfo(LONG lUserID)
{
    int iRet = -1;
    DWORD uiReturnLen = 0;
    LONG lChannel = 0;

    NET_DVR_DEVICE_NET_USING_INFO struParams = {0};
    struParams.dwSize = sizeof(NET_DVR_DEVICE_NET_USING_INFO);

    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_DEVICE_NET_USING_INFO, lChannel, \
        &struParams, sizeof(NET_DVR_DEVICE_NET_USING_INFO), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---Get NET_DVR_DEVICE_NET_USING_INFO error:%d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    printf("pyd---dwPreview:%d\n", struParams.dwPreview);
    printf("pyd---dwPlayback:%d\n", struParams.dwPlayback);
    printf("pyd---dwIPCModule:%d\n", struParams.dwIPCModule);
    printf("pyd---dwNetDiskRW:%d\n", struParams.dwNetDiskRW);

    return HPR_OK;
}

// NET_DVR_SET_DVR_IPC_NET
int Demo_SetIPCNet(LONG lUserID)
{
    int iRet = -1;
    LONG lChannel = 33;

    NET_DVR_IPC_NETCFG struParams = {0};
    struParams.dwSize = sizeof(NET_DVR_IPC_NETCFG);
    strcpy(struParams.struIP.sIpV4, "172.9.12.217");
    struParams.wPort = 8000;

    iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_DVR_IPC_NET, lChannel, &struParams, sizeof(NET_DVR_IPC_NETCFG));
    if (!iRet)
    {
        printf("pyd---Set NET_DVR_SET_DVR_IPC_NET error:%d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    return HPR_OK;
}

// NET_DVR_GET_DISK_RAID_INFO  && NET_DVR_SET_DISK_RAID_ INFO
/**  @fn  int Demo_DiskRaidInfo(LONG lUserID)
 *   @brief 
 *   @param (IN) LONG lUserID  
 *   @return 0--success£¬-1--fail.   
 */
int Demo_DiskRaidInfo(LONG lUserID)
{
    int iRet = -1;
    DWORD uiReturnLen = 0;
    LONG lChannel = 0;

    NET_DVR_DISK_RAID_INFO struParams = {0};
    struParams.dwSize = sizeof(struParams);

    //get
    iRet = NET_DVR_GetDVRConfig(lUserID,  NET_DVR_GET_DISK_RAID_INFO, lChannel, \
        &struParams, sizeof(NET_DVR_DISK_RAID_INFO), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_DISK_RAID_INFO error.%d\n",  NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    struParams.byEnable = !struParams.byEnable;
    //Set
    iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_DISK_RAID_INFO, lChannel, \
        &struParams, sizeof(NET_DVR_DISK_RAID_INFO));
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_SET_DISK_RAID_INFO error.%d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    //Get
    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_DISK_RAID_INFO, lChannel, \
        &struParams, sizeof(NET_DVR_DISK_RAID_INFO), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_DISK_RAID_INFO error.%d\n",  NET_DVR_GetLastError());
        return HPR_ERROR;
    } 

    printf("pyd---byEnable:%d\n", struParams.byEnable);

    return HPR_OK;
}

//IPC params v40
int Demo_IPParaCfg_V40(LONG lUserID)
{
    int iRet = -1;
    DWORD uiReturnLen = 0;
    LONG lChannel = 0;

    NET_DVR_IPPARACFG_V40 struParams = {0};
    struParams.dwSize = sizeof(struParams);

    //get
    iRet = NET_DVR_GetDVRConfig(lUserID,  NET_DVR_GET_IPPARACFG_V40, lChannel, \
        &struParams, sizeof(NET_DVR_IPPARACFG_V40), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_IPPARACFG_V40 error.%d\n",  NET_DVR_GetLastError());
        return HPR_ERROR;
    }
    //Set
    iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_IPPARACFG_V40, lChannel, \
        &struParams, sizeof(NET_DVR_IPPARACFG_V40));
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_SET_IPPARACFG_V40 error.%d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    //Get
    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_IPPARACFG_V40, lChannel, \
        &struParams, sizeof(NET_DVR_IPPARACFG_V40), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_IPPARACFG_V40 error.%d\n",  NET_DVR_GetLastError());
        return HPR_ERROR;
    } 
    printf("pyd---The Info of IP config:\n");
    PrintIPParaCfgInfo(&struParams);

    return HPR_OK;
}

int Demo_IPParaCfg_v31(LONG lUserID)
{
    int iRet = -1;
    DWORD uiReturnLen = 0;
    LONG lChannel = 0;

    NET_DVR_IPPARACFG_V31 struParams = {0};
    struParams.dwSize = sizeof(struParams);

    //get
    iRet = NET_DVR_GetDVRConfig(lUserID,  NET_DVR_GET_IPPARACFG_V31, lChannel, \
        &struParams, sizeof(NET_DVR_IPPARACFG_V31), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_IPPARACFG_V31 error.%d\n",  NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    return HPR_OK;
}

void PrintIPParaCfgInfo(NET_DVR_IPPARACFG_V40 *pIPCParamsInfo)
{
    printf("dwGroupNum:%d\n", pIPCParamsInfo->dwGroupNum);
    printf("dwAChanNum:%d\n", pIPCParamsInfo->dwAChanNum);
    printf("dwDChanNum:%d\n", pIPCParamsInfo->dwDChanNum);
    printf("dwStartDChan:%d\n", pIPCParamsInfo->dwStartDChan);

    //for (int i = 0; i <MAX_CHANNUM_V30; i++)
    int i =0;
    for (i = 0; i <1; i++)
    {
        printf("byAnalogChanEnable[%d]:%d", i, pIPCParamsInfo->byAnalogChanEnable[i]);
    }

    i = 0;
    if (pIPCParamsInfo->struStreamMode[i].byGetStreamType == 3)
    {
        printf("[%d]Enable:%d", i, pIPCParamsInfo->struStreamMode[i].uGetStream.struDDNSStream.byEnable);
        printf("[%d]IP:    %s", i, pIPCParamsInfo->struStreamMode[i].uGetStream.struDDNSStream.struStreamServer.sIpV4);
        printf("[%d]Port:  %d", i, pIPCParamsInfo->struStreamMode[i].uGetStream.struDDNSStream.wStreamServerPort);
    }

}

//NET_DVR_GET_PICCFG
int Demo_PictureParams(LONG lUserID)
{
    int iChannelNO = 1;
    int iRet;

    //1.Get picture params.
    DWORD uiReturnLen;
    NET_DVR_PICCFG_V30 struParams = {0};
    struParams.dwSize = sizeof(NET_DVR_PICCFG_V30);
    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_PICCFG_V30, iChannelNO, &struParams, sizeof(NET_DVR_PICCFG_V30), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GET_PICCFG _V30 %d error.\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }
    printf("pyd---Channel %d Name is %s.\n", iChannelNO, struParams.sChanName);

    //2.modify
	struParams.dwSize = sizeof(NET_DVR_PICCFG_V30);
    strcpy((char *)(struParams.sChanName), "²»ÊÇÂÒÂë");

    //3.set
    iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_PICCFG_V30, iChannelNO, &struParams, sizeof(NET_DVR_PICCFG_V30));
    if (!iRet)
    {
        printf("pyd---NET_DVR_SetDVRConfig NET_DVR_SET_PICCFG error, %d.\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    printf("pyd---Channel %d Name is %s.\n", iChannelNO, struParams.sChanName);
    return HPR_OK;
}


//channel record status
int Demo_ChannelRecordStatus(LONG lUserID)
{
    int iGroupNO = 0;
    int iRet = -1;

    //1.channel record status.
    DWORD uiReturnLen = 0;
    NET_DVR_CHAN_GROUP_RECORD_STATUS struParams = {0};
    struParams.dwSize = sizeof(NET_DVR_CHAN_GROUP_RECORD_STATUS);

    iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_RECORD_CHANNEL_INFO, iGroupNO, &struParams, sizeof(NET_DVR_CHAN_GROUP_RECORD_STATUS), &uiReturnLen);
    if (!iRet)
    {
        printf("pyd---NET_DVR_GET_RECORD_CHANNEL_INFO %d error.\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    //2.set
    iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_RECORD_CHANNEL_INFO, iGroupNO, &struParams, sizeof(NET_DVR_CHAN_GROUP_RECORD_STATUS));
    if (!iRet)
    {
        printf("pyd---NET_DVR_SET_RECORD_CHANNEL_INFO error, %d.\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }

    return HPR_OK;
}


//Get device state
int Demo_GetDeviceState(LONG lUserID)
{
    NET_DVR_WORKSTATE_V30    struWorkState;
    if (NET_DVR_GetDVRWorkState_V30(lUserID, &struWorkState))
    {
        printf("pyd---NET_DVR_GetDVRWorkState_V30 success.\n");
    } 
    else
    {
        printf("pyd---NET_DVR_GetDVRWorkState_V30 error, %d.\n", NET_DVR_GetLastError());
    }

    return HPR_OK;
}



