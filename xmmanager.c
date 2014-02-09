#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

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
static int xm_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
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

void xm_disconnect_callback(long lLoginID, char *pchDVRIP, long nDVRPort, unsigned long dwUser)
{
	FIND_DEVICE_BEGIN(struct xmdevice,DEVICE_XM)
	{
		if(dev->loginid == lLoginID)
		{
			xm_device_init(dev);
			jtprintf("[%s]%p, %s, %lu\n", __FUNCTION__, (void*)dwUser, pchDVRIP, nDVRPort);
			break;
		}
	}
	FIND_DEVICE_END
}
static int xm_pack_type_convert(enum MEDIA_PACK_TYPE type)
{
	if(VIDEO_P_FRAME==type)
		return P_FRAME;
	else if(VIDEO_I_FRAME==type) 
		return I_FRAME;
	else if(VIDEO_B_FRAME==type || VIDEO_BP_FRAME==type ||VIDEO_BBP_FRAME==type)
		return B_FRAME;
	
	/*
	enum MEDIA_PACK_TYPE
	{
		FILE_HEAD = 0,				// 文件头
		VIDEO_I_FRAME = 1,			// 视频I帧
		VIDEO_B_FRAME = 2,			// 视频B帧
		VIDEO_P_FRAME = 3,			// 视频P帧
		VIDEO_BP_FRAME = 4, 		// 视频BP帧
		VIDEO_BBP_FRAME = 5,		// 视频B帧B帧P帧
		VIDEO_J_FRAME = 6,			// 图片帧
		AUDIO_PACKET = 10,			// 音频包
	};
	*/
	return UNKNOWN_FRAME;
}

static int xm_real_data_callback_v2(long lRealHandle, const PACKET_INFO_EX *pFrame, unsigned int dwUser)
{
	//lock
	struct xmstream* stream = (struct xmstream*)dwUser;
	//jtprintf("xm get frame user %p, type %d, size %u\n", stream, pFrame->nPacketType, pFrame->dwPacketSize);

	/*typedef struct
	{
		int 	nPacketType;				// 包类型,见MEDIA_PACK_TYPE
		char*	pPacketBuffer;				// 缓存区地址
		unsigned int	dwPacketSize;				// 包的大小
	
		// 绝对时标
		int 	nYear;						// 时标:年		
		int 	nMonth; 					// 时标:月
		int 	nDay;						// 时标:日
		int 	nHour;						// 时标:时
		int 	nMinute;					// 时标:分
		int 	nSecond;					// 时标:秒
		unsigned int	dwTimeStamp;					// 相对时标低位，单位为毫秒
		unsigned int	dwTimeStampHigh;		//相对时标高位，单位为毫秒
		unsigned int   dwFrameNum;			   //帧序号
		unsigned int   dwFrameRate; 		   //帧率
		unsigned short uWidth;				//图像宽度
		unsigned short uHeight; 			//图像高度
		unsigned int	   Reserved[6]; 		   //保留
	} PACKET_INFO_EX;*/

	if(pFrame->nPacketType == AUDIO_PACKET || pFrame->nPacketType == FILE_HEAD)
		return 1;

	st_stream_data stmdata;
	stmdata.streamtype = VIDEO_STREAM_DATA;
	stmdata.pdata= pFrame->pPacketBuffer+8;
	stmdata.datalen = pFrame->dwPacketSize-8;
	stmdata.stream_info.video_stream_info.encode = stream->currentencode;
	stmdata.stream_info.video_stream_info.frametype = xm_pack_type_convert((enum MEDIA_PACK_TYPE)pFrame->nPacketType);
	stmdata.stream_info.video_stream_info.width = pFrame->dwPacketSize;
	stmdata.stream_info.video_stream_info.height = pFrame->dwPacketSize;
	stmdata.stream_info.video_stream_info.fps = 0;
	stmdata.stream_info.video_stream_info.bitrate = 0;
	stmdata.year = pFrame->nYear;
	stmdata.month = pFrame->nMonth;
	stmdata.day = pFrame->nDay;
	stmdata.hour = pFrame->nHour;
	stmdata.minute = pFrame->nMinute;
	stmdata.second = pFrame->nSecond;

	stream->stm.callback(&stmdata, stream->stm.userdata);

	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
	return 1;
}
void xm_talk_data_callback(LONG lTalkHandle, char *pDataBuf, long dwBufSize, char byAudioFlag, long dwUser)
{
	//lock
	FIND_DEVICE_BEGIN(struct xmdevice,DEVICE_XM)
	{
		if(dev->voicehandle == lTalkHandle)
		{
			struct channel* chn; 												
			LIST_FOR_EACH_ENTRY(chn, &dev->dev.channels, struct channel, entry) 
			{
				if(chn->audiocallback && chn->audiouserdata)
				{
					st_stream_data stmdata;
					stmdata.streamtype = AUDIO_STREAM_DATA;
					stmdata.pdata= pDataBuf;
					stmdata.datalen = dwBufSize;

					jtprintf("[%s]device %p, lTalkHandle %ld, chn %d\n"
						, __FUNCTION__, (void*)dev, lTalkHandle, chn->id);
					chn->audiocallback(&stmdata, chn->audiouserdata);
				}
			}
		
		
			break;
		}

	}
	FIND_DEVICE_END

	//BOOL bResult = TRUE;
}

