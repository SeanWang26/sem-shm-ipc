#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "dhmanager.h"
#include "../devicetype.h"
#include "../jtprintf.h"

static int g_dh_initaled = 0;

static int dh_device_init(struct dhdevice *dev);

static int dh_init(struct device *dev);
static int dh_uninit(struct device *dev);
static int dh_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
static int dh_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
static int dh_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
static int dh_close_video_stream(struct device *dev, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
static int dh_open_audio_stream(struct device *dev, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
static int dh_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
static int dh_get_config(struct device *dev, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
static int dh_set_config(struct device *, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
static int dh_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
static int dh_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
static int dh_ptz_control(struct device *, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
static int dh_set_system_time(struct device *dev, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);

static struct device_ops dh_ops = 
{
	sizeof(struct dhdevice),
	DEVICE_DH,
    dh_init,
    dh_uninit,
    dh_login,
    dh_logout,
	dh_open_video_stream,
	dh_close_video_stream,
	dh_open_audio_stream,
	dh_close_audio_stream,
	dh_get_config,
	dh_set_config,
	dh_open_alarm_stream,
	dh_close_alarm_stream,
	dh_ptz_control,
	dh_set_system_time
};

//initstream
//initchannel

void dh_disconnect_callback(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
	FIND_DEVICE_BEGIN(struct dhdevice,DEVICE_DH)
	{
		if(dev->loginid == lLoginID)
		{
			dh_device_init(dev);
			jtprintf("[%s]%p, %s, %lu\n", __FUNCTION__, (void*)dwUser, pchDVRIP, nDVRPort);
			break;
		}
	}
	FIND_DEVICE_END
}
static int dh_pack_type_convert(int type)
{
	if(0==type)
		return I_FRAME;
	else if(1==type) 
		return P_FRAME;
	else if(2==type)
		return B_FRAME;

	return UNKNOWN_FRAME;
}

static DH_RealPlayType RealPlayTypeConvert(int Codec)
{
	switch(Codec)
	{
		case 0:
			return DH_RType_Realplay_0;
		case 1:
			return DH_RType_Realplay_1;
		case 2:
			return DH_RType_Realplay_2;
		case 3:
			return DH_RType_Realplay_3;
	}

	return DH_RType_Realplay_0;
}

/**/
static void dh_real_data_callback_v2(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LDWORD dwUser)
{
	//lock
	struct dhstream* stream = (struct dhstream*)dwUser;
	jtprintf("dh get frame type %d, size %u, user %d\n", dwDataType, dwBufSize, dwUser);


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

	/*st_stream_data stmdata;
	stmdata.streamtype = VIDEO_STREAM_DATA;
	stmdata.pdata= pFrame->pPacketBuffer;
	stmdata.datalen = pFrame->dwPacketSize;
	stmdata.stream_info.video_stream_info.encode = stream->currentencode;
	stmdata.stream_info.video_stream_info.frametype = dh_pack_type_convert((enum MEDIA_PACK_TYPE)pFrame->nPacketType);
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
*/
	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
}
void dh_talk_data_callback(LONG lTalkHandle, char *pDataBuf, long dwBufSize, char byAudioFlag, long dwUser)
{
	//lock
	FIND_DEVICE_BEGIN(struct dhdevice,DEVICE_DH)
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
					chn->audiocallback(CALLBACK_TYPE_AUDIO_STREAM, &stmdata, &chn->audiouserdata);
				}
			}
		
		
			break;
		}

	}
	FIND_DEVICE_END

	//BOOL bResult = TRUE;
}

static inline int dh_handle_alarm(dhdevice *device, char *pBuf, unsigned long dwBufLen)
{
	/*SDK_AlarmInfo alarmInfo;
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
		
	}*/
	
	//if(device->dev.alarmcallback)
	//	device->dev.alarmcallback(device->dev.alarmuserdata);
	
	return 0;
}

static BOOL dh_mess_callback(LONG lCommand, LLONG lLoginID, char *pBuf, DWORD dwBufLen, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
	//lock
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		assert(device->obj.type == OBJECT_TYPE_DEVICE);
		if(!device->deleted && device->ops->type == DEVICE_DH )
		{	
			if(((dhdevice*)device)->loginid == lLoginID)
			{				
				dh_handle_alarm((dhdevice*)device, pBuf, dwBufLen);
				break;
			}
		}
	}
	
	return 1;
}

