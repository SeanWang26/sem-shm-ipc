#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "xmmanager.h"
#include "../devicetype.h"
#include "../jtprintf.h"
#include "../create_detached_thread.h"

static int g_xm_initaled = 0;

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
static int xm_set_config(struct device *, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
static int xm_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
static int xm_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
static int xm_ptz_control(struct device *, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
static int xm_set_system_time(struct device *, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);
static int xm_start_talk(struct device *, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp);
static int xm_stop_talk(struct device *, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp);
static int xm_send_talk_data(struct device *, struct stSendTalkData_Req *req, struct stSendTalkData_Rsp *rsp);

static struct device_ops xm_ops = 
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
	xm_ptz_control,
	xm_set_system_time,
	xm_start_talk,
	xm_stop_talk,
	xm_send_talk_data
};

void CALL_METHOD xm_disconnect_callback(long lLoginID, char *pchDVRIP, long nDVRPort, unsigned long dwUser)
{
	jtprintf("[%s]enter\n", __FUNCTION__);
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

static void CALL_METHOD xm_sub_disconnect_callBack(long lLoginID, SubConnType type, long nChannel, long dwUser)
{
	jtprintf("[%s]enter\n", __FUNCTION__);
	FIND_DEVICE_BEGIN(struct xmdevice,DEVICE_XM)
	{
		if(dev->loginid == lLoginID)
		{	
			jtprintf("[%s]%p, %s, %lu\n", __FUNCTION__, (void*)dwUser, type, nChannel);
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
static int dosaveraw(unsigned char* data, int len)
{
	static FILE* fp=fopen("/home/raw.bin", "wb+");;
	if(fp==NULL)
		return -1;

	return fwrite(data,len,1,fp);
}
#pragma pack(push,1)
struct XmInnerHeadStruct
{
//0  0  1 fd 2a 14  0  0    0  0  0  1 61
//0  0  1 fc  2 19 a0 5a c4 52 9a 39 2a 2a  0  0  0  0  0  1 67
	char StartCode[3];	   //00 00 01
	char PackgeType;       //fd非i帧，fc为i帧标识
	char Pad[8];     //    //为i帧时有这8个字节
	int  Len;              //后面的数据长度
};
#pragma pack(pop)

static int xm_real_data_callback_v2(long lRealHandle, const PACKET_INFO_EX *pFrame, unsigned int dwUser)
{
	//创建一个线程局部变量,标识pulling,   todo.............
	
	//lock
	struct xmstream* stream = (struct xmstream*)dwUser;

	if(!stream->stm.pulling)
	{	
		static struct stream* _stream = 0;
		if(_stream != (struct stream*)stream)
			jtprintf("[%s]1 stream->stm.pulling %d, stream %p !!!!!!!!\n"
				, __FUNCTION__, _stream->pulling, _stream);
		_stream = (struct stream*)stream;
		return 0;
	}

	
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

	st_stream_data stmdata;

	if(pFrame->nPacketType == VIDEO_P_FRAME  
		|| pFrame->nPacketType == VIDEO_I_FRAME 
		|| pFrame->nPacketType == VIDEO_B_FRAME
		|| pFrame->nPacketType == VIDEO_BP_FRAME
		|| pFrame->nPacketType == VIDEO_BBP_FRAME
		|| pFrame->nPacketType == VIDEO_J_FRAME)
	{

		//dosaveraw((unsigned char*)pFrame->pPacketBuffer, pFrame->dwPacketSize);
	
		/*jtprintf("[%s]pulling %d, nmemb %u, %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n"
			, __FUNCTION__, stream->stm.pulling, pFrame->dwPacketSize
			, (unsigned char)pFrame->pPacketBuffer[0], (unsigned char)pFrame->pPacketBuffer[1], (unsigned char)pFrame->pPacketBuffer[2], (unsigned char)pFrame->pPacketBuffer[3], (unsigned char)pFrame->pPacketBuffer[4]
			, (unsigned char)pFrame->pPacketBuffer[5], (unsigned char)pFrame->pPacketBuffer[6], (unsigned char)pFrame->pPacketBuffer[7], (unsigned char)pFrame->pPacketBuffer[8] , (unsigned char)pFrame->pPacketBuffer[9]
			, (unsigned char)pFrame->pPacketBuffer[10], (unsigned char)pFrame->pPacketBuffer[11], (unsigned char)pFrame->pPacketBuffer[12], (unsigned char)pFrame->pPacketBuffer[13] , (unsigned char)pFrame->pPacketBuffer[14]
			, (unsigned char)pFrame->pPacketBuffer[15], (unsigned char)pFrame->pPacketBuffer[16], (unsigned char)pFrame->pPacketBuffer[17], (unsigned char)pFrame->pPacketBuffer[18] , (unsigned char)pFrame->pPacketBuffer[19]
			, (unsigned char)pFrame->pPacketBuffer[20], (unsigned char)pFrame->pPacketBuffer[21], (unsigned char)pFrame->pPacketBuffer[22], (unsigned char)pFrame->pPacketBuffer[23] , (unsigned char)pFrame->pPacketBuffer[24]
			, (unsigned char)pFrame->pPacketBuffer[pFrame->dwPacketSize-1]);
*/

		struct channel* chn = (struct channel*)stream->stm.obj.parent;
		struct device* dev = (struct device*)chn->obj.parent;

		stmdata.streamtype = VIDEO_STREAM_DATA;

		if(pFrame->nPacketType == VIDEO_I_FRAME)
		{
			stmdata.pdata= pFrame->pPacketBuffer+16;
			stmdata.datalen = pFrame->dwPacketSize-16;
		}
		else
		{
			stmdata.pdata= pFrame->pPacketBuffer+8;
			stmdata.datalen = pFrame->dwPacketSize-8;
		}
		
		stmdata.stream_info.video_stream_info.frametype = xm_pack_type_convert((enum MEDIA_PACK_TYPE)pFrame->nPacketType);

		if(stream->stm.id==0)//主码流
		{
			stmdata.stream_info.video_stream_info.width = dev->encodeinfo.ch_encode_info[chn->id].mainencode.width;
			stmdata.stream_info.video_stream_info.height = dev->encodeinfo.ch_encode_info[chn->id].mainencode.height;
			stmdata.stream_info.video_stream_info.encode = dev->encodeinfo.ch_encode_info[chn->id].mainencode.encodetype;
			stmdata.stream_info.video_stream_info.fps = dev->encodeinfo.ch_encode_info[chn->id].mainencode.fps;
			stmdata.stream_info.video_stream_info.bitrate = dev->encodeinfo.ch_encode_info[chn->id].mainencode.bitrate;
		}
		else//子码流
		{
			stmdata.stream_info.video_stream_info.width = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.width;
			stmdata.stream_info.video_stream_info.height = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.height;
			stmdata.stream_info.video_stream_info.encode = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.encodetype;
			stmdata.stream_info.video_stream_info.fps = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.fps;
			stmdata.stream_info.video_stream_info.bitrate = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.bitrate;
		}
		
		stmdata.year = pFrame->nYear;
		stmdata.month = pFrame->nMonth;
		stmdata.day = pFrame->nDay;
		stmdata.hour = pFrame->nHour;
		stmdata.minute = pFrame->nMinute;
		stmdata.second = pFrame->nSecond;
		
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		if(stream->stm.llbegintime==0ULL)
		{
			stmdata.llbegintime = ts.tv_sec* + ts.tv_nsec/100;//100ns
			stmdata.llrelativetimetick = ts.tv_sec* + ts.tv_nsec/100;//100ns
		}
		else
		{
			stmdata.llbegintime = stream->stm.llbegintime;
			stmdata.llrelativetimetick = ts.tv_sec* + ts.tv_nsec/100 - stmdata.llbegintime;//100ns
		}
		
		if(stream->stm.callback)
			stream->stm.callback(CALLBACK_TYPE_VIDEO_STREAM, &stmdata, &stream->stm.userdata);

		return 1;
	}	
	else if(pFrame->nPacketType == AUDIO_PACKET)
	{
		//jtprintf("[%s]AUDIO_PACKET %d\n", __FUNCTION__, stream->stm.id);
		struct channel* chn = (struct channel*)stream->stm.obj.parent;
		struct device* dev = (struct device*)chn->obj.parent;

		if(chn->audiocallback)// && chn->audiouserdata)
		{
			//jtprintf("[%s]AUDIO_PACKET %d, pFrame->dwPacketSize %d\n", __FUNCTION__, stream->stm.id, pFrame->dwPacketSize);

			
			stmdata.streamtype = AUDIO_STREAM_DATA;
			stmdata.pdata= pFrame->pPacketBuffer+8;
			stmdata.datalen = pFrame->dwPacketSize-8;
			stmdata.stream_info.audio_stream_info.encode = dev->encodeinfo.ch_encode_info[chn->id].audioencode.encodetype;
			stmdata.stream_info.audio_stream_info.channel = dev->encodeinfo.ch_encode_info[chn->id].audioencode.channel;
			stmdata.stream_info.audio_stream_info.frequency = dev->encodeinfo.ch_encode_info[chn->id].audioencode.frequency;
			stmdata.stream_info.audio_stream_info.depth = dev->encodeinfo.ch_encode_info[chn->id].audioencode.depth;
			stmdata.stream_info.audio_stream_info.bitrate = dev->encodeinfo.ch_encode_info[chn->id].audioencode.bitrate;
			stmdata.year = pFrame->nYear;
			stmdata.month = pFrame->nMonth;
			stmdata.day = pFrame->nDay;
			stmdata.hour = pFrame->nHour;
			stmdata.minute = pFrame->nMinute;
			stmdata.second = pFrame->nSecond;

			chn->audiocallback(CALLBACK_TYPE_AUDIO_STREAM, &stmdata, &chn->audiouserdata);
		}

		return 1;
	}
	else if(pFrame->nPacketType == FILE_HEAD)
	{
		jtprintf("[%s]FILE_HEAD\n", __FUNCTION__, stream->stm.id);
	}
	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
	return 1;
}
void xm_audio_data_callback(LONG lTalkHandle, char *pDataBuf, long dwBufSize, char byAudioFlag, long dwUser)
{
	//打开音频这里也有数据，但是我们还是从xm_real_data_callback_v2里得到数据
	//jtprintf("[%s]enter\n", __FUNCTION__);
	//lock
	//struct xmdevice
	//jtprintf("[%s]lTalkHandle %ld, dwBufSize %d, byAudioFlag %d\n"
	//					, __FUNCTION__, lTalkHandle, dwBufSize, byAudioFlag);
		
	/*FIND_DEVICE_BEGIN(struct xmdevice,DEVICE_XM)
	{
		if(dev->voicehandle == lTalkHandle)
		{
			struct channel* chn; 												
			LIST_FOR_EACH_ENTRY(chn, &dev->dev.channels, struct channel, entry) 
			{
				if(chn->audiocallback)// && chn->audiouserdata)
				{
					st_stream_data stmdata;
					stmdata.streamtype = AUDIO_STREAM_DATA;
					stmdata.pdata= pDataBuf+8;
					stmdata.datalen = dwBufSize-8;
					stmdata.stream_info.audio_stream_info.encode = dev->dev.encodeinfo.ch_encode_info[chn->id].audioencode.encodetype;
					stmdata.stream_info.audio_stream_info.channel = dev->dev.encodeinfo.ch_encode_info[chn->id].audioencode.channel;
					stmdata.stream_info.audio_stream_info.frequency = dev->dev.encodeinfo.ch_encode_info[chn->id].audioencode.frequency;
					stmdata.stream_info.audio_stream_info.depth = dev->dev.encodeinfo.ch_encode_info[chn->id].audioencode.depth;
					stmdata.stream_info.audio_stream_info.bitrate = dev->dev.encodeinfo.ch_encode_info[chn->id].audioencode.bitrate;
					//stmdata.year = pFrame->nYear;
					//stmdata.month = pFrame->nMonth;
					//stmdata.day = pFrame->nDay;
					//stmdata.hour = pFrame->nHour;
					//stmdata.minute = pFrame->nMinute;
					//stmdata.second = pFrame->nSecond;
					
					jtprintf("[%s]device %p, lTalkHandle %ld, chn %d\n"
						, __FUNCTION__, (void*)dev, lTalkHandle, chn->id);
					chn->audiocallback(CALLBACK_TYPE_AUDIO_STREAM, &stmdata, &chn->audiouserdata);
				}
			}
			
			break;
		}

	}
	FIND_DEVICE_END
	*/

	//BOOL bResult = TRUE;
}

void* xm_send_talk_data_thread(void* user)
{
	jtprintf("[%s]enter\n", __FUNCTION__);
	/*jtprintf("[%s]xm_send_talk_data_thread\n", __FUNCTION__);
	struct device *dev = (struct device*)user;
	share_lock_devicelist(__FUNCTION__);
	dev = get_device(dev);
	if(dev==NULL)
	{
		jtprintf("[%s]dev %p no find\n", __FUNCTION__, dev);
		unlock_devicelist(__FUNCTION__);
		return DEVICE_NO_FOUND;		
	}

	unlock_devicelist(__FUNCTION__);
	//use dev' lock to lock to
	//devicelock(dev->lock);

	while(1)
	{
		if(wait_talk_date_comming())
		{
			break;
		}

		
	}

	
	dev->talkcallback(xmdev->dev.talkuserdata);
	dev->talkthreadid = 0;
	//devicelock(dev->lock);
*/
	return NULL;
}
static int xm_channelnum_convert(struct xmdevice* dev, int channelnum)
{
	return channelnum;
}

static inline int xm_handle_alarm(xmdevice *device, char *pBuf, unsigned long dwBufLen)
{
	SDK_AlarmInfo alarmInfo;
	memcpy (&alarmInfo, pBuf, dwBufLen); 	

	jtprintf("[%s]alarmInfo.iEvent %d, alarmInfo.nChannel %d, alarmInfo.iStatus %d\n"
		, __FUNCTION__, alarmInfo.iEvent, alarmInfo.nChannel, alarmInfo.iStatus);

	int reason = ALARM_TYPE_UNKNOWN;
	if ( SDK_EVENT_CODE_NET_ALARM == alarmInfo.iEvent 
		|| SDK_EVENT_CODE_MANUAL_ALARM == alarmInfo.iEvent 
		|| SDK_EVENT_CODE_LOCAL_ALARM == alarmInfo.iEvent )
	{
		jtprintf("[%s]SDK_EVENT_CODE_NET_ALARM SDK_EVENT_CODE_MANUAL_ALARM SDK_EVENT_CODE_LOCAL_ALARM\n", __FUNCTION__);
	}
	else if (SDK_EVENT_CODE_LOCAL_ALARM == alarmInfo.iEvent ) 
	{
		reason = ALARM_TYPE_INPUT;
		jtprintf("[%s]SDK_EVENT_CODE_VIDEO_MOTION\n", __FUNCTION__);
	}
	else if ( SDK_EVENT_CODE_VIDEO_MOTION == alarmInfo.iEvent ) 
	{
		reason = ALARM_TYPE_VIDEO_MOTION;
		jtprintf("[%s]SDK_EVENT_CODE_VIDEO_MOTION\n", __FUNCTION__);
	}
	else if ( SDK_EVENT_CODE_VIDEO_LOSS == alarmInfo.iEvent  )
	{
		reason = ALARM_TYPE_VIDEO_LOSS;
		jtprintf("[%s]SDK_EVENT_CODE_VIDEO_LOSS\n", __FUNCTION__);
	}
	else if ( SDK_EVENT_CODE_VIDEO_BLIND == alarmInfo.iEvent )
	{
		reason = ALARM_TYPE_VIDEO_BLIND;
		jtprintf("[%s]SDK_EVENT_CODE_VIDEO_BLIND\n", __FUNCTION__);
	}
	else if ( SDK_EVENT_CODE_STORAGE_FAILURE == alarmInfo.iEvent )
	{
		reason = ALARM_TYPE_STORAGE_NOT_EXIST;
		jtprintf("[%s]SDK_EVENT_CODE_STORAGE_FAILURE\n", __FUNCTION__);	
	}
	else if ( SDK_EVENT_CODE_LOW_SPACE == alarmInfo.iEvent )
	{
		reason = ALARM_TYPE_STORAGE_NOT_EXIST;
		jtprintf("[%s]SDK_EVENT_CODE_LOW_SPACE\n", __FUNCTION__);
	}
	
	if(reason==ALARM_TYPE_UNKNOWN)
	{
		jtprintf("[%s]reason==ALARM_TYPE_UNKNOWN\n", __FUNCTION__);
		return 0;
	}
	
	st_stream_data alarm;
	alarm.streamtype = ALARM_STREAM_DATA;
	alarm.stream_info.alarm_stream_info.reason = reason;
	alarm.stream_info.alarm_stream_info.channelid = xm_channelnum_convert(device, alarmInfo.nChannel);

	if(device->dev.alarmcallback)
		device->dev.alarmcallback(CALLBACK_TYPE_ALARM_STREAM, &alarm, &device->dev.alarmuserdata);

	return 0;
}

static bool CALL_METHOD xm_mess_callback(long lLoginID, char *pBuf, unsigned long dwBufLen, long dwUser)
{
	//lock
	jtprintf("[%s]enter\n", __FUNCTION__);
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
			return VIDEO_ENCODE_UNKNOWN;
		break;
		case SDK_CAPTURE_COMP_MS_MPEG4:		///< MS MPEG4。
			return VIDEO_ENCODE_UNKNOWN;
		break;
		case SDK_CAPTURE_COMP_MPEG2: 		///< MPEG2。
			return VIDEO_ENCODE_UNKNOWN;
		break;
		case SDK_CAPTURE_COMP_MPEG1: 		///< MPEG1。
			return VIDEO_ENCODE_UNKNOWN;
		break;
		case SDK_CAPTURE_COMP_H263:			///< H.263
			return VIDEO_ENCODE_UNKNOWN;
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
		dev->encodeinfo.ch_encode_info[i].mainencode.enable = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.bVideoEnable;
		dev->encodeinfo.ch_encode_info[i].mainencode.encodetype = xm_get_encode_mode(EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iCompression);
		dev->encodeinfo.ch_encode_info[i].mainencode.fps = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.nFPS;

		xm_resolution_convert(EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iResolution
							, &dev->encodeinfo.ch_encode_info[i].mainencode.width
							, &dev->encodeinfo.ch_encode_info[i].mainencode.height);

		dev->encodeinfo.ch_encode_info[i].mainencode.quality = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iQuality;
		dev->encodeinfo.ch_encode_info[i].mainencode.bitrate = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.nBitRate;
		dev->encodeinfo.ch_encode_info[i].mainencode.bitratectl = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iBitRateControl;
		dev->encodeinfo.ch_encode_info[i].mainencode.gop = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.vfFormat.iGOP;

		//辅1
		dev->encodeinfo.ch_encode_info[i].sub1encode.enable = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.bVideoEnable;
		dev->encodeinfo.ch_encode_info[i].sub1encode.encodetype = xm_get_encode_mode(EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iCompression);
		dev->encodeinfo.ch_encode_info[i].sub1encode.fps = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.nFPS;
		
		xm_resolution_convert(EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iResolution
							, &dev->encodeinfo.ch_encode_info[i].sub1encode.width
							, &dev->encodeinfo.ch_encode_info[i].sub1encode.height);
		
		dev->encodeinfo.ch_encode_info[i].sub1encode.quality = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iQuality;
		dev->encodeinfo.ch_encode_info[i].sub1encode.bitrate = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.nBitRate;
		dev->encodeinfo.ch_encode_info[i].sub1encode.bitratectl = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iBitRateControl;
		dev->encodeinfo.ch_encode_info[i].sub1encode.gop = EncodeConfig->vEncodeConfigAll[i].dstExtraFmt.vfFormat.iGOP;

		//音频
		dev->encodeinfo.ch_encode_info[i].audioencode.encodetype = AUDIO_G711;
		dev->encodeinfo.ch_encode_info[i].audioencode.frequency = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.afFormat.nFrequency;
		dev->encodeinfo.ch_encode_info[i].audioencode.bitrate = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.afFormat.nBitRate;
		dev->encodeinfo.ch_encode_info[i].audioencode.channel = 1;
		dev->encodeinfo.ch_encode_info[i].audioencode.depth = 16;

	}

	return 0;
}

struct xmdevice *xm_alloc_device(void)
{
	xmdevice *device = (xmdevice *)_alloc_device(&xm_ops);
	if(device)
	{
		xm_device_init(device);
		return device;
	}

	return NULL;
}

int xm_lib_init()
{
	jtprintf("[%s]\n", __FUNCTION__);
	if(!g_xm_initaled && H264_DVR_Init(xm_disconnect_callback, (unsigned long)&devicelist))
	{
		H264_DVR_SetDVRMessCallBack(xm_mess_callback, (unsigned long)&devicelist);

		H264_DVR_SetSubDisconnectCallBack(xm_sub_disconnect_callBack, (unsigned long)&devicelist);
		jtprintf("[%s]init success %d\n", __FUNCTION__, g_xm_initaled);
		++g_xm_initaled;
	}

	return SUCCESS;
}

static int xm_init(struct device *dev)
{
	jtprintf("[%s]\n", __FUNCTION__);
	/*if(!g_xm_initaled && H264_DVR_Init(xm_disconnect_callback, (unsigned long)&devicelist))
	{
		H264_DVR_SetDVRMessCallBack(xm_mess_callback, (unsigned long)&devicelist);

		H264_DVR_SetSubDisconnectCallBack(xm_sub_disconnect_callBack, (unsigned long)&devicelist);
		jtprintf("[%s]init success %d\n", __FUNCTION__, g_xm_initaled);
		++g_xm_initaled;
	}*/

	return SUCCESS;
}

static int xm_uninit(struct device *dev)
{
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

			xmstream->currentencode = VIDEO_ENCODE_UNKNOWN;
			xmstream->playhandle = XM_INVALIDE_PLAYHANDLE;

			stream_init(&xmstream->stm);
		}

		channel_init(&xmchannel->chn);
	}

	memset(&dev->info, 0, sizeof(dev->info));//清除设备信息
	dev->voicehandle = 0;
	dev->loginid = XM_INVALIDE_LOGINID;

	device_init(&dev->dev);

	return 0;
}

static int xm_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	jtprintf("[%s]%s, %u, %s, %s\n", __FUNCTION__, req->Ip, req->Port, req->User, req->Password);

	///if(get_device(dev)==NULL) add_device(dev);

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
	
	xmdev->loginid = H264_DVR_Login((char*)dev->ip, dev->port,(char*)dev->user, (char*)dev->password,(LPH264_DVR_DEVICEINFO)(&xmdev->info),&err);		
	if(xmdev->loginid != 0)
	{
		jtprintf("[%s]xmdev login success, %s, %d, %s, %s, %ld, err %d\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, xmdev->loginid, err);

		return SUCCESS;
	}
	else
	{
		jtprintf("[%s]xmdev login failed, %s, %d, %s, %s, error %d, loginid %d, err %d\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, err, xmdev->loginid, err);
		return LOGIN_FAILED;
	}

	return LOGIN_FAILED;
}
static int xm_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	
	xmdevice *xmdev = (xmdevice *)dev;
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct channel* channel;
	LIST_FOR_EACH_ENTRY(channel, &xmdev->dev.channels, struct channel, entry)
	{
		struct xmchannel* xmchannel = (struct xmchannel*)channel;
		assert(xmchannel->chn.obj.type == OBJECT_TYPE_CHANNEL);
		struct stream* stream;
		LIST_FOR_EACH_ENTRY(stream, &xmchannel->chn.streams, struct stream, entry)
		{
			struct xmstream* xmstream = (struct xmstream*)stream;
			assert(xmstream->stm.obj.type == OBJECT_TYPE_STREAM);

			if(xmstream->playhandle != XM_INVALIDE_PLAYHANDLE)
			{
				struct stCloseVideoStream_Req req;
				struct stCloseVideoStream_Rsp rsp;
				req.StreamHandle = (long)xmstream;
				xm_close_video_stream(dev, &req, &rsp);//关闭流
			}
		}

		if(xmchannel->chn.audiocallback)
		{
			struct stCloseAudioStream_Req req;
			struct stCloseAudioStream_Rsp rsp;
		
			req.Channel = xmchannel->chn.id;
			xm_close_audio_stream(dev, &req, &rsp);//关闭音频
		}

		channel_init(&xmchannel->chn);
	}

	/*if(xmdev->alarmhandle)
	{
		NET_DVR_CloseAlarmChan_V30(xmdev->alarmhandle);
		xmdev->alarmhandle = HK_INVALIDE_HANDLE;
	}*/

	//改为hk_close_video_stream??????
	/*if(xmchannel->voicehandle != XM_INVALIDE_PLAYHANDLE)
	{
		NET_DVR_StopVoiceCom(xmchannel->voicehandle);
		xmchannel->voicehandle = XM_INVALIDE_PLAYHANDLE;
	}*/

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	if(H264_DVR_Logout(xmdev->loginid))
	{
		jtprintf("[%s]xmdev xm_logout success\n", __FUNCTION__);
		xm_device_init(xmdev);
		return SUCCESS;		
	}
	else
	{
		jtprintf("[%s]xmdev login failed\n", __FUNCTION__);
		xm_device_init(xmdev);
		return LOGOUT_FAILED;
	}

	return -2;
}
static int xm_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp)
{
	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);
	
	assert(dev!=NULL);
	assert(req->Channel < MAX_CHANNEL_ENCODE_INFO);
	assert(req->Codec < 10);
	
	xmdevice *xmdev = (xmdevice *)dev;
	
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
		stm = (struct xmstream*)alloc_stream(sizeof(struct xmstream), req->Codec);
		if(stm)
		{
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
	jtprintf("[%s]H264_DVR_GetLastError %ld\n", __FUNCTION__, H264_DVR_GetLastError());
	if(handle == 0)
	{
		jtprintf("[%s]start real stream wrong! m_iPlayhandle %ld, channel %d, nStream %d\n"
			, __FUNCTION__, handle, req->Channel, req->Codec);
		return OPEN_VIDEO_STREAM_FAILED;
	}
	else
	{
		jtprintf("[%s]start real stream ok, playhandle %d\n", __FUNCTION__, handle);
		//方便用户修改上一次的 userdata数据
		if(stm->stm.callback)
			stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_OPENED, NULL, &stm->stm.userdata);
		
		stm->playhandle = handle;
		stm->stm.pulling = 1;
		stm->stm.llbegintime = 0;
		stm->currentencode = VIDEO_ENCODE_VIDEO_H264;//先默认了
		stm->stm.callback = (jt_stream_callback)req->Callback;
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
	
	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	xmdevice *xmdev = (xmdevice *)dev;
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct xmstream* stm = (struct xmstream*)get_stream_by_dev(dev, (struct stream*)req->StreamHandle);
	if(stm==NULL)
	{
		jtprintf("[%s]stm null\n", __FUNCTION__);
		return INVALID_STREAM_NO_FAILED;
	}

	if(stm->playhandle==XM_INVALIDE_PLAYHANDLE)
	{
		jtprintf("[%s]stm->playhandle==0, aready closed\n", __FUNCTION__);
		stm->stm.pulling = 0;
		stm->playhandle = XM_INVALIDE_PLAYHANDLE;
		stm->stm.callback = NULL;
		stm->stm.llbegintime = 0;
		return SUCCESS;
	}

	if(H264_DVR_SetRealDataCallBack_V2(stm->playhandle, NULL, 0)==0)
	{
		jtprintf("[%s]set video callback failed!\n", __FUNCTION__);
		return SET_VIDEO_CALLBACK_FAILED;
	}

	if(H264_DVR_StopRealPlay(stm->playhandle))
	{
		if(stm->stm.callback)
			stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_CLOSEED, SUCCESS, &stm->stm.userdata);
		
		jtprintf("[%s]H264_DVR_StopRealPlay ok!\n", __FUNCTION__);
		stm->stm.pulling = 0;
		stm->playhandle = XM_INVALIDE_PLAYHANDLE;
		stm->stm.callback = NULL;
		stm->stm.llbegintime = 0;
	}
	else
	{
		if(stm->stm.callback)
			stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_CLOSEED, (void*)OPEN_AUDIO_STREAM_FAILED, &stm->stm.userdata);
	
		jtprintf("[%s]H264_DVR_StopRealPlay error\n", __FUNCTION__);
		return CLOSE_VIDEO_STREAM_FAILED;
	}
	
	return SUCCESS;
}
static int xm_operator_channel(struct channel *chn, int type, void* data)
{
	if(STOP_AUDIO==type)//关闭音频
	{
		jtprintf("[%s]STOP_AUDIO \n", __FUNCTION__);

		if(chn->audiocallback)
			chn->audiocallback(CALLBACK_TYPE_AUDIO_STREAM_CLOSEED, SUCCESS, &chn->audiouserdata);
		
		chn->audiocallback = NULL;
	}
	else if(START_AUDIO==type)//打开音频
	{
		jtprintf("[%s]START_AUDIO \n", __FUNCTION__);
		struct stOpenAudioStream_Req *req = (struct stOpenAudioStream_Req *)data;

		void* tmp = chn->audiouserdata;//记录之前的用户数据

		chn->audiocallback = (jt_stream_callback)req->Callback;
		chn->audiouserdata = req->UserData;
		
		if(chn->audiocallback)
			chn->audiocallback(CALLBACK_TYPE_AUDIO_STREAM_OPENED, SUCCESS, &tmp);//对之前的用户数据进行处理
	}
	else if(CHECK_AUDIO_CHANNEL==type)//
	{
		jtprintf("[%s]CHECK_AUDIO_CHANNEL \n", __FUNCTION__);
		
		//熊迈的音频是对应到设备的，这里要看看那些通道要了音频数据
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
	
	xmdevice *xmdev = (xmdevice *)get_device(dev);
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NO_FOUND;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, xmdev->dev.ip, xmdev->dev.port);

	struct xmchannel* chn = NULL;
	chn = (struct xmchannel*)get_channel(&(xmdev->dev.channels), req->Channel);
	if(chn == NULL)
	{
		return INVALID_CHANNEL_NO_FAILED;
	}

	//jtprintf("[%s]1 xmdev->voicehandle %ld, xmdev->loginid %ld\n", __FUNCTION__, xmdev->voicehandle, xmdev->loginid);
	/*if(xmdev->voicehandle == 0)
	{
		long voicehandle = H264_DVR_StartVoiceCom_MR(xmdev->loginid, xm_audio_data_callback, (long)dev);
		if(voicehandle <= 0)
		{
			jtprintf("[%s]H264_DVR_StartVoiceCom_MR voicehandle %ld, failed\n", __FUNCTION__, voicehandle);
			//return OPEN_AUDIO_STREAM_FAILED;
		}

		jtprintf("[%s]H264_DVR_StartVoiceCom_MR voicehandle %ld, ok\n", __FUNCTION__, voicehandle);
		xmdev->voicehandle = voicehandle;
	}*/

	rsp->ChannelHandle = (long)do_channel(&dev->channels, req->Channel, xm_operator_channel, START_AUDIO, req);

	return SUCCESS;
}
static int xm_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	
	xmdevice *xmdev = (xmdevice *)get_device(dev);
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NO_FOUND;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, xmdev->dev.ip, xmdev->dev.port);

	struct xmchannel* chn = NULL;
	chn = (struct xmchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		jtprintf("[%s]no channel %d\n", __FUNCTION__, req->Channel);
		return INVALID_CHANNEL_NO_FAILED;
	}

	do_channel(&dev->channels, req->Channel, xm_operator_channel, STOP_AUDIO, NULL);

	/*int havechannelcnt=0;
	do_each_channel(&dev->channels, xm_operator_channel, CHECK_AUDIO_CHANNEL, &havechannelcnt);
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
	}*/

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

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	xmdevice *xmdev = (xmdevice *)dev;
	switch(req->Type)
	{
		case GET_ENCODE_CONFIG://获取编码配置
		{
			jtprintf("[%s]GET_ENCODE_CONFIG, chn %d, codec %d\n"
				, __FUNCTION__, req->Channel, req->Codec);
			long curtime = time(0);
			if(curtime - xmdev->dev.encodeinfo.last_get_time > 120)
			{
				//大于120秒，重新获取
				jtprintf("[%s]re get encode info\n", __FUNCTION__);
				struct SDK_EncodeConfigAll_SIMPLIIFY EncodeConfig;
				unsigned long dwRetLen = 0;
				int nWaitTime = 10000;
				xmdev->dev.encodeinfo.last_get_time = curtime;

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
			else
			{
				jtprintf("[%s]curtime(%d) - xmdev->dev.encodeinfo.last_get_time(%d) = %d, use old config\n"
					, __FUNCTION__, curtime, xmdev->dev.encodeinfo.last_get_time, curtime-xmdev->dev.encodeinfo.last_get_time);			
			}

			struct encode_info* einfo = (struct encode_info*)malloc(sizeof(struct encode_info));
			if(req->Codec==0)
			{
				memcpy(einfo, &xmdev->dev.encodeinfo.ch_encode_info[req->Channel].mainencode, sizeof(struct encode_info));
			}
			else
			{
				memcpy(einfo, &xmdev->dev.encodeinfo.ch_encode_info[req->Channel].sub1encode, sizeof(struct encode_info));
			}

			printf_device_encode_info(dev);

			rsp->Config = (char*)einfo;
			rsp->Size = 1;
			
			break;
		}
	}

	return SUCCESS;
}
static int xm_set_config(struct device *, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp)
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

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	xmdevice *xmdev = (xmdevice *)dev;
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(H264_DVR_SetupAlarmChan(xmdev->loginid))
	{
		jtprintf("[%s]xmdev H264_DVR_SetupAlarmChan ok, alarmcallback %p\n", __FUNCTION__, dev->alarmcallback);
		if(dev->alarmcallback)
			dev->alarmcallback(CALLBACK_TYPE_ALARM_STREAM_OPENED, (void*)SUCCESS, &dev->alarmuserdata);

		//jtprintf("[%s]xmdev H264_DVR_SetupAlarmChan ok  22, alarmcallback %p\n"
		//	, __FUNCTION__, dev->alarmcallback);

		dev->alarmcallback = req->Callback;
		dev->alarmuserdata = req->UserData;
		rsp->DeviceHandle = (long long)(unsigned)xmdev;
	}
	else
	{		
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

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);
	
	xmdevice *xmdev = (xmdevice *)dev;

	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(dev->alarmcallback)
		dev->alarmcallback(CALLBACK_TYPE_ALARM_STREAM_CLOSEED, (void*)OPEN_AUDIO_STREAM_FAILED, &dev->alarmuserdata);
	dev->alarmcallback = NULL;

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