static inline int xm_handle_alarm(xmdevice *device, char *pBuf, unsigned long dwBufLen)
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
		jtprintf("[%s]SDK_EVENT_CODE_VIDEO_MOTION\n", __FUNCTION__);
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

	//device->dev.alarmcallback(device->dev.alarmuserdata);
	return 0;
}

static bool xm_mess_callback(long lLoginID, char *pBuf, unsigned long dwBufLen, long dwUser)
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
				xm_handle_alarm((xmdevice*)device, pBuf, dwBufLen);
				break;
			}
		}
	}
	
	return 1;
}

static int xm_resolution_convert(int resolution, int *width, int *height)
{
	//jtprintf("[%s]type %d\n", __FUNCTION__, resolution);

	switch(resolution)
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

static int xm_get_encode_mode(int type)
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
			return VIDEO_ENCODE_JPEG;
		break;
		case SDK_CAPTURE_COMP_FCC_MPEG4:	///< FCC MPEG4
			return VIDEO_ENCODE_VIDEO_MPEG4;
		break;
		case SDK_CAPTURE_COMP_H264:			///< H.264
			return VIDEO_ENCODE_VIDEO_H264;
		break;
	}

	return 0;
}
static int xm_fill_encode_info(struct device* dev, struct SDK_EncodeConfigAll_SIMPLIIFY *EncodeConfig)
{
	dev->encodeinfo.last_get_time = time(0);

	//memset????
	
	for(int i=0; i<MAX_CHANNEL_ENCODE_INFO &&  i<NET_MAX_CHANNUM; ++i)
	{
		//主
		dev->encodeinfo.encode_info[i].mainencode.enable = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.bVideoEnable;
		dev->encodeinfo.encode_info[i].mainencode.encodetype 
			= xm_get_encode_mode(EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iCompression);
		dev->encodeinfo.encode_info[i].mainencode.fps = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.nFPS;

		xm_resolution_convert(EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iResolution
							, &dev->encodeinfo.encode_info[i].mainencode.width
							, &dev->encodeinfo.encode_info[i].mainencode.height);

		dev->encodeinfo.encode_info[i].mainencode.quality = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iQuality;
		dev->encodeinfo.encode_info[i].mainencode.bitrate = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.nBitRate;
		dev->encodeinfo.encode_info[i].mainencode.bitratectl = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iBitRateControl;
		dev->encodeinfo.encode_info[i].mainencode.gop = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iGOP;

		//辅1
		dev->encodeinfo.encode_info[i].sub1encode.enable = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.bVideoEnable;
		dev->encodeinfo.encode_info[i].sub1encode.encodetype 
			= xm_get_encode_mode(EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iCompression);
		dev->encodeinfo.encode_info[i].sub1encode.fps = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.nFPS;
		
		xm_resolution_convert(EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iResolution
							, &dev->encodeinfo.encode_info[i].sub1encode.width
							, &dev->encodeinfo.encode_info[i].sub1encode.height);
		
		dev->encodeinfo.encode_info[i].sub1encode.quality = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iQuality;
		dev->encodeinfo.encode_info[i].sub1encode.bitrate = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.nBitRate;
		dev->encodeinfo.encode_info[i].sub1encode.bitratectl = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iBitRateControl;
		dev->encodeinfo.encode_info[i].sub1encode.gop = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iGOP;
	}

	return 0;
};