static int dh_resolution_convert(int resolution, int *width, int *height)
{
	//jtprintf("[%s]type %d\n", __FUNCTION__, resolution);

/*	switch(resolution)
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
	}*/

	return 0;
}

static int dh_get_encode_mode(int type)
{
	/*switch(type)
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
	}*/

	return 0;
}
static int dh_fill_encode_info(struct device* dev, DHDEV_PREVIEW_CFG *EncodeConfig)
{
	dev->encodeinfo.last_get_time = time(0);

	//memset????
	
	for(int i=0; i<MAX_CHANNEL_ENCODE_INFO &&  i<DH_MAX_CHANNUM; ++i)
	{
		//主
		/*dev->encodeinfo.encode_info[i].mainencode.enable = EncodeConfig[i].dstMainFmt.bVideoEnable;
		dev->encodeinfo.encode_info[i].mainencode.encodetype 
			= dh_get_encode_mode(EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iCompression);
		dev->encodeinfo.encode_info[i].mainencode.fps = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.nFPS;

		dh_resolution_convert(EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iResolution
							, &dev->encodeinfo.encode_info[i].mainencode.width
							, &dev->encodeinfo.encode_info[i].mainencode.height);

		dev->encodeinfo.encode_info[i].mainencode.quality = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iQuality;
		dev->encodeinfo.encode_info[i].mainencode.bitrate = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.nBitRate;
		dev->encodeinfo.encode_info[i].mainencode.bitratectl = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iBitRateControl;
		dev->encodeinfo.encode_info[i].mainencode.gop = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iGOP;

		//辅1
		dev->encodeinfo.encode_info[i].sub1encode.enable = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.bVideoEnable;
		dev->encodeinfo.encode_info[i].sub1encode.encodetype 
			= dh_get_encode_mode(EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iCompression);
		dev->encodeinfo.encode_info[i].sub1encode.fps = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.nFPS;
		
		dh_resolution_convert(EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iResolution
							, &dev->encodeinfo.encode_info[i].sub1encode.width
							, &dev->encodeinfo.encode_info[i].sub1encode.height);
		
		dev->encodeinfo.encode_info[i].sub1encode.quality = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iQuality;
		dev->encodeinfo.encode_info[i].sub1encode.bitrate = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.nBitRate;
		dev->encodeinfo.encode_info[i].sub1encode.bitratectl = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iBitRateControl;
		dev->encodeinfo.encode_info[i].sub1encode.gop = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iGOP;
	*/}

	return 0;
};
int dh_lib_init()
{
	jtprintf("[%s]\n", __FUNCTION__);
	if(!g_dh_initaled && CLIENT_Init(dh_disconnect_callback, (unsigned long)&devicelist))
	{
		CLIENT_SetDVRMessCallBack(dh_mess_callback, (unsigned long)&devicelist);
		jtprintf("[%s]init success %d\n", __FUNCTION__, g_dh_initaled);
		++g_dh_initaled;
	}

	return SUCCESS;
}

struct dhdevice *dh_alloc_device()
{
	dhdevice *device = (dhdevice *)_alloc_device(&dh_ops);
	if(device)
	{
		dh_device_init(device);
		return device;
	}

	return NULL;
}

static int dh_init(struct device *dev)
{
	jtprintf("[%s]\n", __FUNCTION__);
	if(!g_dh_initaled && CLIENT_Init(dh_disconnect_callback, (unsigned long)&devicelist))
	{
		CLIENT_SetDVRMessCallBack(dh_mess_callback, (unsigned long)&devicelist);
		jtprintf("[%s]init success %d\n", __FUNCTION__, g_dh_initaled);
		++g_dh_initaled;
	}

	return SUCCESS;
}

static int dh_uninit(struct device *dev)
{
	if(--g_dh_initaled)
	{
		g_dh_initaled=0;
		CLIENT_Cleanup();
	}
	
	return SUCCESS;
}