static PTZ_ControlType xm_ptz_direction_convert(int jtdirection)
{
	switch(jtdirection)
	{
		case JPTZ_UP:
			return TILT_UP;
		case JPTZ_RIGHT_UP:
			return PAN_RIGTHTOP;
		case JPTZ_RIGHT:
			return PAN_RIGHT;
		case JPTZ_RIGHT_DOWN:
			return PAN_RIGTHDOWN;
		case JPTZ_DOWN:
			return TILT_DOWN;
		case JPTZ_LEFT_DOWN:
			return PAN_LEFTDOWN;
		case JPTZ_LEFT:
			return PAN_LEFT;
		case JPTZ_LEFT_UP:
			return PAN_LEFTTOP;
	}

	return TILT_UP;
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

	jtprintf("[%s]ip %s, port %u, Channel %d, Action %u, Speed %d\n"
		, __FUNCTION__, dev->ip, dev->port, req->Channel, req->Action, req->Speed);

	xmdevice *xmdev = (xmdevice *)dev;

	switch(req->Action)
	{
		case JPTZ_STOP://停止
		
			jtprintf("[%s]JPTZ_STOP ip %s, port %u, Channel %d, Action %u, Speed %d\n"
			, __FUNCTION__, dev->ip, dev->port, req->Channel, req->Action, req->Speed);
			
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, 0, 1, 0);
		break;
		case JPTZ_UP://8个方向
		case JPTZ_RIGHT_UP:
		case JPTZ_RIGHT:
		case JPTZ_RIGHT_DOWN:
		case JPTZ_DOWN:
		case JPTZ_LEFT_DOWN:
		case JPTZ_LEFT:
		case JPTZ_LEFT_UP:
			jtprintf("[%s]FANGXIANG JPTZ_STOP ip %s, port %u, Channel %d, Action %u, Speed %d\n"
			, __FUNCTION__, dev->ip, dev->port, req->Channel, req->Action, req->Speed);
			if(!H264_DVR_PTZControl(xmdev->loginid, req->Channel, xm_ptz_direction_convert(req->Action), 0, req->Speed))
			{

			}	
		break;
		case JPTZ_PUSH_FAR://拉远
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, ZOOM_OUT, 0, req->Speed);
		break;
		case JPTZ_PULL_NEAR://推近
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, ZOOM_IN, 0, req->Speed);
		break;
		case JPTZ_IRIS_ADD://光圈加
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, IRIS_OPEN, 0, req->Speed);
		break;
		case JPTZ_IRIS_SUB://光圈减
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, IRIS_CLOSE, 0, req->Speed);
		break;
		case JPTZ_FOCUS_FAR://焦距++
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, FOCUS_FAR, 0, req->Speed);
		break;
		case JPTZ_FOCUS_NEAR://焦距--
			H264_DVR_PTZControl(xmdev->loginid, req->Channel, FOCUS_NEAR, 0, req->Speed);
		break;
		case JSET_PRESET://设置预置点
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_POINT_SET_CONTROL, req->PresetNum, 2, 3, 0);
		break;
		case JCLEAR_PRESET://清除预置点
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_POINT_DEL_CONTROL, req->PresetNum, 2, 3, 0);
		break;
		case JGOTO_PRESET://转到预置点
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_POINT_MOVE_CONTROL, req->PresetNum, 2, 3, 0);
		break;
		case JADD_TO_LOOP://添加预置点到巡航
			jtprintf("[%s]dev %s, %u, JADD_TO_LOOP\n", __FUNCTION__, dev->ip, dev->port);
		
			if(!H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_CLOSELOOP, req->TourNum, 2, 3, 0))
			{
				jtprintf("[%s]dev %s, %u, JADD_TO_LOOP, failed 1\n", __FUNCTION__, dev->ip, dev->port);
			}

			for(int i=0 ; i<req->SequenceNum ; ++i)
			{
				H264_DVR_PTZControlEx(xmdev->loginid, req->Channel
					, EXTPTZ_ADDTOLOOP, req->TourNum, req->SequenceGroup[i], 3, 0);

			}

		break;
		case JDEL_FROM_LOOP://将预置点从巡航删除
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_DELFROMLOOP, req->TourNum, req->PresetNum, 3, 0);
		break;
		case JSTART_LOOP://开始巡航
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_POINT_LOOP_CONTROL, req->TourNum, 2, 3, 0);
		break;
		case JSTOP_LOOP://停止巡航
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_POINT_STOP_LOOP_CONTROL, req->TourNum, 2, 3, 0);
		break;
		case JREMOVE_LOOP://删除巡航
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_CLOSELOOP, req->TourNum, 2, 3, 0);
		break;
		case JPTZ_RESET://云台复位
			H264_DVR_PTZControlEx(xmdev->loginid, req->Channel, EXTPTZ_OPERATION_RESET, req->PresetNum, 2, 3, 0);
		break;
		default:
			jtprintf("[%s]dev %p, PTZ, unkonwn cmd %d\n", __FUNCTION__, dev, req->Action);
		return UNKONWN_PTZ_COMMAND;
	}

	return 0;
}
static int xm_set_system_time(struct device *dev, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp)
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

	xmdevice *xmdev = (xmdevice *)dev;	

	SDK_SYSTEM_TIME time;
	time.month = req->month;
	time.day = req->day;
	time.hour = req->hour;
	time.isdst = 1;
	time.minute = req->minute;
	time.second = req->second;
	time.wday = 1;//may be calc is better
	time.year = req->year;
	if(H264_DVR_SetSystemDateTime (xmdev->loginid, &time))
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
static int xm_start_talk(struct device* dev, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp)
{	
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	
	xmdevice *xmdev = (xmdevice *)get_device(dev);
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NO_FOUND;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, xmdev->dev.ip, xmdev->dev.port);

	struct xmchannel* chn = NULL;
	chn = (struct xmchannel*)get_channel(&(xmdev->dev.channels), req->Channel);
	if(chn == NULL)
	{
		return INVALID_CHANNEL_NO_FAILED;
	}

	jtprintf("[%s]1 xmdev->voicehandle %ld, xmdev->loginid %ld\n", __FUNCTION__, xmdev->voicehandle, xmdev->loginid);

	if(xmdev->voicehandle == 0)
	{
		//H264_DVR_SetTalkMode (long lLoginID, SDK_AudioInFormatConfig* pTalkMode);

		long voicehandle = H264_DVR_StartVoiceCom_MR(xmdev->loginid, xm_audio_data_callback, (long)dev);
		if(voicehandle <= 0)
		{
			jtprintf("[%s]H264_DVR_StartVoiceCom_MR voicehandle %ld, failed\n", __FUNCTION__, voicehandle);
			return START_TALK_FAILED;
		}

		jtprintf("[%s]H264_DVR_StartVoiceCom_MR voicehandle %ld, ok\n", __FUNCTION__, voicehandle);
		xmdev->voicehandle = voicehandle;

		xmdev->dev.talkcallback = req->TalkCallback;
		
		pthread_t tid;
		if(create_detached_thread(&tid, xm_send_talk_data_thread, xmdev))
		{
			
			
		}

		xmdev->dev.talkthreadid = tid;
	}

	rsp->DeviceHandle = (long long) xmdev;
	rsp->Channel = (long)chn;//(long)do_channel(&dev->channels, req->Channel, xm_operator_channel, START_AUDIO, req);

	return SUCCESS;
}
static int xm_stop_talk(struct device * dev, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);

	xmdevice *xmdev = (xmdevice *)get_device(dev);
	if(xmdev==NULL)
	{
		jtprintf("[%s]xmdev null\n", __FUNCTION__);
		return DEVICE_NO_FOUND;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, xmdev->dev.ip, xmdev->dev.port);

	//struct xmchannel* chn = NULL;
	//chn = (struct xmchannel*)get_channel(&dev->channels, req->Channel);
	//if(chn == NULL)
	//{
	//	return INVALID_CHANNEL_NO;
	//}
	
	if(xmdev->voicehandle)
	{
		if(H264_DVR_StopVoiceCom(xmdev->voicehandle)==0)
		{
			jtprintf("[%s]H264_DVR_StopVoiceCom wrong!!\n", __FUNCTION__);

			//return STOP_TALK_FAILED;///??????
		}

		xmdev->voicehandle = 0;
	}

	return SUCCESS;
}
static int xm_send_talk_data(struct device *dev, struct stSendTalkData_Req *req, struct stSendTalkData_Rsp *rsp)
{
	xmdevice *xmdev = (xmdevice *)dev;
	if(0==H264_DVR_VoiceComSendData (xmdev->voicehandle, (char*)req->Data, (long)req->DataLen))
	{
		jtprintf("[%s]H264_DVR_VoiceComSendData wrong!!\n", __FUNCTION__);
		return SEND_TALK_DATA_FAILED;
	}
	
	return SUCCESS;	
}