struct xmdevice *xm_alloc_device()
{
	xmdevice *device = (xmdevice *)_alloc_device(xm_ops);
	if(device)
	{
		xm_device_init(device);
		return device;
	}

	return NULL;
}

static int xm_init(struct device *dev)
{
	jtprintf("[%s]\n", __FUNCTION__);
	if(!g_initaled && H264_DVR_Init(xm_disconnect_callback, (unsigned long)&devicelist))
	{
		H264_DVR_SetDVRMessCallBack(xm_mess_callback, (unsigned long)&devicelist);
		jtprintf("[%s]init success %d\n", __FUNCTION__, g_initaled);
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
	struct channel* channel;
	LIST_FOR_EACH_ENTRY(channel, &dev->dev.channels, struct channel, entry)
	{
		struct xmchannel* xmchannel = (struct xmchannel*)channel;
		assert(xmchannel->chn.obj.type == OBJECT_TYPE_CHANNEL);
		struct stream* stream;
		LIST_FOR_EACH_ENTRY(stream, &xmchannel->chn.streams, struct stream, entry)
		{
			struct xmstream* xmstream = (struct xmstream*)stream;
			assert(xmstream->stm.obj.type == OBJECT_TYPE_STREAM);
			xmstream->currentencode = VIDEO_ENCODE_UNKNOW;
			xmstream->playhandle = XM_INVALIDE_PLAYHANDLE;
			xmstream->stm.pulling = 0;
			xmstream->stm.callback = NULL;
			if(xmstream->stm.userdata)
			{
				free(xmstream->stm.userdata);
				xmstream->stm.userdata = NULL;
			}

			memset(&dev->info, 0, sizeof(dev->info));
			memset(&dev->dev.encodeinfo, 0, sizeof(dev->dev.encodeinfo));
		}

		xmchannel->chn.audiocallback = NULL;
		xmchannel->chn.audiouserdata = NULL;//free
	}

	dev->voicehandle = 0;
	dev->dev.alarmcallback = NULL;
	dev->dev.alarmuserdata = NULL;	//free
	dev->loginid = XM_INVALIDE_LOGINID;
	
	memset(dev->dev.ip, 0, sizeof(dev->dev.ip));
	dev->dev.port = 9999;
	memset(dev->dev.user, 0, sizeof(dev->dev.user));
	memset(dev->dev.password, 0, sizeof(dev->dev.password));

	return 0;
}

static int xm_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	xm_init(dev);//not good

	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);

	if(get_device(dev)==NULL) add_device(dev);

	struct xmdevice *xmdev = (xmdevice *)dev;
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}
	
	strcpy(dev->ip, req->Ip);
	dev->port = req->Port;
	strcpy(dev->user, req->User);
	strcpy(dev->password, req->Password);
	
	int err = 0;
	
	//xmdev->loginid = H264_DVR_Login("192.168.1.10", 34567,"admin", "",(LPH264_DVR_DEVICEINFO)(&xmdev->info),&err);		
	xmdev->loginid = H264_DVR_Login((char*)dev->ip, dev->port,(char*)dev->user, (char*)dev->password,(LPH264_DVR_DEVICEINFO)(&xmdev->info),&err);		
	if(xmdev->loginid > 0)
	{
		jtprintf("[%s]xmdev login success, %s, %d, %s, %s, %ld\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, xmdev->loginid);
		return SUCCESS;
	}
	else
	{
		jtprintf("[%s]xmdev login failed, %s, %d, %s, %s, error %d, loginid %d\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, err, xmdev->loginid);
		return LOGIN_FAILED;
	}

	return LOGIN_FAILED;
}
static int xm_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;

	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(H264_DVR_Logout(xmdev->loginid))
	{
		xm_device_init(xmdev);
		jtprintf("[%s]xmdev xm_logout success\n", __FUNCTION__);
		return SUCCESS;		
	}
	else
	{
		jtprintf("[%s]xmdev login failed\n", __FUNCTION__);
		return -2;
	}

	return -2;
}
static int xm_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	//lock??
	struct xmchannel* chn = NULL;
	struct xmstream* stm = NULL;
	chn = (struct xmchannel*)get_channel(&dev->channels, req->Channel);
	if(chn)
	{
		stm = (struct xmstream*)get_stream_by_id(&chn->chn.streams, req->Codec);
	}
	else
	{
		chn = (struct xmchannel*)alloc_channel(sizeof(struct xmchannel));
		if(chn)
		{
			chn->chn.id = req->Channel;
			if((struct channel*)chn != add_channel(dev, (struct channel*)chn))
			{
				jtprintf("[%s]add_channel old??\n", __FUNCTION__);
				free(chn);
				chn = NULL;
				//chn = old;??
			}
		}
		else
		{
			jtprintf("[%s]alloc_channel failed\n", __FUNCTION__);
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
				jtprintf("[%s]add_stream old??\n", __FUNCTION__);
				free(stm);
				stm = NULL;
				//stm = old;
			}
		}
		else
		{
			jtprintf("[%s]alloc_stream failed\n", __FUNCTION__);
			return ALLOC_STREAM_FAILED;
		}
	}

	H264_DVR_CLIENTINFO playstru;
	playstru.nChannel = req->Channel;
	playstru.nStream = req->Codec;
	playstru.nMode = 0;

	jtprintf("[%s]before H264_DVR_RealPlay %ld, channel %d, nStream %d\n"
			, __FUNCTION__, xmdev->loginid, req->Channel, req->Codec);
	long handle = H264_DVR_RealPlay(xmdev->loginid, &playstru);	
	if(handle <= 0)
	{
		jtprintf("[%s]start real stream wrong! m_iPlayhandle %ld, channel %d, nStream %d\n"
			, __FUNCTION__, handle, req->Channel, req->Codec);
		return OPEN_VIDEO_STREAM_FAILED;
	}
	else
	{
		jtprintf("[%s]start real stream ok\n", __FUNCTION__);
		stm->playhandle = handle;
		stm->stm.pulling = 1;
		stm->currentencode = VIDEO_ENCODE_VIDEO_H264;//先默认了
		stm->stm.callback = (stream_callback)req->Callback;
		if(stm->stm.userdata) 
			free(stm->stm.userdata);//////danger!!!!!!!!
		stm->stm.userdata = req->UserData;
		rsp->StreamHandle = (long)stm;
		jtprintf("[%s]rsp->StreamHandle %ld\n", __FUNCTION__, rsp->StreamHandle);

		if(H264_DVR_SetRealDataCallBack_V2(handle, xm_real_data_callback_v2, (long)stm)==0)
		{
			jtprintf("[%s]set video callback failed!\n", __FUNCTION__);
			return SET_VIDEO_CALLBACK_FAILED;
		}
	}

	return SUCCESS;
}
static int xm_close_video_stream(struct device *dev, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct xmchannel* chn = NULL;
	struct xmstream* stm = NULL;
	/*chn = (struct xmchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		jtprintf("[%s]chn null\n", __FUNCTION__, chn);
		return INVALID_CHANNEL_NO;
	}
	
	stm = (struct xmstream*)get_stream(&chn->chn.streams, (struct stream*)req->StreamHandle);
	if(stm == NULL)
	{
		return INVALID_STREAM_NO;
	}

	if((long)stm != req->StreamHandle)
	{
		//should we using stm to close it???
		assert(false);
	}
*/

	stm = (struct xmstream*)get_stream_by_dev(dev, (struct stream*)req->StreamHandle);
	if(stm==NULL)
	{
		jtprintf("[%s]stm null\n", __FUNCTION__);
		return INVALID_STREAM_NO;
	}

	if(H264_DVR_SetRealDataCallBack_V2(stm->playhandle, NULL, 0)==0)
	{
		jtprintf("[%s]set video callback failed!\n", __FUNCTION__);
		//return SET_VIDEO_CALLBACK_FAILED;
	}

	if(H264_DVR_StopRealPlay(stm->playhandle))
	{
		jtprintf("[%s]H264_DVR_StopRealPlay ok!\n", __FUNCTION__);
		stm->stm.pulling = 0;
		stm->playhandle = 0L;
		stm->stm.callback = NULL;
		free(stm->stm.userdata);
		stm->stm.userdata = NULL;
	}
	else
	{
		jtprintf("[%s]H264_DVR_StopRealPlay error\n", __FUNCTION__);
		return CLOSE_VIDEO_STREAM_FAILED;
	}
	
	return SUCCESS;
}
static int xm_operator_channel(struct channel *chn, int type, void* data)
{
	if(STOP_AUDIO==type)
	{
		chn->audiocallback = NULL;
		if(chn->audiouserdata) 
		{
			//////danger!!!!!!!!
			void* tmp = chn->audiouserdata;
			chn->audiouserdata = NULL;
			free(tmp);

			return 1;
		}
	}
	else if(START_AUDIO==type)
	{
		struct stOpenAudioStream_Req *req = (struct stOpenAudioStream_Req *)data;
		chn->audiocallback = (stream_callback)req->Callback;
		if(chn->audiouserdata) 
			free(chn->audiouserdata);//////danger!!!!!!!!
		chn->audiouserdata = req->UserData;
	}
	else if(CHCHK_AUDIO_CHANNEL==type)
	{
		if(chn->audiocallback && chn->audiouserdata)
		{
			++(*((int*)data));
		}
	}

	return 0;
}
static int xm_open_audio_stream(struct device *dev, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);

	xmdevice *xmdev = (xmdevice *)dev;
	
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct xmchannel* chn = NULL;
	chn = (struct xmchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		return INVALID_CHANNEL_NO;
	}
	
	if(xmdev->voicehandle == 0)
	{
		long voicehandle = H264_DVR_StartVoiceCom_MR(xmdev->loginid, xm_talk_data_callback, (long)dev);
		if(voicehandle <= 0)
		{
			return OPEN_AUDIO_STREAM_FAILED;
		}
		xmdev->voicehandle = voicehandle;
	}

	rsp->ChannelHandle = (long)do_channel(&dev->channels, req->Channel, xm_operator_channel, START_AUDIO, req);

	return SUCCESS;
}
static int xm_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct xmchannel* chn = NULL;
	chn = (struct xmchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		jtprintf("[%s]no channel %d\n", __FUNCTION__, req->Channel);
		return INVALID_CHANNEL_NO;
	}

	do_channel(&dev->channels, req->Channel, xm_operator_channel, STOP_AUDIO, NULL);

	int havechannelcnt=0;
	do_each_channel(&dev->channels, xm_operator_channel, CHCHK_AUDIO_CHANNEL, &havechannelcnt);
	if(havechannelcnt==0)
	{
		//没有需要音频的通道了，关闭他
		if(H264_DVR_StopVoiceCom(xmdev->voicehandle))
		{
			jtprintf("[%s]H264_DVR_StopVoiceCom ok\n", __FUNCTION__);
			xmdev->voicehandle = 0;
		}
		else
		{
			jtprintf("[%s]H264_DVR_StopVoiceCom error\n", __FUNCTION__);
			return CLOSE_AUDIO_STREAM_FAILED;
		}		
	}

	return SUCCESS;
}