static int dh_device_init(struct dhdevice *dev)
{
	struct channel* channel;
	LIST_FOR_EACH_ENTRY(channel, &dev->dev.channels, struct channel, entry)
	{
		struct dhchannel* dhchannel = (struct dhchannel*)channel;
		assert(dhchannel->chn.obj.type == OBJECT_TYPE_CHANNEL);
		struct stream* stream;
		LIST_FOR_EACH_ENTRY(stream, &dhchannel->chn.streams, struct stream, entry)
		{
			struct dhstream* dhstream = (struct dhstream*)stream;
			assert(dhstream->stm.obj.type == OBJECT_TYPE_STREAM);
			dhstream->currentencode = VIDEO_ENCODE_UNKNOW;
			dhstream->playhandle = DH_INVALIDE_PLAYHANDLE;

			stream_init(&dhstream->stm);
		}

		channel_init(&dhchannel->chn);
	}

	dev->voicehandle = 0;
	dev->loginid = DH_INVALIDE_LOGINID;
	dev->ProtoVer = 0;
	memset(&dev->info, 0, sizeof(dev->info));

	device_init(&dev->dev);

	return 0;
}

static int dh_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	dh_init(dev);//not good

	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);

	if(get_device(dev)==NULL) add_device(dev);

	struct dhdevice *dhdev = (dhdevice *)dev;
	if(dhdev==NULL)
	{
		jtprintf("[%s]dhdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}
	
	strcpy(dev->ip, req->Ip);
	dev->port = req->Port;
	strcpy(dev->user, req->User);
	strcpy(dev->password, req->Password);
	
	int err = 0;
	
	dhdev->loginid = CLIENT_Login((char*)dev->ip, dev->port,(char*)dev->user, (char*)dev->password,(NET_DEVICEINFO*)(&dhdev->info),&err);		
	if(dhdev->loginid)
	{
		int outlen=0;
		CLIENT_QueryDevState(dhdev->loginid, DH_DEVSTATE_PROTOCAL_VER, (char*)&dhdev->ProtoVer, sizeof(dhdev->ProtoVer), &outlen, 1000);

		rsp->DeviceHandle = (long long)dhdev;
		jtprintf("[%s]dhdev login success, %s, %d, %s, %s, %ld, pro %d\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, dhdev->loginid, dhdev->ProtoVer);

		return SUCCESS;
	}
	else
	{
		jtprintf("[%s]dhdev login failed, %s, %d, %s, %s, error %d, loginid %d\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, err, dhdev->loginid);
		return LOGIN_FAILED;
	}

	return LOGIN_FAILED;
}
static int dh_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	dhdevice *dhdev = (dhdevice *)dev;
	if(dhdev==NULL)
	{
		jtprintf("[%s]dhdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(CLIENT_Logout(dhdev->loginid))
	{
		dh_device_init(dhdev);
		jtprintf("[%s]dhdev dh_logout success\n", __FUNCTION__);
		return SUCCESS;		
	}
	else
	{
		jtprintf("[%s]dhdev login failed\n", __FUNCTION__);
		return -2;
	}

	return -2;
}

static int dosaveraw( unsigned char* data, int len)
{
	static FILE* fp=fopen("/home/raw.bin", "wb+");;
	if(fp==NULL)
		return -1;

	return fwrite(data,len,1,fp);
}

void CALLBACK Static_RealDataCallBackEx(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG param, LDWORD dwUser)
{
	//jtprintf("[%s]fRealDataCallBackEx\n", __FUNCTION__);
	dosaveraw(pBuffer, dwBufSize);

	jtprintf("[fream head, before]type %d, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, (%d)\n"
											, dwDataType, pBuffer[0], pBuffer[1]
											, pBuffer[2], pBuffer[3]
											, pBuffer[4], pBuffer[5]
											, pBuffer[6], pBuffer[7]
											, pBuffer[8], pBuffer[9]
											, pBuffer[10], pBuffer[11]
											, pBuffer[12], pBuffer[13]
											, pBuffer[14], pBuffer[15]
											, pBuffer[16], pBuffer[17]
											, pBuffer[18], pBuffer[19]
											, pBuffer[20], pBuffer[21]
											, pBuffer[22], pBuffer[23]
											, pBuffer[24], pBuffer[25]
											, pBuffer[26], pBuffer[27]
											, pBuffer[28], pBuffer[29]
											, pBuffer[30], pBuffer[31]
											, pBuffer[32], pBuffer[33]
											, pBuffer[34], pBuffer[35]
											, pBuffer[36], pBuffer[37]
											, pBuffer[38], pBuffer[39]
											, dwBufSize); 


}

static int dh_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	dhdevice *dhdev = (dhdevice *)dev;
	if(dhdev==NULL)
	{
		jtprintf("[%s]dhdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	//lock??
	struct dhchannel* chn = NULL;
	struct dhstream* stm = NULL;
	chn = (struct dhchannel*)get_channel(&dev->channels, req->Channel);
	if(chn)
	{
		stm = (struct dhstream*)get_stream_by_id(&chn->chn.streams, req->Codec);
	}
	else
	{
		chn = (struct dhchannel*)alloc_channel(sizeof(struct dhchannel));
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
		stm = (struct dhstream*)alloc_stream(sizeof(struct dhstream));
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

	jtprintf("[%s]before H264_DVR_RealPlay %ld, channel %d, nStream %d\n"
			, __FUNCTION__, dhdev->loginid, req->Channel, req->Codec);

	long handle = CLIENT_RealPlayEx(dhdev->loginid, req->Channel, NULL, RealPlayTypeConvert(req->Codec));	
	if(handle == 0)
	{
		jtprintf("[%s]start real stream wrong! m_iPlayhandle %ld, channel %d, nStream %d\n"
			, __FUNCTION__, handle, req->Channel, req->Codec);
		return OPEN_VIDEO_STREAM_FAILED;
	}
	else
	{
		jtprintf("[%s]start real stream ok\n", __FUNCTION__);

		//方便用户修改上一次的 userdata数据
		if(stm->stm.callback)
			stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_OPENED, NULL, &stm->stm.userdata);

		stm->playhandle = handle;
		stm->stm.pulling = 1;
		stm->currentencode = VIDEO_ENCODE_VIDEO_H264;//先默认了
		stm->stm.callback = (jt_stream_callback)req->Callback;
		stm->stm.userdata = req->UserData;
		rsp->StreamHandle = (long)stm;
		jtprintf("[%s]rsp->StreamHandle %ld\n", __FUNCTION__, rsp->StreamHandle);

		if(CLIENT_SetRealDataCallBackEx(handle, Static_RealDataCallBackEx, (long)stm, 0x00000001)==0)
		{
			jtprintf("[%s]set video callback failed!\n", __FUNCTION__);
			return SET_VIDEO_CALLBACK_FAILED;
		}
	}

	return SUCCESS;
}
static int dh_close_video_stream(struct device *dev, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	dhdevice *dhdev = (dhdevice *)dev;
	if(dhdev==NULL)
	{
		jtprintf("[%s]dhdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct dhchannel* chn = NULL;
	struct dhstream* stm = NULL;
	/*chn = (struct dhchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		jtprintf("[%s]chn null\n", __FUNCTION__, chn);
		return INVALID_CHANNEL_NO;
	}
	
	stm = (struct dhstream*)get_stream(&chn->chn.streams, (struct stream*)req->StreamHandle);
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

	stm = (struct dhstream*)get_stream_by_dev(dev, (struct stream*)req->StreamHandle);
	if(stm==NULL)
	{
		jtprintf("[%s]stm null\n", __FUNCTION__);
		return INVALID_STREAM_NO;
	}

	if(CLIENT_SetRealDataCallBack(stm->playhandle, NULL, 0)==0)
	{
		jtprintf("[%s]set video callback failed!\n", __FUNCTION__);
		//return SET_VIDEO_CALLBACK_FAILED;
	}

	if(CLIENT_StopRealPlayEx(stm->playhandle))
	{
		jtprintf("[%s]H264_DVR_StopRealPlay ok!\n", __FUNCTION__);

		if(stm->stm.callback)
			stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_CLOSEED, SUCCESS, &stm->stm.userdata);

		stm->stm.pulling = 0;
		stm->playhandle = DH_INVALIDE_PLAYHANDLE;
		stm->stm.callback = NULL;
	}
	else
	{
		jtprintf("[%s]H264_DVR_StopRealPlay error\n", __FUNCTION__);
		return CLOSE_VIDEO_STREAM_FAILED;
	}
	
	return SUCCESS;
}
static int dh_operator_channel(struct channel *chn, int type, void* data)
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
		chn->audiocallback = (jt_stream_callback)req->Callback;
		//if(chn->audiouserdata) 
		//	free(chn->audiouserdata);//////danger!!!!!!!!
		chn->audiouserdata = req->UserData;
	}
	else if(CHECK_AUDIO_CHANNEL==type)
	{
		if(chn->audiocallback && chn->audiouserdata)
		{
			++(*((int*)data));
		}
	}

	return 0;
}

static int playing_stream(struct stream *stm, void* data)
{
	if(stm->pulling)
		return 1;
}

static int dh_open_audio_stream(struct device *dev, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);

	dhdevice *dhdev = (dhdevice *)dev;
	
	if(dhdev==NULL)
	{
		jtprintf("[%s]dhdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct dhchannel* chn = NULL;
	chn = (struct dhchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		return INVALID_CHANNEL_NO;
	}

	//返回正在播放视屏的流
	struct dhstream *stm = (struct dhstream *)get_special_stream(&chn->chn.streams, playing_stream, NULL);
	if(stm == NULL)
	{
		return INVALID_STREAM_NO;
	}

	//using a flag????START_AUDIO  STOP_AUDIO
	rsp->ChannelHandle = (long)do_channel(&dev->channels, req->Channel, dh_operator_channel, START_AUDIO, req);

	if(!CLIENT_OpenSound(stm->playhandle))
	{
		do_channel(&dev->channels, req->Channel, dh_operator_channel, STOP_AUDIO, NULL);
		rsp->ChannelHandle==NULL;
		return OPEN_AUDIO_STREAM_FAILED;
	}

	return SUCCESS;
}
static int dh_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	dhdevice *dhdev = (dhdevice *)dev;
	if(dhdev==NULL)
	{
		jtprintf("[%s]dhdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct dhchannel* chn = NULL;
	chn = (struct dhchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		jtprintf("[%s]no channel %d\n", __FUNCTION__, req->Channel);
		return INVALID_CHANNEL_NO;
	}

	do_channel(&dev->channels, req->Channel, dh_operator_channel, STOP_AUDIO, NULL);

	int havechannelcnt=0;
	do_each_channel(&dev->channels, dh_operator_channel, CHECK_AUDIO_CHANNEL, &havechannelcnt);
	if(havechannelcnt==0)
	{
		//没有需要音频的通道了，关闭他
		if(CLIENT_CloseSound())
		{
			jtprintf("[%s]H264_DVR_StopVoiceCom ok\n", __FUNCTION__);
		}
		else
		{
			jtprintf("[%s]H264_DVR_StopVoiceCom error\n", __FUNCTION__);
			return CLOSE_AUDIO_STREAM_FAILED;
		}		
	}

	return SUCCESS;
}

static int dh_get_config(struct device *dev, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);	
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	dhdevice *dhdev = (dhdevice *)dev;
	
	switch(req->Type)
	{
		case GET_ENCODE_CONFIG://获取编码配置
		{		
			if(time(0)-dhdev->dev.encodeinfo.last_get_time > 120)
			{
				//大于120秒，重新获取
				jtprintf("[%s]re get encode info\n", __FUNCTION__);

				DWORD dwRet = 0;
				DHDEV_PREVIEW_CFG EncodeConfig[32] = {0};
				if (CLIENT_GetDevConfig(dhdev->loginid, DH_DEV_PREVIEWCFG, -1, EncodeConfig, sizeof(EncodeConfig[32]), &dwRet, 3000))
				{
					dh_fill_encode_info((struct device*)dhdev, EncodeConfig);
					//获取成功
					///jtprintf("[%s]H264_DVR_GetDevConfig ok, dwRetLen %lu\n", __FUNCTION__, dwRetLen);
				}
				else
				{	
					//jtprintf("[%s]GetConfig Wrong:%d,RetLen:%ld  !=  %d\n"
					//			, __FUNCTION__,bSuccess,dwRetLen,sizeof (SDK_EncodeConfigAll_SIMPLIIFY));
					return GET_CONFIG_FAILED;
				}							
			}

			struct encode_info* einfo = (struct encode_info*)malloc(sizeof(struct encode_info));
			if(req->Codec==0)
			{
				memcpy(einfo, &dhdev->dev.encodeinfo.encode_info[req->Channel].mainencode, sizeof(struct encode_info));
			}
			else
			{
				memcpy(einfo, &dhdev->dev.encodeinfo.encode_info[req->Channel].sub1encode, sizeof(struct encode_info));
			}

			rsp->Config = (char*)einfo;
			rsp->Size = 1;
			
			break;
		}
	}

	return SUCCESS;
}
static int dh_set_config(struct device *, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);

	return SUCCESS;
}
static int dh_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);	
	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	dhdevice *dhdev = (dhdevice *)dev;
	if(dhdev==NULL)
	{
		jtprintf("[%s]dhdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(CLIENT_StartListenEx(dhdev->loginid))
	{
		if(dev->alarmcallback)
			dev->alarmcallback(CALLBACK_TYPE_ALARM_STREAM_OPENED, (void*)OPEN_AUDIO_STREAM_FAILED, &dev->alarmuserdata);

		dev->alarmcallback = (jt_stream_callback)req->Callback;
		dev->alarmuserdata = req->UserData;
		rsp->DeviceHandle = (long long)dhdev;
		jtprintf("[%s]dhdev H264_DVR_SetupAlarmChan ok\n", __FUNCTION__);
	}
	else
	{	
		dev->alarmcallback = NULL;
		if(dev->alarmuserdata)
		{
			dev->alarmuserdata = NULL;
		}
	
		jtprintf("[%s]dhdev H264_DVR_SetupAlarmChan failed\n", __FUNCTION__);
		return OPEN_ALARM_STREAM_FAILED;
	}

	return SUCCESS;
}
static int  dh_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}
	
	dhdevice *dhdev = (dhdevice *)dev;

	if(dhdev==NULL)
	{
		jtprintf("[%s]dhdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	dev->alarmcallback = NULL;
	if(dev->alarmuserdata)
	{
		//free(dev->alarmuserdata);
		dev->alarmuserdata = NULL;
	}
	
	if(CLIENT_StopListen(dhdev->loginid))
	{
		jtprintf("[%s]dhdev H264_DVR_SetupAlarmChan ok\n", __FUNCTION__);
	}
	else
	{
		jtprintf("[%s]dhdev H264_DVR_SetupAlarmChan failed\n", __FUNCTION__);
		return CLOSE_ALARM_STREAM_FAILED;
	}
	
	return SUCCESS;
}

static DWORD dh_ptz_direction_convert(int jtdirection)
{
	switch(jtdirection)
	{
		case JPTZ_UP://上
			return (DWORD)DH_PTZ_UP_CONTROL;
		case JPTZ_RIGHT_UP://右上
			return (DWORD)DH_EXTPTZ_RIGHTTOP;
		case JPTZ_RIGHT://右
			return (DWORD)DH_PTZ_RIGHT_CONTROL;
		case JPTZ_RIGHT_DOWN://右下
			return (DWORD)DH_EXTPTZ_RIGHTDOWN;
		case JPTZ_DOWN://下
			return (DWORD)DH_PTZ_DOWN_CONTROL;
		case JPTZ_LEFT_DOWN://左下
			return (DWORD)DH_EXTPTZ_LEFTDOWN;
		case JPTZ_LEFT://左
			return (DWORD)DH_PTZ_LEFT_CONTROL;
		case JPTZ_LEFT_UP://左上
			return (DWORD)DH_EXTPTZ_LEFTTOP;
	}

	return (DWORD)JPTZ_UP;
}

static int dh_ptz_control(struct device * dev, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	dhdevice *dhdev = (dhdevice *)dev;

	switch(req->Action)
	{
		case JPTZ_STOP://停止
			CLIENT_PTZControl(dhdev->loginid, req->Channel, 0, 0, 1);
		break;
		case JPTZ_UP://8个方向
		case JPTZ_RIGHT_UP:
		case JPTZ_RIGHT:
		case JPTZ_RIGHT_DOWN:
		case JPTZ_DOWN:
		case JPTZ_LEFT_DOWN:
		case JPTZ_LEFT:
		case JPTZ_LEFT_UP:
			if(!CLIENT_DHPTZControl(dhdev->loginid, req->Channel, (DWORD)dh_ptz_direction_convert(req->Action), 0, req->Speed, 0, 0))
			{

			}			
		break;
		case JPTZ_PUSH_FAR:
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_ZOOM_ADD_CONTROL, 0, req->Speed, 0, 0);
		break;
		case JPTZ_PULL_NEAR:
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_ZOOM_DEC_CONTROL, 0, req->Speed, 0, 0);
		break;
		case JPTZ_IRIS_ADD:
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_APERTURE_ADD_CONTROL, 0, req->Speed, 0, 0);
		break;
		case JPTZ_IRIS_SUB:
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_APERTURE_DEC_CONTROL, 0, req->Speed, 0, 0);
		break;
		case JPTZ_FOCUS_FAR:
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_FOCUS_ADD_CONTROL, 0, req->Speed, 0, 0);
		break;
		case JPTZ_FOCUS_NEAR:
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_FOCUS_DEC_CONTROL, 0, req->Speed, 0, 0);
		break;
		case JSET_PRESET://设置预置点
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_POINT_SET_CONTROL, 1, req->PresetNum, 3, 0);
		break;
		case JCLEAR_PRESET://清除预置点
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_POINT_DEL_CONTROL, 1, req->PresetNum, 3, 0);
		break;
		case JGOTO_PRESET://转到预置点
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_POINT_MOVE_CONTROL, 1, req->PresetNum, 3, 0);
		break;
		case JADD_TO_LOOP://添加预置点到巡航
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_EXTPTZ_ADDTOLOOP, req->TourNum, req->PresetNum, 3, 0);
		break;
		case JDEL_FROM_LOOP://将预置点从巡航删除
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_EXTPTZ_DELFROMLOOP, req->TourNum, req->PresetNum, 3, 0);
		break;
		case JSTART_LOOP://开始巡航
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_POINT_LOOP_CONTROL, req->TourNum, 2, 3, 76);//76为开始
		break;
		case JSTOP_LOOP://停止巡航
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_PTZ_POINT_LOOP_CONTROL, req->TourNum, 2, 3, 96);//96为停止
		break;
		case JREMOVE_LOOP://删除巡航
			CLIENT_DHPTZControl(dhdev->loginid, req->Channel, DH_EXTPTZ_CLOSELOOP, req->TourNum, 2, 3, 0);
		break;
		JPTZ_RESET://云台复位
			return UNKONWN_PTZ_COMMAND;//大华的不支持复位
		default:
			jtprintf("[%s]dev %p, PTZ, unkonwn cmd %d\n", __FUNCTION__, dev, req->Action);
			return UNKONWN_PTZ_COMMAND;
	}

	return 0;
}
static int dh_set_system_time(struct device *dev, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	if(get_device(dev)==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		return DEVICE_NO_FOUND;		
	}

	jtprintf("[%s]try set time to %d-%d-%d %d:%d:%d\n"
		, __FUNCTION__, req->year, req->month, req->day, req->hour, req->minute, req->second);

	dhdevice *xmdev = (dhdevice *)dev;	

	NET_TIME time;
	time.dwYear = req->year;
	time.dwMonth = req->month;
	time.dwDay = req->day;
	time.dwHour = req->hour;
	time.dwMinute = req->minute;
	time.dwSecond = req->second;

	if(CLIENT_SetupDeviceTime(xmdev->loginid, &time))
	{
		jtprintf("[%s]set time ok\n", __FUNCTION__);
		rsp->DeviceHandle = (long long)(unsigned long)dev;
		rsp->month=req->month;
		rsp->day=req->day;
		rsp->hour=req->hour;
		rsp->minute=req->minute;
		rsp->second=req->second;
		rsp->year=req->year;

		return 0;
	}
	
	return -1;
}
