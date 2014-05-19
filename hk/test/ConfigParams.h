/*
* Copyright(C) 2010,Hikvision Digital Technology Co., Ltd 
* 
* File   name£ºConfigParams.h
* Discription£º
* Version    £º1.0
* Author     £ºpanyadong
* Create Date£º2010_3_29
* Modification History£º
*/
#include "public.h"

int Demo_ConfigParams(LONG lUserID);

//NET_DVR_GET_COMPRESSCFG
int Demo_CompressParams();
//NET_DVR_GET_COMPRESSCFG_V30
int Demo_CompressParams_V30(LONG lUserID);

//***NVR(9600)***//
//NET_DVR_GET_DVR_SYNCHRONOUS_IPC && NET_DVR_SET_DVR_SYNCHRONOUS_IPC
int Demo_Synchronous_IPC(LONG lUserID);
//NET_DVR_SET_DVR_IPC_PASSWD
int Demo_IPCPasswd(LONG lUserID);
// NET_DVR_GET_DEVICE_NET_USING_INFO
int Demo_GetDeviceNetUsingInfo(LONG lUserID);
// NET_DVR_SET_DVR_IPC_NET
int Demo_SetIPCNet(LONG lUserID);

//***64 NVR(9664)****//
// NET_DVR_GET_DISK_RAID_INFO  && NET_DVR_SET_DISK_RAID_ INFO
int Demo_DiskRaidInfo(LONG lUserID);

//IPC params v40
int Demo_IPParaCfg_V40(LONG lUserID);
int Demo_IPParaCfg_v31(LONG lUserID);

void PrintIPParaCfgInfo(NET_DVR_IPPARACFG_V40 *pIPCParamsInfo);

//NET_DVR_GET_PICCFG_V30
int Demo_PictureParams(LONG lUserID);

//channel record status
int Demo_ChannelRecordStatus(LONG lUserID);

//Get device state
int Demo_GetDeviceState(LONG lUserID);