static int xm_get_config(struct device *dev, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);	
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;
	
	switch(req->Type)
	{
		case GET_ENCODE_CONFIG://获取编码配置
		{		
			if(time(0)-xmdev->dev.encodeinfo.last_get_time > 120)
			{
				//大于120秒，重新获取
				jtprintf("[%s]re get encode info\n", __FUNCTION__);
				struct SDK_EncodeConfigAll_SIMPLIIFY EncodeConfig;
				unsigned long dwRetLen = 0;
				int nWaitTime = 10000;

				BOOL bSuccess = H264_DVR_GetDevConfig(xmdev->loginid, E_SDK_CONFIG_SYSENCODE_SIMPLIIFY, -1,
							(char *)&EncodeConfig, sizeof(SDK_EncodeConfigAll_SIMPLIIFY),&dwRetLen,nWaitTime);
				if (bSuccess && dwRetLen == sizeof (SDK_EncodeConfigAll_SIMPLIIFY))
				{
					xm_fill_encode_info((struct device*)xmdev, &EncodeConfig);
					//获取成功
					jtprintf("[%s]H264_DVR_GetDevConfig ok, dwRetLen %lu\n", __FUNCTION__, dwRetLen);
				}
				else
				{	
					jtprintf("[%s]GetConfig Wrong:%d,RetLen:%ld  !=  %d\n"
								, __FUNCTION__,bSuccess,dwRetLen,sizeof (SDK_EncodeConfigAll_SIMPLIIFY));
					return GET_CONFIG_FAILED;
				}							
			}

			struct encode_info* einfo = (struct encode_info*)malloc(sizeof(struct encode_info));
			if(req->Codec==0)
			{
				memcpy(einfo, &xmdev->dev.encodeinfo.encode_info[req->Channel].mainencode, sizeof(struct encode_info));
			}
			else
			{
				memcpy(einfo, &xmdev->dev.encodeinfo.encode_info[req->Channel].sub1encode, sizeof(struct encode_info));
			}

			rsp->Config = (char*)einfo;
			rsp->Size = 1;
			
			break;
		}
	}

	return SUCCESS;
}
static int xm_set_config(struct device *dev)
{
	jtprintf("[%s]\n", __FUNCTION__);

	return SUCCESS;
}
static int xm_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);	
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	xmdevice *xmdev = (xmdevice *)dev;
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(H264_DVR_SetupAlarmChan(xmdev->loginid))
	{
		dev->alarmcallback = (stream_callback)req->Callback;
		if(dev->alarmuserdata)
		{	
			free(dev->alarmuserdata);//danger!!!!!!!!!!!!!
			dev->alarmuserdata = req->UserData;
		}
		rsp->DeviceHandle = (long long)xmdev;
		jtprintf("[%s]xmdev H264_DVR_SetupAlarmChan ok\n", __FUNCTION__);
	}
	else
	{	
		dev->alarmcallback = NULL;	
		if(dev->alarmuserdata)
		{
			free(req->UserData);
			dev->alarmuserdata = NULL;
		}
	
		jtprintf("[%s]xmdev H264_DVR_SetupAlarmChan failed\n", __FUNCTION__);
		return OPEN_ALARM_STREAM_FAILED;
	}

	return SUCCESS;
}
static int  xm_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}
	
	xmdevice *xmdev = (xmdevice *)dev;

	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	dev->alarmcallback = NULL;
	if(dev->alarmuserdata)
	{
		free(dev->alarmuserdata);
		dev->alarmuserdata = NULL;
	}
	
	if(H264_DVR_CloseAlarmChan(xmdev->loginid))
	{
		jtprintf("[%s]xmdev H264_DVR_SetupAlarmChan ok\n", __FUNCTION__);
	}
	else
	{
		jtprintf("[%s]xmdev H264_DVR_SetupAlarmChan failed\n", __FUNCTION__);
		return CLOSE_ALARM_STREAM_FAILED;
	}
	
	return SUCCESS;
}
static int xm_ptz_control(struct device * dev, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
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
				jtprintf("[%s]dev %p, PTZ_MOVE, unkonwn direct\n", __FUNCTION__, dev);
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
			jtprintf("[%s]dev %p, PTZ, unkonwn cmd %d\n", __FUNCTION__, dev, req->Action);
			return UNKONWN_PTZ_COMMAND;
	}

	return 0;
}
