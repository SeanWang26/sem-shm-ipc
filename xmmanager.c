#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "xmmanager.h"
#include "devicetype.h"

static int g_initaled = 0;

static int xm_device_init(struct xmdevice *dev);

static int xm_init(struct device *dev);
static int xm_uninit(struct device *dev);
static int xm_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
static int xm_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
static int xm_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
static int xm_close_video_stream(struct device *dev, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
static int xm_open_audio_stream(struct device *dev, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
static int xm_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
static int xm_get_config(struct device *dev, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
static int xm_set_config(struct device *dev);
static int xm_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
static int xm_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Req *rsp);
static int xm_ptz_control(struct device *, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);


static struct device_ops xm_ops[] = 
{
	sizeof(struct xmdevice),
	DEVICE_XM,
    xm_init,
    xm_uninit,
    xm_login,
    xm_logout,
	xm_open_video_stream,
	xm_close_video_stream,
	xm_open_audio_stream,
	xm_close_audio_stream,
	xm_get_config,
	xm_set_config,
	xm_open_alarm_stream,
	xm_close_alarm_stream,
	xm_ptz_control
};

void disconnect_callback(long lLoginID, char *pchDVRIP, long nDVRPort, unsigned long dwUser)
{
	FIND_DEVICE_BEGIN(struct xmdevice,DEVICE_XM)
	{
		if(dev->loginid == lLoginID)
		{
			xm_device_init(dev);
			printf("[%s]%p, %s, %lu\n", __FUNCTION__, (void*)dwUser, pchDVRIP, nDVRPort);
			break;
		}
	}
	FIND_DEVICE_END
}

static int real_data_callback_v2(long lRealHandle, const PACKET_INFO_EX *pFrame, unsigned int dwUser)
{
	//lock
	struct xmstream* stream = (struct xmstream*)dwUser;
	//stream->stm.callback(stream->stm.userdata);
		

	/*int		nPacketType;				// 包类型,见MEDIA_PACK_TYPE
	char*	pPacketBuffer;				// 缓存区地址
	unsigned int	dwPacketSize;				// 包的大小

	// 绝对时标
	int		nYear;						// 时标:年		
	int		nMonth;						// 时标:月
	int		nDay;						// 时标:日
	int		nHour;						// 时标:时
	int		nMinute;					// 时标:分
	int		nSecond;					// 时标:秒
	unsigned int 	dwTimeStamp;					// 相对时标低位，单位为毫秒
	unsigned int	dwTimeStampHigh;        //相对时标高位，单位为毫秒
	unsigned int   dwFrameNum;             //帧序号
	unsigned int   dwFrameRate;            //帧率
	unsigned short uWidth;              //图像宽度
	unsigned short uHeight;             //图像高度
	unsigned int       Reserved[6];            //保留
*/


	printf("xm get frame user %p, type %d, size %u\n", stream, pFrame->nPacketType, pFrame->dwPacketSize);
	
	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
	return 1;
}
void talk_data_callback(LONG lTalkHandle, char *pDataBuf, long dwBufSize, char byAudioFlag, long dwUser)
{
	//lock
	FIND_DEVICE_BEGIN(struct xmdevice,DEVICE_XM)
	{
		if(dev->voicehandle == lTalkHandle)
		{
			//((xmdevice*)device)->voice_callback(((xmdevice*)device)->voice_callbackuserdata);
			printf("[%s]device %p, lTalkHandle %ld\n", __FUNCTION__, (void*)dev, lTalkHandle);
			break;
		}

	}
	FIND_DEVICE_END

	//BOOL bResult = TRUE;
}

static inline int handle_alarm(xmdevice *device, char *pBuf, unsigned long dwBufLen)
{
	SDK_AlarmInfo alarmInfo;
	memcpy (&alarmInfo, pBuf, dwBufLen); 	

	if ( SDK_EVENT_CODE_NET_ALARM == alarmInfo.iEvent 
		|| SDK_EVENT_CODE_MANUAL_ALARM == alarmInfo.iEvent 
		|| SDK_EVENT_CODE_LOCAL_ALARM == alarmInfo.iEvent )
	{
		
	}
	else if ( SDK_EVENT_CODE_VIDEO_MOTION == alarmInfo.iEvent ) 
	{
		printf("[%s]SDK_EVENT_CODE_VIDEO_MOTION\n", __FUNCTION__);
	}
	else if ( SDK_EVENT_CODE_VIDEO_LOSS == alarmInfo.iEvent  )
	{
	
	}
	else if ( SDK_EVENT_CODE_VIDEO_BLIND == alarmInfo.iEvent )
	{
	
	}
	else if ( SDK_EVENT_CODE_STORAGE_FAILURE == alarmInfo.iEvent )
	{
		
	}
	else if ( SDK_EVENT_CODE_LOW_SPACE == alarmInfo.iEvent  )
	{
		
	}

	return 0;
}


bool mess_callback(long lLoginID, char *pBuf, unsigned long dwBufLen, long dwUser)
{
	//lock
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		assert(device->obj.type == OBJECT_TYPE_DEVICE);
		if(!device->deleted && device->ops->type == DEVICE_XM )
		{	
			if(((xmdevice*)device)->loginid == lLoginID)
			{				
				handle_alarm((xmdevice*)device, pBuf, dwBufLen);
				break;
			}
		}
	}
	
	return 1;
}

static int GetResolution(int type, int *width, int *height)
{
	printf("[%s]type %d\n", __FUNCTION__, type);

	switch(type)
	{
		case SDK_CAPTURE_SIZE_D1:
			*width = 720;
			*height = 576;
		break;
		case SDK_CAPTURE_SIZE_HD1:
			*width = 352;
			*height = 480;		
		break;
		case SDK_CAPTURE_SIZE_BCIF:
			*width = 720;
			*height = 288;	
		break;
		case SDK_CAPTURE_SIZE_CIF:
			*width = 352;
			*height = 288;	
		break;
		case SDK_CAPTURE_SIZE_QCIF:
			*width = 176;
			*height = 144;			
		break;
		case SDK_CAPTURE_SIZE_VGA:
			*width = 640;
			*height = 480;				
		break;
		case SDK_CAPTURE_SIZE_QVGA:
			*width = 480;
			*height = 480;	
		break;
		case SDK_CAPTURE_SIZE_QQVGA:
			*width = 160;
			*height = 128;	
		break;
		case SDK_CAPTURE_SIZE_ND1:
			*width = 240;
			*height = 192;				
		break;
		case SDK_CAPTURE_SIZE_650TVL:
			*width = 926;
			*height = 576;				
		break;
		case SDK_CAPTURE_SIZE_720P:
			*width = 1280;
			*height = 720;				
		break;
		case SDK_CAPTURE_SIZE_1_3M:		  ///< 1280*960
			*width = 1280;
			*height = 960;			
		break;
		case SDK_CAPTURE_SIZE_UXGA: 	  ///< 1600*1200
			*width = 1600;
			*height = 1200;			
		break;
		case SDK_CAPTURE_SIZE_1080P: 	  ///< 1920*1080
			*width = 1920;
			*height = 1080;			
		break;
		case SDK_CAPTURE_SIZE_WUXGA: 	  ///< 1920*1200
			*width = 1920;
			*height = 1200;			
		break;
		case SDK_CAPTURE_SIZE_2_5M:		  ///< 1872*1408
			*width = 1872;
			*height = 1408;			
		break;
		case SDK_CAPTURE_SIZE_3M:		  ///< 2048*1536
			*width = 2048;
			*height = 1536;			
		break;
		case SDK_CAPTURE_SIZE_5M:		  ///< 3744*1408
			*width = 3744;
			*height = 1408;			
		break;
		default:
			*width = 0;
			*height = 0;
		return -1;
	}

	return 0;
}

static int GetEncodeMode(int type)
{
	switch(type)
	{
		case SDK_CAPTURE_COMP_DIVX_MPEG4:	///< DIVX MPEG4。
			return type;
		break;
		case SDK_CAPTURE_COMP_MS_MPEG4:		///< MS MPEG4。
			return type;
		break;
		case SDK_CAPTURE_COMP_MPEG2: 		///< MPEG2。
			return type;
		break;
		case SDK_CAPTURE_COMP_MPEG1: 		///< MPEG1。
			return type;
		break;
		case SDK_CAPTURE_COMP_H263:			///< H.263
			return type;
		break;
		case SDK_CAPTURE_COMP_MJPG:			///< MJPG
			return type;
		break;
		case SDK_CAPTURE_COMP_FCC_MPEG4:	///< FCC MPEG4
			return type;
		break;
		case SDK_CAPTURE_COMP_H264:			///< H.264
			return type;
		break;
	}

	return 0;
}


struct xmdevice *xm_alloc_device()
{
	xmdevice *device = (xmdevice *)alloc_device(xm_ops);
	if(device)
	{
		device->loginid = XM_INVALIDE_LOGINID;
		memset(&device->info, 0 ,sizeof(device->info));
		return device;
	}

	return NULL;
}

static int xm_init(struct device *dev)
{
	printf("[%s]\n", __FUNCTION__);
	if(!g_initaled && H264_DVR_Init(disconnect_callback, (unsigned long)&devicelist))
	{
		list_init(&devicelist);
		
		H264_DVR_SetDVRMessCallBack(mess_callback, (unsigned long)&devicelist);
		printf("[%s]init success %d\n", __FUNCTION__, g_initaled);
		++g_initaled;
	}

	return SUCCESS;
}

static int xm_uninit(struct device *dev)
{
	if(g_initaled)
	{
		g_initaled=0;
		H264_DVR_Cleanup();
	}
	
	return SUCCESS;
}

static int xm_device_init(struct xmdevice *dev)
{
	dev->loginid = XM_INVALIDE_LOGINID;

	struct channel* channel;
	LIST_FOR_EACH_ENTRY(channel, &dev->dev.channels, struct channel, entry)
	{
		assert(channel->obj.type == OBJECT_TYPE_CHANNEL);

		struct stream* stream;
		LIST_FOR_EACH_ENTRY(stream, &channel->streams, struct stream, entry)
		{
			assert(stream->obj.type == OBJECT_TYPE_STREAM);
			stream->pulling = 0;
			stream->callback = NULL;
			stream->userdata = NULL;
			H264_DVR_SetRealDataCallBack_V2(((struct xmstream*)stream)->playhandle, NULL, 0);
			H264_DVR_StopVoiceCom(dev->voicehandle);///?????? need
			dev->voicehandle = 0;
			memset(&dev->info, 0, sizeof(dev->info));
		}
	}

	return 0;
}

static int xm_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);

	if(get_device(dev)==NULL) add_device(dev);

	struct xmdevice *xmdev = (xmdevice *)dev;
	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}
	
	strcpy(dev->ip, req->Ip);
	dev->port = req->Port;
	strcpy(dev->user, req->User);
	strcpy(dev->password, req->Password);
	
	int err = 0;			
	xmdev->loginid = H264_DVR_Login((char*)dev->ip, dev->port,(char*)dev->user, (char*)dev->password,(LPH264_DVR_DEVICEINFO)(&xmdev->info),&err);		
	if(xmdev->loginid > 0)
	{
		printf("[%s]xmdev login success, %s, %d, %s, %s\n"
			, __FUNCTION__, req->Ip, req->Port, req->User, req->Password);
		return 0;
	}
	else
	{
		printf("[%s]xmdev login failed, %s, %d, %s, %s\n"
			, __FUNCTION__, req->Ip, req->Port, req->User, req->Password);
		return LOGIN_FAILED;
	}

	return LOGIN_FAILED;
}
static int xm_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		printf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;

	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(H264_DVR_Logout(xmdev->loginid))
	{
		xmdev->loginid = 0;
		memset(&xmdev->info,0,sizeof(xmdev->info));
		printf("[%s]xmdev xm_logout success\n", __FUNCTION__);
		return 0;		
	}
	else
	{
		printf("[%s]xmdev login failed\n", __FUNCTION__);
		return -2;
	}

	return -2;
}
static int xm_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		printf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;
	
	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	//lock??
	struct xmchannel* chn = NULL;
	struct xmstream* stm = NULL;
	chn = (struct xmchannel*)get_channel(&dev->channels, req->Channel);
	if(chn)
	{
		stm = (struct xmstream*)get_stream(&chn->chn.streams, req->Codec);
	}
	else
	{
		chn = (struct xmchannel*)alloc_channel(sizeof(struct xmchannel));
		if(chn)
		{
			chn->chn.id = req->Channel;
			if((struct channel*)chn != add_channel(dev, (struct channel*)chn))
			{
				printf("[%s]add_channel old??\n", __FUNCTION__);
				free(chn);
				chn = NULL;
				//chn = old;??
			}
		}
		else
		{
			printf("[%s]alloc_channel failed\n", __FUNCTION__);
			return ALLOC_CHANNEL_FAILED;
		}
	}

	if(stm == NULL)
	{
		//add stream
		stm = (struct xmstream*)alloc_stream(sizeof(struct xmstream));
		if(stm)
		{
			stm->stm.id = req->Codec;
			if((struct stream*)stm != add_stream((channel*)chn, (struct stream*)stm))
			{
				printf("[%s]add_stream old??\n", __FUNCTION__);
				free(stm);
				stm = NULL;
				//stm = old;
			}
		}
		else
		{
			printf("[%s]alloc_stream failed\n", __FUNCTION__);
			return ALLOC_STREAM_FAILED;
		}
	}

	H264_DVR_CLIENTINFO playstru;
	playstru.nChannel = req->Channel;
	playstru.nStream = req->Codec;
	playstru.nMode = 0;
	long handle = H264_DVR_RealPlay(xmdev->loginid, &playstru);	
	if(handle <= 0 )
	{
		printf("[%s]start real stream wrong! m_iPlayhandle %ld, channel %d, nStream %d\n"
			, __FUNCTION__, handle, req->Channel, req->Codec);
		return OPEN_VIDEO_STREAM_FAILED;
	}
	else
	{
		printf("[%s]start real stream ok\n", __FUNCTION__);
		if(H264_DVR_SetRealDataCallBack_V2(handle, real_data_callback_v2, (long)stm)==0)
		{
			printf("[%s]set video callback failed!\n", __FUNCTION__);
			return SET_VIDEO_CALLBACK_FAILED;
		}
		
		stm->playhandle = handle;
		stm->stm.pulling = 1;
		stm->stm.callback = req->Callback;
		stm->stm.userdata = req->UserData;		
		rsp->StreamHandle = (long)stm;
	}

	return SUCCESS;
}
static int xm_close_video_stream(struct device *dev, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		printf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;
	
	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct xmchannel* chn = NULL;
	struct xmstream* stm = NULL;
	chn = (struct xmchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		return INVALID_CHANNEL_NO;
	}
	
	stm = (struct xmstream*)get_stream(&chn->chn.streams, req->Codec);
	if(stm == NULL)
	{
		return INVALID_STREAM_NO;
	}

	if((long)stm != req->StreamHandle)
	{
		//should we using stm to close it???
		assert(false);
	}

	if(H264_DVR_SetRealDataCallBack_V2(stm->playhandle, NULL, 0)==0)
	{
		printf("[%s]set video callback failed!\n", __FUNCTION__);
		//return SET_VIDEO_CALLBACK_FAILED;
	}

	if(H264_DVR_StopRealPlay(stm->playhandle))
	{
		printf("[%s]H264_DVR_StopRealPlay ok!\n", __FUNCTION__);
		stm->stm.pulling = 0;
		stm->playhandle = 0L;
	}
	else
	{
		printf("[%s]H264_DVR_StopRealPlay error\n", __FUNCTION__);
		return CLOSE_VIDEO_STREAM_FAILED;
	}
	
	return SUCCESS;
}
static int xm_open_audio_stream(struct device *dev, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);

	xmdevice *xmdev = (xmdevice *)dev;
	
	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	long voicehandle = H264_DVR_StartVoiceCom_MR(xmdev->loginid, talk_data_callback, (long)dev);
	if(voicehandle > 0)
	{
		xmdev->voicehandle = voicehandle;
	}
	
	printf("[%s]\n", __FUNCTION__);
	return SUCCESS;
}
static int xm_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		printf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;

	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(H264_DVR_StopVoiceCom(xmdev->voicehandle))
	{
		printf("[%s]H264_DVR_StopVoiceCom ok\n", __FUNCTION__);
		xmdev->voicehandle = 0;
	}
	else
	{
		printf("[%s]H264_DVR_StopVoiceCom error\n", __FUNCTION__);
		return CLOSE_AUDIO_STREAM_FAILED;
	}
	
	return SUCCESS;
}
static int xm_get_config(struct device *dev, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);	
	if(get_device(dev)==NULL)
	{
		printf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;
	
	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	switch(req->Type)
	{
		case GET_ENCODE_CONFIG://获取编码配置
		{		
			struct SDK_EncodeConfigAll_SIMPLIIFY EncodeConfig;
			unsigned long dwRetLen = 0;
			int nWaitTime = 10000;

			memset(&EncodeConfig, 0, sizeof(EncodeConfig));

			BOOL bSuccess = H264_DVR_GetDevConfig(xmdev->loginid, E_SDK_CONFIG_SYSENCODE_SIMPLIIFY, -1,
						(char *)&EncodeConfig, sizeof(SDK_EncodeConfigAll_SIMPLIIFY),&dwRetLen,nWaitTime);
			if (bSuccess && dwRetLen == sizeof (SDK_EncodeConfigAll_SIMPLIIFY))
			{
				printf("[%s]H264_DVR_GetDevConfig ok, dwRetLen %lu\n", __FUNCTION__, dwRetLen);
				rsp->Channel = req->Channel;
				rsp->Type = GET_ENCODE_CONFIG;
				rsp->Size = dwRetLen;
				rsp->Config = (char*) malloc(dwRetLen);//not check null ptr??
				memcpy(rsp->Config, &EncodeConfig, sizeof(SDK_EncodeConfigAll_SIMPLIIFY));

				for(int i=0; i<NET_MAX_CHANNUM; ++i)
				{
					if(EncodeConfig.vEncodeConfigAll[i].dstMainFmt.bVideoEnable)
					{
						int width = 0, height = 0;
						GetResolution(EncodeConfig.vEncodeConfigAll[i].dstMainFmt.vfFormat.iResolution
							, &width, &height);
						printf("[%s]dstMainFmt width %d, height %d, venable %d, FPS %d, BitRate %d, GOP %d, encode %d\n"
							, __FUNCTION__, width, height
							, EncodeConfig.vEncodeConfigAll[i].dstMainFmt.bVideoEnable
							, EncodeConfig.vEncodeConfigAll[i].dstMainFmt.vfFormat.nFPS
							, EncodeConfig.vEncodeConfigAll[i].dstMainFmt.vfFormat.nBitRate
							, EncodeConfig.vEncodeConfigAll[i].dstMainFmt.vfFormat.iGOP
							, GetEncodeMode(EncodeConfig.vEncodeConfigAll[i].dstMainFmt.vfFormat.iCompression));

					}

					if(EncodeConfig.vEncodeConfigAll[i].dstExtraFmt.bVideoEnable)
					{
						int width = 0, height = 0;
						GetResolution(EncodeConfig.vEncodeConfigAll[i].dstExtraFmt.vfFormat.iResolution
							, &width, &height);
						printf("[%s]dstExtraFmt width %d, height %d, venable %d, FPS %d, BitRate %d, GOP %d, encode %d\n"
							, __FUNCTION__, width, height
							, EncodeConfig.vEncodeConfigAll[i].dstExtraFmt.bVideoEnable
							, EncodeConfig.vEncodeConfigAll[i].dstExtraFmt.vfFormat.nFPS
							, EncodeConfig.vEncodeConfigAll[i].dstExtraFmt.vfFormat.nBitRate
							, EncodeConfig.vEncodeConfigAll[i].dstExtraFmt.vfFormat.iGOP
							, GetEncodeMode(EncodeConfig.vEncodeConfigAll[i].dstExtraFmt.vfFormat.iCompression));
					
					}

				}
				
			}
			else
			{	
				printf("[%s]GetConfig Wrong:%d,RetLen:%ld  !=  %d\n"
							, __FUNCTION__,bSuccess,dwRetLen,sizeof (SDK_EncodeConfigAll_SIMPLIIFY));
				return GET_CONFIG_FAILED;
			}			
			break;
		}
	}

	return SUCCESS;
}
static int xm_set_config(struct device *dev)
{
	printf("[%s]\n", __FUNCTION__);

	return SUCCESS;
}
static int xm_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);	
	if(get_device(dev)==NULL)
	{
		printf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;

	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(H264_DVR_SetupAlarmChan(xmdev->loginid))
	{
		printf("[%s]xmdev H264_DVR_SetupAlarmChan ok\n", __FUNCTION__);
	}
	else
	{
		printf("[%s]xmdev H264_DVR_SetupAlarmChan failed\n", __FUNCTION__);
		return OPEN_ALARM_STREAM_FAILED;
	}

	return SUCCESS;
}
static int  xm_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Req *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	if(get_device(dev)==NULL)
	{
		printf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}
	
	xmdevice *xmdev = (xmdevice *)dev;

	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}
	
	if(H264_DVR_CloseAlarmChan(xmdev->loginid))
	{
		printf("[%s]xmdev H264_DVR_SetupAlarmChan ok\n", __FUNCTION__);
	}
	else
	{
		printf("[%s]xmdev H264_DVR_SetupAlarmChan failed\n", __FUNCTION__);
		return CLOSE_ALARM_STREAM_FAILED;
	}
	
	return SUCCESS;
}
static int xm_ptz_control(struct device * dev, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	if(get_device(dev)==NULL)
	{
		printf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;

	switch(req->Action)
	{
		case PTZ_STOP:
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, 0, 1, 0);
		break;
		case PTZ_MOVE:
			if(PTZ_UP) 
				H264_DVR_PTZControl(xmdev->loginid, req->Channel, TILT_UP, 0, req->Speed);
			else if(PTZ_DOWN)
				H264_DVR_PTZControl(xmdev->loginid, req->Channel, TILT_DOWN, 0, req->Speed);
			else if(PTZ_LEFT) 
				H264_DVR_PTZControl(xmdev->loginid, req->Channel, PAN_LEFT, 0, req->Speed);
			else if(PTZ_RIGHT) 
				H264_DVR_PTZControl(xmdev->loginid, req->Channel, PAN_RIGHT, 0, req->Speed);
			else if(PTZ_LEFT_UP) 
				H264_DVR_PTZControl(xmdev->loginid, req->Channel, PAN_LEFTTOP, 0, req->Speed);
			else if(PTZ_DOWN_LEFT) 
				H264_DVR_PTZControl(xmdev->loginid, req->Channel, PAN_LEFTDOWN, 0, req->Speed);
			else if(PTZ_UP_RIGHT) 
				H264_DVR_PTZControl(xmdev->loginid, req->Channel, PAN_RIGTHTOP, 0, req->Speed);
			else if(PTZ_DOWN_RIGHT) 
				H264_DVR_PTZControl(xmdev->loginid, req->Channel, PAN_RIGTHDOWN, 0, req->Speed);
			else
			{
				printf("[%s]dev %p, PTZ_MOVE, unkonwn direct\n", __FUNCTION__, dev);
				return UNKONWN_PTZ_COMMAND;
			}
		break;
		case PTZ_PUSH_FAR:
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, ZOOM_OUT, 0, req->Speed);
		break;
		case PTZ_PULL_NEAR:
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, ZOOM_IN, 0, req->Speed);
		break;
		case PTZ_IRIS_ADD:
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, IRIS_OPEN, 0, req->Speed);
		break;
		case PTZ_IRIS_SUB:
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, IRIS_CLOSE, 0, req->Speed);
		break;		
		case PTZ_FOCUS_FAR:
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, FOCUS_FAR, 0, req->Speed);
		break;
		case PTZ_FOCUS_NEAR:
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, FOCUS_NEAR, 0, req->Speed);
		break;
		case SET_PRESET:
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_POINT_SET_CONTROL, req->PresetNum, 2, 3, 0);
		case CLEAR_PRESET:
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_POINT_DEL_CONTROL, req->PresetNum, 2, 3, 0);
		default:
			printf("[%s]dev %p, PTZ, unkonwn cmd %d\n", __FUNCTION__, dev, req->Action);
			return UNKONWN_PTZ_COMMAND;
	}

	return 0;
}












