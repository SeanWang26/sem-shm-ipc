#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "hkmanager.h"
#include "../devicetype.h"
#include "../jtprintf.h"
#include "../seansinglebuf.h"
#include "../seanh264utility.h"

static int g_hk_initaled = 0;

static int hk_device_init(struct hkdevice *dev);

static int hk_init(struct device *dev);
static int hk_uninit(struct device *dev);
static int hk_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
static int hk_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
static int hk_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
static int hk_close_video_stream(struct device *dev, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
static int hk_open_audio_stream(struct device *dev, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
static int hk_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
static int hk_get_config(struct device *dev, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
static int hk_set_config(struct device *, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
static int hk_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
static int hk_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
static int hk_ptz_control(struct device *, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
static int hk_set_system_time(struct device *, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);
static int hk_start_talk(struct device *, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp);
static int hk_stop_talk(struct device *, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp);
static int hk_send_talk_data(struct device *, struct stSendTalkData_Req *req, struct stSendTalkData_Rsp *rsp);
static int hk_get_video_effect(struct device *, stGetVideoEffect_Req *req, stGetVideoEffect_Rsp *rsp);
static int hk_set_video_effect(struct device *, stSetVideoEffect_Req *req, stSetVideoEffect_Rsp *rsp);

static struct device_ops hk_ops = 
{
	sizeof(struct hkdevice),
	DEVICE_HK,
    hk_init,
    hk_uninit,
    hk_login,
    hk_logout,
	hk_open_video_stream,
	hk_close_video_stream,
	hk_open_audio_stream,
	hk_close_audio_stream,
	hk_get_config,
	hk_set_config,
	hk_open_alarm_stream,
	hk_close_alarm_stream,
	hk_ptz_control,
	hk_set_system_time,
	hk_start_talk,
	hk_stop_talk,
	hk_send_talk_data,
	hk_get_video_effect,
	hk_set_video_effect
};

void hk_disconnect_callback(long lLoginID, char *pchDVRIP, long nDVRPort, unsigned long dwUser)
{
	FIND_DEVICE_BEGIN(struct hkdevice,DEVICE_HK)
	{
		if(dev->loginid == lLoginID)
		{
			hk_device_init(dev);
			jtprintf("[%s]%p, %s, %lu\n", __FUNCTION__, (void*)dwUser, pchDVRIP, nDVRPort);
			break;
		}
	}
	FIND_DEVICE_END
}

/*void hk_sub_disconnect_callBack(long lLoginID, SubConnType type, long nChannel, long dwUser)
{
	FIND_DEVICE_BEGIN(struct hkdevice,DEVICE_HK)
	{
		if(dev->loginid == lLoginID)
		{
			jtprintf("[%s]%p, %s, %lu\n", __FUNCTION__, (void*)dwUser, type, nChannel);
			break;
		}
	}
	FIND_DEVICE_END
}*/

/*static int hk_pack_type_convert(enum MEDIA_PACK_TYPE type)
{
	if(VIDEO_P_FRAME==type)
		return P_FRAME;
	else if(VIDEO_I_FRAME==type) 
		return I_FRAME;
	else if(VIDEO_B_FRAME==type || VIDEO_BP_FRAME==type ||VIDEO_BBP_FRAME==type)
		return B_FRAME;
	
	return UNKNOWN_FRAME;
}*/
	
static int dosaveraw( unsigned char* data, DWORD len)
{
	static FILE* fp=fopen("./raw.bin", "wb+");;
	if(fp==NULL)
		return -1;

	return fwrite(data,len,1,fp);
}

#pragma pack(1)
struct hk_ps_pack_start_code  
{
    unsigned char start_code[3];  
    unsigned char stream_id[1];  
};

union hk_ps_littel_endian_size  
{  
    unsigned short int  length;  
    unsigned char       byte[2];  
};

struct hk_ps_program_stream_e  
{  
    hk_ps_pack_start_code     PackStart;  
    hk_ps_littel_endian_size  PackLength;//we mast do exchange  
    char                PackInfo1[2];  
    unsigned char       stuffing_length;  
};
#pragma pack()

//只适用于流表示为0xe?的部分
inline unsigned int move_to_video_payload_data(BYTE *indata, unsigned int indatalen, BYTE **outdata, unsigned int *outdatalen)
{
    //printf("[%s]%x %x %x %x\n", __FUNCTION__, Pack[0], Pack[1], Pack[2], Pack[3]);  
    hk_ps_program_stream_e* PSEPack = (hk_ps_program_stream_e*)indata;  

    hk_ps_littel_endian_size pse_length;  
    pse_length.byte[0] = PSEPack->PackLength.byte[1];  
    pse_length.byte[1] = PSEPack->PackLength.byte[0];

	//
    *outdatalen = pse_length.length - 2 - 1 - PSEPack->stuffing_length;  
    if(*outdatalen>0)   
        *outdata = (BYTE *)PSEPack + sizeof(hk_ps_program_stream_e) + PSEPack->stuffing_length;  
          
    //*leftlength = length - pse_length.length - sizeof(pack_start_code) - sizeof(littel_endian_size);  
 
    return *outdatalen;  
}

#if 1
static void CALLBACK hk_real_data_callback_v2(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{
	struct hkstream* stream = (struct hkstream*)pUser;

	if(!stream->stm.pulling)
	{	
		static struct stream* _stream = 0;
		if(_stream != (struct stream*)stream)
			jtprintf("[%s]1 stream->stm.pulling %d, stream %p !!!!!!!!\n"
				, __FUNCTION__, _stream->pulling, _stream);
		_stream = (struct stream*)stream;
		return ;
	}

	
	/*jtprintf("[%s]1type %d, size %u, %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n"
		, __FUNCTION__, dwDataType, dwBufSize
		, pBuffer[0], pBuffer[1], pBuffer[2], pBuffer[3] ,pBuffer[4]
		, pBuffer[5], pBuffer[6], pBuffer[7], pBuffer[8] ,pBuffer[9]
		, pBuffer[10], pBuffer[11], pBuffer[12], pBuffer[13] , pBuffer[14]
		, pBuffer[15], pBuffer[16], pBuffer[17], pBuffer[18] , pBuffer[19]);
	*/

	if(dwDataType == NET_DVR_STREAMDATA)
	{	
		//海康默认返回最大0x8000个数据,多的分到多个pes包
		struct hk_ps_pack_start_code *startcode = (struct hk_ps_pack_start_code *)pBuffer;
		if(startcode->stream_id[0] == 0xba)
		{
			//jtprintf("[%s]new packet\n", __FUNCTION__);
			//dosaveraw(pBuffer, dwBufSize);

			unsigned char* vdata = NULL;
			unsigned int vdatalen = 0;
			if(read_from_singlebuf(&stream->stm.videobuf, &vdata, &vdatalen))
			{
				//jtprintf("[%s]read_from_singlebuf data %p, len %d\n"
				//	, __FUNCTION__, vdata, vdatalen);

				/*jtprintf("[%s]2type %d, size %u, %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n"
					, __FUNCTION__, dwDataType, vdatalen
					, vdata[0], vdata[1], vdata[2], vdata[3] ,vdata[4]
					, vdata[5], vdata[6], vdata[7], vdata[8] ,vdata[9]
					, vdata[10], vdata[11], vdata[12], vdata[13] , vdata[14]
					, vdata[15], vdata[16], vdata[17], vdata[18] , vdata[19]);
                */
				struct channel* chn = (struct channel*)stream->stm.obj.parent;
				struct device* dev = (struct device*)chn->obj.parent;
		
				//dosaveraw(vdata, vdatalen);
				st_stream_data stmdata;
				stmdata.streamtype = VIDEO_STREAM_DATA;
				stmdata.pdata= (char*)vdata;
				stmdata.datalen = (int)vdatalen;

				//只要是三者之一就判断为i帧
				if(ispictureseq(vdata[4]) || isframeseq(vdata[4]) || isrefrencefream(vdata[4]))
				{
					//jtprintf("[%s]i frame\n", __FUNCTION__);
					stmdata.stream_info.video_stream_info.frametype = I_FRAME;
				}
				else 
				{
					//jtprintf("[%s]no frame\n", __FUNCTION__);
					stmdata.stream_info.video_stream_info.frametype = P_FRAME;
				}				
				
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

				stmdata.year = 2014;
				stmdata.month = 1;
				stmdata.day = 1;
				stmdata.hour = 1;
				stmdata.minute = 1;
				stmdata.second = 1;

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
			}

			clear_singlebuf(&stream->stm.videobuf);

			return;
		}
		else if((startcode->stream_id[0] & 0xf0) == 0xe0)
		{
			//jtprintf("[%s]video pes\n", __FUNCTION__);

			unsigned char* vdata = NULL;
			unsigned int vdatalen = 0;	
			if(move_to_video_payload_data(pBuffer, dwBufSize, &vdata, &vdatalen))
			{
				write_to_singlebuf(&stream->stm.videobuf, vdata, vdatalen);
			}

			return;
		}
		else if((startcode->stream_id[0] & 0xf0) == 0xc0)
		{
			int printed = 0;
			if(!printed)
			{
				jtprintf("[%s]AUDIO_PACKET pes %d\n", __FUNCTION__, dwBufSize);
				printed=1;
			}
			return;
		}
		else
		{
			/*if(startcode->stream_id[0] == 0xbd)
			{
				dosaveraw(pBuffer, dwBufSize);
			}*/
			return;
		}

		return;
	}
	else if(dwDataType == NET_DVR_AUDIOSTREAMDATA)
	{
		jtprintf("[%s]AUDIO_PACKET %d\n", __FUNCTION__, stream->stm.id);

		//jtprintf("[%s]AUDIO_PACKET %d\n", __FUNCTION__, stream->stm.id);
		struct channel* chn = (struct channel*)stream->stm.obj.parent;
		struct device* dev = (struct device*)chn->obj.parent;

		if(chn->audiocallback)// && chn->audiouserdata)
		{
			st_stream_data stmdata;
			stmdata.streamtype = AUDIO_STREAM_DATA;
			stmdata.pdata= (char*)pBuffer;
			stmdata.datalen = dwBufSize;
			stmdata.stream_info.audio_stream_info.encode = dev->encodeinfo.ch_encode_info[chn->id].audioencode.encodetype;
			stmdata.stream_info.audio_stream_info.channel = dev->encodeinfo.ch_encode_info[chn->id].audioencode.channel;
			stmdata.stream_info.audio_stream_info.frequency = dev->encodeinfo.ch_encode_info[chn->id].audioencode.frequency;
			stmdata.stream_info.audio_stream_info.depth = dev->encodeinfo.ch_encode_info[chn->id].audioencode.depth;
			stmdata.stream_info.audio_stream_info.bitrate = dev->encodeinfo.ch_encode_info[chn->id].audioencode.bitrate;
			stmdata.year = 2014;
			stmdata.month = 1;
			stmdata.day = 1;
			stmdata.hour = 1;
			stmdata.minute = 1;
			stmdata.second = 1;

			chn->audiocallback(CALLBACK_TYPE_AUDIO_STREAM, &stmdata, &chn->audiouserdata);
		}

		return;
	}
	else if(dwDataType == NET_DVR_SYSHEAD)
	{
		jtprintf("[%s]NET_DVR_SYSHEAD stm id %d\n", __FUNCTION__, stream->stm.id);
		return;
	}
	else if(NET_DVR_PRIVATE_DATA == dwDataType)
	{
		jtprintf("[%s]NET_DVR_PRIVATE_DATA stm id %d\n", __FUNCTION__, stream->stm.id);
		return;
	}

	return;
}
#endif

#if 0
static void CALLBACK hk_standard_real_data_callback_v2(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, DWORD pUser)
{
	//创建一个线程局部变量,标识pulling,   todo.............
	struct hkstream* stream = (struct hkstream*)pUser;
	
	/*jtprintf("[%s]type %d, size %u, %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n"
		, __FUNCTION__, dwDataType, dwBufSize
		, pBuffer[0], pBuffer[1], pBuffer[2], pBuffer[3] ,pBuffer[4]
		, pBuffer[5], pBuffer[6], pBuffer[7], pBuffer[8] ,pBuffer[9]
		, pBuffer[10], pBuffer[11], pBuffer[12], pBuffer[13] , pBuffer[14]
		, pBuffer[15], pBuffer[16], pBuffer[17], pBuffer[18] , pBuffer[19]);
	*/
	if(dwDataType == NET_DVR_STREAMDATA)
	{
		struct hk_ps_pack_start_code *startcode = (struct hk_ps_pack_start_code *)pBuffer;
		if(startcode->stream_id[0] == 0xba)
		{
			//jtprintf("[%s]new packet\n", __FUNCTION__);

			unsigned char* vdata = NULL;
			unsigned int vdatalen = 0;
			if(read_from_singlebuf(&stream->stm.videobuf, &vdata, &vdatalen))
			{
				//jtprintf("[%s]read_from_singlebuf data %p, len %d\n"
				//	, __FUNCTION__, vdata, vdatalen);

				//dosaveraw(vdata, vdatalen);
			}

			clear_singlebuf(&stream->stm.videobuf);
		}
		else if((startcode->stream_id[0] & 0xf0) == 0xe0)
		{
			//jtprintf("[%s]video pes\n", __FUNCTION__);

			unsigned char* vdata = NULL;
			unsigned int vdatalen = 0;	

			if(move_to_video_payload_data(pBuffer, dwBufSize, &vdata, &vdatalen))
			{
				write_to_singlebuf(&stream->stm.videobuf, vdata, vdatalen);
			}
		}

		st_stream_data stmdata;
		stmdata.streamtype = VIDEO_STREAM_DATA;
		stmdata.pdata= (char*)pBuffer;
		stmdata.datalen = dwBufSize;
		stmdata.stream_info.video_stream_info.encode = stream->currentencode;
		//stmdata.stream_info.video_stream_info.frametype = hk_pack_type_convert((enum MEDIA_PACK_TYPE)pFrame->nPacketType);
		//stmdata.stream_info.video_stream_info.width = pFrame->dwPacketSize;
		//stmdata.stream_info.video_stream_info.height = pFrame->dwPacketSize;
		stmdata.stream_info.video_stream_info.fps = 0;
		stmdata.stream_info.video_stream_info.bitrate = 0;
		stmdata.year = 2014;
		stmdata.month = 1;
		stmdata.day = 1;
		stmdata.hour = 1;
		stmdata.minute = 1;
		stmdata.second = 1;

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

		return;
	}
	else if(dwDataType == NET_DVR_AUDIOSTREAMDATA)
	{
		jtprintf("[%s]AUDIO_PACKET %d\n", __FUNCTION__, stream->stm.id);

		//jtprintf("[%s]AUDIO_PACKET %d\n", __FUNCTION__, stream->stm.id);
		struct channel* chn = (struct channel*)stream->stm.obj.parent;
		struct device* dev = (struct device*)chn->obj.parent;

		if(chn->audiocallback)// && chn->audiouserdata)
		{
			st_stream_data stmdata;
			stmdata.streamtype = AUDIO_STREAM_DATA;
			stmdata.pdata= (char*)pBuffer;
			stmdata.datalen = dwBufSize;
			stmdata.stream_info.audio_stream_info.encode = dev->encodeinfo.ch_encode_info[chn->id].audioencode.encodetype;
			stmdata.stream_info.audio_stream_info.channel = dev->encodeinfo.ch_encode_info[chn->id].audioencode.channel;
			stmdata.stream_info.audio_stream_info.frequency = dev->encodeinfo.ch_encode_info[chn->id].audioencode.frequency;
			stmdata.stream_info.audio_stream_info.depth = dev->encodeinfo.ch_encode_info[chn->id].audioencode.depth;
			stmdata.stream_info.audio_stream_info.bitrate = dev->encodeinfo.ch_encode_info[chn->id].audioencode.bitrate;
			stmdata.year = 2014;
			stmdata.month = 1;
			stmdata.day = 1;
			stmdata.hour = 1;
			stmdata.minute = 1;
			stmdata.second = 1;

			chn->audiocallback(CALLBACK_TYPE_AUDIO_STREAM, &stmdata, &chn->audiouserdata);
		}

		return;
	}
	else if(dwDataType == NET_DVR_SYSHEAD)
	{
		jtprintf("[%s]NET_DVR_SYSHEAD stm id %d\n", __FUNCTION__, stream->stm.id);
		return;
	}
	else if(NET_DVR_PRIVATE_DATA == dwDataType)
	{
		jtprintf("[%s]NET_DVR_PRIVATE_DATA stm id %d\n", __FUNCTION__, stream->stm.id);
		return;
	}

	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
	return;
}
#endif

void hk_audio_data_callback(LONG lTalkHandle, char *pDataBuf, long dwBufSize, char byAudioFlag, long dwUser)
{
	//lock
	//struct hkdevice

	
	/*FIND_DEVICE_BEGIN(struct hkdevice,DEVICE_HK)
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
*/
	//BOOL bResult = TRUE;
}

static inline int hk_handle_alarm(hkdevice *device, char *pBuf, unsigned long dwBufLen)
{
	int reason = 0;

	/*SDK_AlarmInfo alarmInfo;
	memcpy (&alarmInfo, pBuf, dwBufLen); 	

	
	if ( SDK_EVENT_CODE_NET_ALARM == alarmInfo.iEvent 
		|| SDK_EVENT_CODE_MANUAL_ALARM == alarmInfo.iEvent 
		|| SDK_EVENT_CODE_LOCAL_ALARM == alarmInfo.iEvent )
	{
		jtprintf("[%s]SDK_EVENT_CODE_NET_ALARM SDK_EVENT_CODE_MANUAL_ALARM SDK_EVENT_CODE_LOCAL_ALARM\n", __FUNCTION__);
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
	
	if(reason==0)
		return 0;
*/	
	st_stream_data alarm;
	alarm.streamtype = ALARM_STREAM_DATA;
	alarm.stream_info.alarm_stream_info.reason = reason;
	alarm.stream_info.alarm_stream_info.channelid = 0;
	
	if(device->dev.alarmcallback)
		device->dev.alarmcallback(CALLBACK_TYPE_ALARM_STREAM, &alarm, &device->dev.alarmuserdata);

	return 0;
}

/*static bool hk_mess_callback(long lLoginID, char *pBuf, unsigned long dwBufLen, long dwUser)
{
	//lock
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		assert(device->obj.type == OBJECT_TYPE_DEVICE);
		if(!device->deleted && device->ops->type == DEVICE_HK )
		{	
			if(((hkdevice*)device)->loginid == lLoginID)
			{
				hk_handle_alarm((hkdevice*)device, pBuf, dwBufLen);
				break;
			}
		}
	}
	
	return 1;
}*/

static void CALLBACK hk_handle_exception(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
	switch(dwType)
	{
		case EXCEPTION_AUDIOEXCHANGE:		 //语音对讲时网络异常
			jtprintf("语音对讲时网络异常!!!");
		break;
		case EXCEPTION_ALARM:			 //报警上传时网络异常
			jtprintf("报警上传时网络异常!!!");
		break;
		case EXCEPTION_PREVIEW:		 //网络预览时异常
			jtprintf("网络预览时网络异常!!!");
		break;
		case EXCEPTION_SERIAL: 		 //透明通道传输时异常
			jtprintf("透明通道传输时网络异常!!!");
		break;
		case EXCEPTION_RECONNECT:			 //预览时重连
			jtprintf("预览时重连!!!");
		break;
		default:
		break;
	}

	
}

static void CALLBACK hk_mess_callback(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
	//lock
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		assert(device->obj.type == OBJECT_TYPE_DEVICE);
		if(!device->deleted && device->ops->type == DEVICE_HK )
		{	
			if(((hkdevice*)device)->loginid == lUserID)
			{
				hk_handle_exception(dwType, lUserID, lHandle, pUser);
				break;
			}
		}
	}
}

static int hk_alarm_type_convert(unsigned int type)
{
	jtprintf("[%s]hk type %u\n", __FUNCTION__, type);
	switch(type)
	{	
		case 0:
			return ALARM_TYPE_INPUT;
		case 2:
			return ALARM_TYPE_VIDEO_LOSS;
		case 3:
			return ALARM_TYPE_VIDEO_MOTION;
		case 6:
			return ALARM_TYPE_VIDEO_BLIND;
		default:
			break ;
	}

	jtprintf("[%s]ALARM_TYPE_UNKNOWN\n", __FUNCTION__);
	return ALARM_TYPE_UNKNOWN;
}

static void CALLBACK hk_alarm_callback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
	struct hkdevice* hkdev = (struct hkdevice*)pUser;
	//printf("lCommand is %d, alarm type is %d\n", lCommand, struAlarmInfo.dwAlarmType);
	switch(lCommand) 
	{
		case COMM_ALARM_V30:
		{
			jtprintf("[%s]COMM_ALARM above v3.0\n", __FUNCTION__);

			NET_DVR_ALARMINFO_V30 struAlarmInfo;
			memcpy(&struAlarmInfo, pAlarmInfo, sizeof(NET_DVR_ALARMINFO_V30));

			st_stream_data alarm;
			alarm.streamtype = ALARM_STREAM_DATA;
			alarm.stream_info.alarm_stream_info.reason = hk_alarm_type_convert(struAlarmInfo.dwAlarmType);
			if(alarm.stream_info.alarm_stream_info.reason != ALARM_TYPE_UNKNOWN)
			{
				for (int i=0; i<MAX_CHANNUM; i++)	//#define MAX_CHANNUM	16	//最大通道数
				{
					if (struAlarmInfo.byChannel[i] == 1)
					{
						if(hkdev->dev.alarmcallback)
						{
							alarm.stream_info.alarm_stream_info.channelid = i;
							
							if(hkdev->dev.alarmcallback)
								hkdev->dev.alarmcallback(CALLBACK_TYPE_ALARM_STREAM, &alarm, &hkdev->dev.alarmuserdata);		
						}
					}
				}
			}
		}
		break;
		case COMM_ALARM:
		{	
			jtprintf("[%s]COMM_ALARM below v3.0\n", __FUNCTION__);
			NET_DVR_ALARMINFO struAlarmInfo;
			memcpy(&struAlarmInfo, pAlarmInfo, sizeof(NET_DVR_ALARMINFO_V30));

			st_stream_data alarm;
			alarm.streamtype = ALARM_STREAM_DATA;
			alarm.stream_info.alarm_stream_info.reason = hk_alarm_type_convert(struAlarmInfo.dwAlarmType);
			if(alarm.stream_info.alarm_stream_info.reason != ALARM_TYPE_UNKNOWN)
			{
				for (int i=0; i<MAX_CHANNUM; i++)	//#define MAX_CHANNUM	16	//最大通道数
				{
					if (struAlarmInfo.dwChannel[i] == 1)
					{
						if(hkdev->dev.alarmcallback)
						{
							alarm.stream_info.alarm_stream_info.channelid = i;
							
							if(hkdev->dev.alarmcallback)
								hkdev->dev.alarmcallback(CALLBACK_TYPE_ALARM_STREAM, &alarm, &hkdev->dev.alarmuserdata);		
						}
					}
				}
			}

			break;
		}
		default:
			jtprintf("[%s]lCommand %d, not handle it\n", __FUNCTION__, lCommand);
		break;
	}
}

static inline long hk_channelnum_convert(struct hkdevice* dev, int channelnum)
{
	long inner_channel = dev->info.byStartChan + channelnum;
	jtprintf("[%s]channelnum %d convert to inner_channel %d\n"
		, __FUNCTION__, channelnum, inner_channel);
	
	return inner_channel;
}
static inline long hk_audiochannelnum_convert(struct hkdevice* dev, int audiochannelnum)
{
	return audiochannelnum+1;
}

static inline DWORD hk_presetnum_convert(int presetnum)
{
	return presetnum+1;
}

static inline DWORD hk_cruisenum_convert(int cruisenum)
{
	return cruisenum+1;
}

static int hk_resolution_convert(int resolution, int *width, int *height)
{
    /*分辨率0-DCIF 1-CIF, 2-QCIF, 3-4CIF, 4-2CIF 5（保留）,6-QVGA(320*240), 7-QQVGA(160*120), 13-576*576,
    16-VGA（640*480）, 17-UXGA（1600*1200）, 18-SVGA （800*600）,
    19-HD720p（1280*720）,20-XVGA,  21-HD900p, 23-1536*1536,24-1920*1920, 27-1920*1080(1080p), 
    28-2560*1920, 29-1600*304, 30-2048*1536, 31-2448*2048,32-2448*1200,
    33-2448*800,34-XGA（1024*768），35-SXGA（1280*1024）,36-WD1(960*576/960*480),37-1080i,38-WXGA+(1440*900)
	39-HD_F(1920*1080/1280*720),40-HD_H(1920*540/1280*360),41-HD_Q(960*540/630*360)
	42-2336*1744, 43-1920*1456, 44-2592*2048, 45-3296*2472, 46-1376*768, 47-1366*768,48-1360*768,49-WSXGA+
	0xff-Auto(使用当前码流分辨率)*/

	switch(resolution)
	{
		case 0://0-DCIF 
			*width = 584;
			*height = 384;
		break;
		case 1://1-CIF 352*288
			*width = 352;
			*height = 288;		
		break;
		case 2://2-QCIF
			*width = 176;
			*height = 144;	
		break;
		case 3://3-4CIF  704*576
			*width = 704;
			*height = 576;	
		break;
		case 5://
			*width = 0;
			*height = 0;			
		break;
		case 6://6-QVGA(320*240)
			*width = 320;
			*height = 240;				
		break;
		case 7://7-QQVGA(160*120)
			*width = 160;
			*height = 120;	
		break;
		case 13://13-576*576,
			*width = 576;
			*height = 576;	
		break;
		case 16://16-VGA（640*480）
			*width = 640;
			*height = 480;				
		break;
		case 17://17-UXGA（1600*1200）
			*width = 1600;
			*height = 1200;				
		break;
		case 18://18-SVGA （800*600）
			*width = 800;
			*height = 600;				
		break;
		case 19:		  //19-HD720p（1280*720）
			*width = 1280;
			*height = 720;			
		break;
		case 20: 	  //,20-XVGA  1280×960
			*width = 1280;
			*height = 960;			
		break;
		case 21: 	  //21-HD900p  1600*912
			*width = 1600;
			*height = 912;			
		break;
		case 22: 	  //
			*width = 0;
			*height = 0;			
		break;
		case 23:		  //23-1536*1536
			*width = 1536;
			*height = 1536;			
		break;
		case 24://24-1920*1920
			*width = 1920;
			*height = 1920;			
		break;		
		case 27:		  //27-1920*1080(1080p), 
			*width = 1920;
			*height = 1080;			
		break;
		case 28:		  //28-2560*1920, 
			*width = 2560;
			*height = 1920;			
		break;
		case 29://29-1600*304,
			*width = 1600;
			*height = 304;			
		break;
		case 30://, 30-2048*1536
			*width = 2048;
			*height = 1536;			
		break;	
		case 31:		// 31-2448*2048
			*width = 2448;
			*height = 2048;			
		break;	
		case 32://32-2448*1200,
			*width = 2448;
			*height = 1200;			
		break;	
		case 33://33-2448*800 
			*width = 2448;
			*height = 800;			
		break;	
		case 34:		//,34-XGA（1024*768），
			*width = 1024;
			*height = 768;			
		break;	
		case 35:		// 35-SXGA（1280*1024）,
			*width = 1280;
			*height = 1024;			
		break;	
		case 36:		//36-WD1(960*576/960*480),
			*width = 960;
			*height = 576;			
		break;	
		case 37:		//37-1080i
			*width = 0;
			*height = 0;			
		break;	
		case 38:		//,38-WXGA+(1440*900)	
			*width = 1440;
			*height = 900;			
		break;	
		case 39:	//39-HD_F(1920*1080/1280*720)
			*width = 1920;
			*height = 1080;			
		break;	
		case 40:		//,40-HD_H(1920*540/1280*360),
			*width = 1920;
			*height = 540;			
		break;	
		case 41:		// 41-HD_Q(960*540/630*360)
			*width = 960;
			*height = 540;			
		break;	
		case 42:		//42-2336*1744,
			*width = 2336;
			*height = 1744;			
		break;	
		case 43:		//43-1920*1456,
			*width = 1920;
			*height = 1456;			
		break;	
		case 44:		// 44-2592*2048,
			*width = 2592;
			*height = 2048;			
		break;	
		case 45:		//45-3296*2472,
			*width = 3296;
			*height = 2472;			
		break;	
		case 46:		//46-1376*768,
			*width = 1376;
			*height = 768;			
		break;	
		case 47:		// 47-1366*768
			*width = 1366;
			*height = 768;			
		break;	
		case 48:		//  ,48-1360*768
			*width = 1360;
			*height = 768;			
		break;	
		case 49:	//	 ,49-WSXGA+ 1680×1050
			*width = 1680;
			*height = 1050;			
		break;	
		case 0xff:	
			*width = *width;
			*height = *height;			
		break;
		default:
			*width = 0;
			*height = 0;
		return -1;
	}

	return 0;
}

static int hk_get_encode_mode(int type)
{
	 //视频编码类型 0 私有h264;1标准h264; 2标准mpeg4; 3-M-JPEG
	switch(type)
	{
		case 0:	//
			return VIDEO_ENCODE_HIK_H264;
		break;
		case 1:		//
			return VIDEO_ENCODE_VIDEO_H264;
		break;
		case 2: 		//
			return VIDEO_ENCODE_VIDEO_MPEG4;
		break;
		case 3: 		//
			return VIDEO_ENCODE_JPEG;
		break;
		default:
			return VIDEO_ENCODE_UNKNOWN;
	}

	return 0;
}

static int hk_bitrate_convert(DWORD bitrate)
{
    /*视频码率:0-保留，1-16K(保留)，2-32K，3-48k，4-64K，5-80K，6-96K，7-128K，8-160k，9-192K，10-224K，
    11-256K，12-320K，13-384K，14-448K，15-512K，16-640K，17-768K，18-896K，19-1024K，20-1280K
    ，21-1536K，22-1792K，23-2048K，
    24-3072K，25-4096K，26-8192K，27-16384K。最高位(31位)置成1表示是自定义码流，0～30位表示码流值，最小值16k*/

	if(bitrate & 0x8000)
	{
		return bitrate & 0xef;
	}
	else
	{
		switch(bitrate)
		{
			case 1: 	//
				return 16*1024;
			case 2: 		//
				return 32*1024;
			case 3: 		//
				return 48*1024;
			case 4: 	//
				return 64*1024;
			case 5: 		//
				return 80*1024;
			case 6: 		//
				return 96*1024;
			case 7: 	//
				return 128*1024;
			case 8: 		//
				return 160*1024;
			case 9: 		//
				return 192*1024;
			case 10:	//
				return 224*1024;
			case 11:		//
				return 256*1024;
			case 12:		//
				return 320*1024;
			case 13:	//
				return 384*1024;
			case 14:		//
				return 448*1024;
			case 15:		//
				return 512*1024;
			case 16:	//
				return 640*1024;
			case 17:		//
				return 768*1024;
			case 18:		//
				return 896*1024;
			case 19:	//
				return 1024*1024;
			case 20:		//
				return 1280*1024;
			case 21:		//
				return 1536*1024;
			case 22:	//
				return 1792*1024;
			case 23:		//
				return 2048*1024;
			case 24:		//
				return 3072*1024;
			case 25:	//
				return 4096*1024;
			case 26:		//
				return 8192*1024;
			case 27:		//
				return 16384*1024;
		
			default:
				return 0*1024;
		}
	}

	return 0;
}

static int hk_fps_convert(int bitrate)
{
	if(bitrate==0)
		return 25;
	return bitrate;
}

int hk_audio_encode_convert(int type)
{
	 //音频编码类型 0-OggVorbis;1-G711_U;2-G711_A;5-MP2L2;6-G276;7-AAC
	 switch(type)
	 {
		case 0: //
			return AUDIO_OGG_VORBIT;
		case 1:	 //
			return AUDIO_G711U;
		case 2:		 //
			return AUDIO_G711A;
		case 6:		 //
			return AUDIO_G276;
		case 7:		 //
			return AUDIO_AAC;
		default:
			return AUDIO_ENCODE_UNKOWN;
	 }
	 
	 return 0;
}
static int hk_fill_ch_encode_info(struct device* dev, int channel, NET_DVR_COMPRESSIONCFG_V30 *EncodeConfig)
{
	dev->encodeinfo.ch_encode_info[channel].ch_last_get_time = time(0);

	//主
	dev->encodeinfo.ch_encode_info[channel].mainencode.enable = 1;
	dev->encodeinfo.ch_encode_info[channel].mainencode.encodetype = hk_get_encode_mode(EncodeConfig->struNormHighRecordPara.byVideoEncType);
	dev->encodeinfo.ch_encode_info[channel].mainencode.fps = hk_fps_convert(EncodeConfig->struNormHighRecordPara.dwVideoFrameRate);

	hk_resolution_convert(EncodeConfig->struNormHighRecordPara.byResolution
						, &dev->encodeinfo.ch_encode_info[channel].mainencode.width
						, &dev->encodeinfo.ch_encode_info[channel].mainencode.height);

	dev->encodeinfo.ch_encode_info[channel].mainencode.quality = EncodeConfig->struNormHighRecordPara.byPicQuality;
	dev->encodeinfo.ch_encode_info[channel].mainencode.bitrate = hk_bitrate_convert(EncodeConfig->struNormHighRecordPara.dwVideoBitrate);
	dev->encodeinfo.ch_encode_info[channel].mainencode.bitratectl = EncodeConfig->struNormHighRecordPara.byBitrateType;
	dev->encodeinfo.ch_encode_info[channel].mainencode.gop = EncodeConfig->struNormHighRecordPara.wIntervalFrameI;

	//辅1
	dev->encodeinfo.ch_encode_info[channel].sub1encode.enable = 1;
	dev->encodeinfo.ch_encode_info[channel].sub1encode.encodetype = hk_get_encode_mode(EncodeConfig->struNetPara.byVideoEncType);
	dev->encodeinfo.ch_encode_info[channel].sub1encode.fps = hk_fps_convert(EncodeConfig->struNetPara.dwVideoFrameRate);
	
	hk_resolution_convert(EncodeConfig->struNetPara.byResolution
						, &dev->encodeinfo.ch_encode_info[channel].sub1encode.width
						, &dev->encodeinfo.ch_encode_info[channel].sub1encode.height);
	
	dev->encodeinfo.ch_encode_info[channel].sub1encode.quality = EncodeConfig->struNetPara.byPicQuality;
	dev->encodeinfo.ch_encode_info[channel].sub1encode.bitrate = hk_bitrate_convert(EncodeConfig->struNetPara.dwVideoBitrate);
	dev->encodeinfo.ch_encode_info[channel].sub1encode.bitratectl = EncodeConfig->struNetPara.byBitrateType;
	dev->encodeinfo.ch_encode_info[channel].sub1encode.gop = EncodeConfig->struNetPara.wIntervalFrameI;

	//音频
	dev->encodeinfo.ch_encode_info[channel].audioencode.encodetype = hk_audio_encode_convert(EncodeConfig->struNormHighRecordPara.byAudioEncType);
	dev->encodeinfo.ch_encode_info[channel].audioencode.frequency = 8000;
	dev->encodeinfo.ch_encode_info[channel].audioencode.bitrate = 16000;
	dev->encodeinfo.ch_encode_info[channel].audioencode.channel = 1;
	dev->encodeinfo.ch_encode_info[channel].audioencode.depth = 16;

	return 0;
}

struct hkdevice *hk_alloc_device(void)
{
	hkdevice *device = (hkdevice *)_alloc_device(&hk_ops);
	if(device)
	{
		hk_device_init(device);
		return device;
	}

	return NULL;
}

void SDK_Version()
{
    unsigned int uiVersion = NET_DVR_GetSDKBuildVersion();

    char strTemp[1024] = {0};
    sprintf(strTemp, "HCNetSDK V%d.%d.%d.%d\n", \
        (0xff000000 & uiVersion)>>24, \
        (0x00ff0000 & uiVersion)>>16, \
        (0x0000ff00 & uiVersion)>>8, \
        (0x000000ff & uiVersion));
    jtprintf(strTemp);
}

int hk_lib_init()
{
	jtprintf("[%s]\n", __FUNCTION__);

    SDK_Version();
    //NET_DVR_SetLogToFile(3, "./sdkLog");

	if(!g_hk_initaled && NET_DVR_Init())
	{
		//H264_DVR_SetDVRMessCallBack(hk_mess_callback, (unsigned long)&devicelist);

		//注册接收异常消息的回调函数
		NET_DVR_SetExceptionCallBack_V30(0, NULL, hk_mess_callback, &devicelist);

		if(0==NET_DVR_SetConnectTime(20000, 1))
		{
			jtprintf("[%s]NET_DVR_SetConnectTime error %d\n", __FUNCTION__, NET_DVR_GetLastError());
		}

		jtprintf("[%s]init success %d\n", __FUNCTION__, g_hk_initaled);
		++g_hk_initaled;
	}

	return SUCCESS;
}

static int hk_init(struct device *dev)
{
	jtprintf("[%s]\n", __FUNCTION__);
	/*if(!g_hk_initaled && H264_DVR_Init(hk_disconnect_callback, (unsigned long)&devicelist))
	{
		H264_DVR_SetDVRMessCallBack(hk_mess_callback, (unsigned long)&devicelist);

		H264_DVR_SetSubDisconnectCallBack(hk_sub_disconnect_callBack, (unsigned long)&devicelist);
		jtprintf("[%s]init success %d\n", __FUNCTION__, g_hk_initaled);
		++g_hk_initaled;
	}*/

	return SUCCESS;
}

static int hk_uninit(struct device *dev)
{
	jtprintf("[%s]\n", __FUNCTION__);

	return SUCCESS;
}

static int hk_device_init(struct hkdevice *dev)
{
	struct channel* channel;
	LIST_FOR_EACH_ENTRY(channel, &dev->dev.channels, struct channel, entry)
	{
		struct hkchannel* hkchannel = (struct hkchannel*)channel;
		assert(hkchannel->chn.obj.type == OBJECT_TYPE_CHANNEL);
		struct stream* stream;
		LIST_FOR_EACH_ENTRY(stream, &hkchannel->chn.streams, struct stream, entry)
		{
			struct hkstream* hkstream = (struct hkstream*)stream;
			assert(hkstream->stm.obj.type == OBJECT_TYPE_STREAM);

			hkstream->currentencode = VIDEO_ENCODE_UNKNOWN;
			hkstream->playhandle = HK_INVALIDE_HANDLE;

			stream_init(&hkstream->stm);
		}

		hkchannel->voicehandle = HK_INVALIDE_HANDLE;

		channel_init(&hkchannel->chn);
	}

	/////memset(&dev->info, 0, sizeof(dev->info));//清除设备信息
	dev->alarmhandle = HK_INVALIDE_HANDLE;
	dev->loginid = HK_INVALIDE_HANDLE;

	device_init(&dev->dev);

	return 0;
}

static void hk_show_dev_ability(struct device *dev)
{
	jtprintf("[%s]\n", __FUNCTION__);
	struct hkdevice *hkdev = (hkdevice *)dev;   
	jtprintf("[%s]byChanNum %d, byIPChanNum %d, byStartChan %d, byStartDChan %d, byMainProto %d, bySubProto %d, byAudioChanNum %d\n", __FUNCTION__
		, hkdev->info.byChanNum,  hkdev->info.byIPChanNum
		, hkdev->info.byStartChan, hkdev->info.byStartDChan, hkdev->info.byMainProto, hkdev->info.bySubProto, hkdev->info.byAudioChanNum);
}

static int hk_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	jtprintf("[%s]%s, %u, %s, %s\n", __FUNCTION__, req->Ip, req->Port, req->User, req->Password);

	hk_init(dev);//not good

	///if(get_device(dev)==NULL) add_device(dev);

	struct hkdevice *hkdev = (hkdevice *)dev;
	if(hkdev==NULL)
	{
		jtprintf("[%s]hkdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}
	
	strcpy(dev->ip, req->Ip);
	dev->port = req->Port;
	strcpy(dev->user, req->User);
	strcpy(dev->password, req->Password);
	
	hkdev->loginid = NET_DVR_Login_V30((char*)dev->ip, dev->port,(char*)dev->user, (char*)dev->password, &hkdev->info);
	if(hkdev->loginid != -1)
	{
		jtprintf("[%s]hkdev login success, %s, %d, %s, %s, %ld\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, hkdev->loginid);

		NET_DVR_IPPARACFG_V40 struParams = {0};
		unsigned int uiReturnLen = 0;
		if(NET_DVR_GetDVRConfig(hkdev->loginid,  NET_DVR_GET_IPPARACFG_V40, 0, &struParams, sizeof(NET_DVR_IPPARACFG_V40), &uiReturnLen))
		{
			jtprintf("[%s]NET_DVR_GetDVRConfig NET_DVR_GET_IPPARACFG_V40 ok, byAnalogChanEnable %d, dwAChanNum %d, dwDChanNum %d, dwGroupNum %d, dwSize %d, dwStartDChan %d\n", __FUNCTION__
				, struParams.byAnalogChanEnable, struParams.dwAChanNum, struParams.dwDChanNum
				, struParams.dwGroupNum, struParams.dwSize, struParams.dwStartDChan);
		}
		else
		{
			jtprintf("[%s]NET_DVR_GetDVRConfig NET_DVR_GET_IPPARACFG_V40 failed, %d\n", __FUNCTION__, NET_DVR_GetLastError());
		}

		hk_show_dev_ability(dev);
		return SUCCESS;
	}
	else
	{
		jtprintf("[%s]hkdev login failed, %s, %d, %s, %s, error %d, loginid %d\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, NET_DVR_GetLastError(), hkdev->loginid);
		return LOGIN_FAILED;
	}

	return LOGIN_FAILED;
}
static int hk_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	
	hkdevice *hkdev = (hkdevice *)dev;
	if(hkdev==NULL)
	{
		jtprintf("[%s]hkdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	struct channel* channel;
	LIST_FOR_EACH_ENTRY(channel, &hkdev->dev.channels, struct channel, entry)
	{
		struct hkchannel* hkchannel = (struct hkchannel*)channel;
		assert(hkchannel->chn.obj.type == OBJECT_TYPE_CHANNEL);
		struct stream* stream;
		LIST_FOR_EACH_ENTRY(stream, &hkchannel->chn.streams, struct stream, entry)
		{
			struct hkstream* hkstream = (struct hkstream*)stream;
			assert(hkstream->stm.obj.type == OBJECT_TYPE_STREAM);

			if(hkstream->playhandle != HK_INVALIDE_HANDLE)
			{
				struct stCloseVideoStream_Req req;
				struct stCloseVideoStream_Rsp rsp;
				req.StreamHandle = (long)hkstream;
				hk_close_video_stream(dev, &req, &rsp);		
			}
		}

		//改为hk_close_video_stream??????
		//if(hkchannel->voicehandle != HK_INVALIDE_HANDLE)
		//{
		//	NET_DVR_StopVoiceCom(hkchannel->voicehandle);
		//	hkchannel->voicehandle = HK_INVALIDE_HANDLE;
		//}

		if(hkchannel->chn.audiocallback)
		{
			struct stCloseAudioStream_Req req;
			struct stCloseAudioStream_Rsp rsp;
		
			req.Channel = hkchannel->chn.id;
			hk_close_audio_stream(dev, &req, &rsp);//关闭音频
		}

		channel_init(&hkchannel->chn);
	}

	if(hkdev->alarmhandle)
	{
		NET_DVR_CloseAlarmChan_V30(hkdev->alarmhandle);
		hkdev->alarmhandle = HK_INVALIDE_HANDLE;
	}

	if(NET_DVR_Logout(hkdev->loginid))
	{
		jtprintf("[%s]hkdev hk_logout success\n", __FUNCTION__);
		
		device_init(&hkdev->dev);
		return SUCCESS;		
	}
	else
	{
		jtprintf("[%s]hkdev login failed\n", __FUNCTION__);
		
		device_init(&hkdev->dev);
		return LOGOUT_FAILED;
	}

	return -2;
}


static int hk_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	hkdevice *hkdev = (hkdevice *)dev;
	if(hkdev==NULL)
	{
		jtprintf("[%s]hkdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	//lock??
	struct hkchannel* chn = NULL;
	struct hkstream* stm = NULL;
	chn = (struct hkchannel*)get_channel(&dev->channels, req->Channel);
	if(chn)
	{
		stm = (struct hkstream*)get_stream_by_id(&chn->chn.streams, req->Codec);
	}
	else
	{
		chn = (struct hkchannel*)alloc_channel(sizeof(struct hkchannel));
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
		stm = (struct hkstream*)alloc_stream_with_videobuf(sizeof(struct hkstream), req->Codec, 1024*1024);
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

	jtprintf("[%s]before H264_DVR_RealPlay %ld, channel %d, nStream %d\n"
			, __FUNCTION__, hkdev->loginid, req->Channel, req->Codec);

	NET_DVR_PREVIEWINFO playstru;
	memset(&playstru, 0 , sizeof(NET_DVR_PREVIEWINFO));
	playstru.lChannel = hk_channelnum_convert(hkdev, req->Channel);//有待验证
	playstru.dwStreamType = req->Codec;
	playstru.dwLinkMode = 0;		//0为tcp模式
	playstru.hPlayWnd = 0;
	playstru.bBlocked = 0;       //
	playstru.bPassbackRecord = 0;//不启用录像回传
	playstru.byPreviewMode = 0;  //正常预览
	///////playstru.byStreamID = 0;
	playstru.byProtoType = 0;

	stm->stm.pulling = 0;

	//方便用户修改上一次的 userdata数据
	if(stm->stm.callback)
		stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_OPENED, NULL, &stm->stm.userdata);

	clear_singlebuf(&stm->stm.videobuf);//

	//long handle = NET_DVR_RealPlay_V40(hkdev->loginid, &playstru, NULL, NULL);
	long handle = NET_DVR_RealPlay_V40(hkdev->loginid, &playstru, hk_real_data_callback_v2, stm);
	//long handle = NET_DVR_RealPlay_V40(hkdev->loginid, &playstru, NULL, NULL);
	if(handle == -1)
	{
		stm->playhandle = HK_INVALIDE_PLAYHANDLE;
		stm->stm.pulling = 0;

		jtprintf("[%s]start real stream wrong! m_iPlayhandle %ld, channel %d, nStream %d, error %d\n"
			, __FUNCTION__, handle, req->Channel, req->Codec, NET_DVR_GetLastError());
		return OPEN_VIDEO_STREAM_FAILED;
	}
	else
	{
		jtprintf("[%s]start real stream ok\n", __FUNCTION__);

		stm->playhandle = handle;
		stm->stm.pulling = 1;
		stm->stm.llbegintime = 0;///改为时间?????
		stm->currentencode = VIDEO_ENCODE_VIDEO_H264;//先默认了//从dev里得到他来填一个
		stm->stm.callback = (jt_stream_callback)req->Callback;
		stm->stm.userdata = req->UserData;
		
		rsp->StreamHandle = (long)stm;
		jtprintf("[%s]rsp->StreamHandle %ld\n", __FUNCTION__, rsp->StreamHandle);

		/*if(NET_DVR_SetRealDataCallBack(handle,hk_standard_real_data_callback_v2,(DWORD)stm))
		//if(NET_DVR_SetStandardDataCallBack(handle, hk_standard_real_data_callback_v2, (DWORD)stm))
		{
			jtprintf("[%s]NET_DVR_SetStandardDataCallBack ok\n", __FUNCTION__);
		}
		else
		{
			jtprintf("[%s]set video callback failed!, %d\n", __FUNCTION__, NET_DVR_GetLastError());
			return SET_VIDEO_CALLBACK_FAILED;
		}*/

		
	}

	return SUCCESS;
}
static int hk_close_video_stream(struct device *dev, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	hkdevice *hkdev = (hkdevice *)dev;
	if(hkdev==NULL)
	{
		jtprintf("[%s]hkdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct hkstream* stm = (struct hkstream*)get_stream_by_dev(dev, (struct stream*)req->StreamHandle);
	if(stm==NULL)
	{
		jtprintf("[%s]stm null\n", __FUNCTION__);
		return INVALID_STREAM_NO_FAILED;
	}

	if(stm->playhandle==HK_INVALIDE_PLAYHANDLE)
	{
		jtprintf("[%s]stm->playhandle==-1, aready closed\n", __FUNCTION__);
		stm->stm.pulling = 0;
		stm->playhandle = 0L;
		stm->stm.callback = NULL;
		stm->stm.llbegintime = 0;
		return SUCCESS;
	}

	if(NET_DVR_SetRealDataCallBack(stm->playhandle, NULL, 0)==0)
	{
		jtprintf("[%s]set video callback failed! error %d\n", __FUNCTION__, NET_DVR_GetLastError());
	}

	clear_singlebuf(&stm->stm.videobuf);//

	if(NET_DVR_StopRealPlay(stm->playhandle))
	{
		if(stm->stm.callback)
			stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_CLOSEED, SUCCESS, &stm->stm.userdata);
		
		jtprintf("[%s]H264_DVR_StopRealPlay ok!\n", __FUNCTION__);
		stm->stm.pulling = 0;
		stm->playhandle = HK_INVALIDE_PLAYHANDLE;
		stm->stm.callback = NULL;
		stm->stm.llbegintime = 0;
	}
	else
	{
		if(stm->stm.callback)
			stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_CLOSEED, (void*)OPEN_AUDIO_STREAM_FAILED, &stm->stm.userdata);

		stm->stm.pulling = 0;
		//stm->playhandle = HK_INVALIDE_PLAYHANDLE;
		stm->stm.callback = NULL;
		stm->stm.llbegintime = 0;
		
		jtprintf("[%s]H264_DVR_StopRealPlay error, %d\n", __FUNCTION__, NET_DVR_GetLastError());
		return CLOSE_VIDEO_STREAM_FAILED;
	}
	
	return SUCCESS;
}
static int hk_operator_channel(struct channel *chn, int type, void* data)
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

		void* tmp = chn->audiouserdata;

		chn->audiocallback = req->Callback;
		chn->audiouserdata = req->UserData;
		
		if(chn->audiocallback)
			chn->audiocallback(CALLBACK_TYPE_AUDIO_STREAM_OPENED, SUCCESS, &tmp);
	}
	else if(CHECK_AUDIO_CHANNEL==type)//
	{
		jtprintf("[%s]CHECK_AUDIO_CHANNEL \n", __FUNCTION__);
		
		if(chn->audiocallback && chn->audiouserdata)
		{
			++(*((int*)data));
		}
	}

	return 0;
}
static int hk_open_audio_stream(struct device *dev, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	
	hkdevice *hkdev = (hkdevice *)get_device(dev);
	if(hkdev==NULL)
	{
		jtprintf("[%s]hkdev null\n", __FUNCTION__);
		return DEVICE_NO_FOUND;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, hkdev->dev.ip, hkdev->dev.port);

	if(hkdev->info.byAudioChanNum==0)
	{
		jtprintf("[%s]device not suport audio\n", __FUNCTION__);
		return NOT_SURPORT;
	}

	struct hkchannel* chn = NULL;
	chn = (struct hkchannel*)get_channel(&(hkdev->dev.channels), req->Channel);
	if(chn == NULL)
	{
		return INVALID_CHANNEL_NO_FAILED;
	}

	rsp->ChannelHandle = (long)do_channel(&dev->channels, req->Channel, hk_operator_channel, START_AUDIO, req);

	return SUCCESS;
}
static int hk_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	
	hkdevice *hkdev = (hkdevice *)get_device(dev);
	if(hkdev==NULL)
	{
		jtprintf("[%s]hkdev null\n", __FUNCTION__);
		return DEVICE_NO_FOUND;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, hkdev->dev.ip, hkdev->dev.port);

	struct hkchannel* chn = NULL;
	chn = (struct hkchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		jtprintf("[%s]no channel %d\n", __FUNCTION__, req->Channel);
		return INVALID_CHANNEL_NO_FAILED;
	}

	do_channel(&dev->channels, req->Channel, hk_operator_channel, STOP_AUDIO, NULL);

	return SUCCESS;
}


static void printf_hkdevice_encode_info(NET_DVR_COMPRESSIONCFG_V30 *struParams)
{
	jtprintf("[%s]byStreamType %d, byVideoEncType %d, dwVideoFrameRate %d, dwVideoBitrate %d, byResolution %d\n", __FUNCTION__
		, struParams->struNormHighRecordPara.byStreamType
		, struParams->struNormHighRecordPara.byVideoEncType
		, struParams->struNormHighRecordPara.dwVideoFrameRate
		, struParams->struNormHighRecordPara.dwVideoBitrate
		, struParams->struNormHighRecordPara.byResolution);
}
	

static int hk_get_config(struct device *dev, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);	
	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	if(req->Channel > MAX_CHANNEL_ENCODE_INFO - 1)
	{
		jtprintf("[%s]req->Channel(%d) > MAX_CHANNEL_ENCODE_INFO - 1\n", __FUNCTION__, req->Channel);
		return INVALID_CHANNEL_NO_FAILED;
	}

	hkdevice *hkdev = (hkdevice *)dev;
	switch(req->Type)
	{
		case GET_ENCODE_CONFIG://获取编码配置
		{		
			jtprintf("[%s]GET_ENCODE_CONFIG, chn %d, codec %d\n"
				, __FUNCTION__, req->Channel, req->Codec);
			long curtime = time(0);
			if(curtime - hkdev->dev.encodeinfo.ch_encode_info[req->Channel].ch_last_get_time > 120)
			{
				//大于120秒，重新获取
				jtprintf("[%s]re get encode info\n", __FUNCTION__);
				
				hkdev->dev.encodeinfo.ch_encode_info[req->Channel].ch_last_get_time= curtime;

				DWORD uiReturnLen = 0;
				NET_DVR_COMPRESSIONCFG_V30 struParams;//
				BOOL bSuccess = NET_DVR_GetDVRConfig(hkdev->loginid, NET_DVR_GET_COMPRESSCFG_V30
					, hk_channelnum_convert(hkdev, req->Channel), &struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30)
					, &uiReturnLen);
				if (bSuccess && uiReturnLen == sizeof (struParams))
				{
					printf_hkdevice_encode_info(&struParams);
					hk_fill_ch_encode_info((struct device*)hkdev, req->Channel, &struParams);
					//获取成功
					jtprintf("[%s]H264_DVR_GetDevConfig ok, dwRetLen %lu\n", __FUNCTION__, uiReturnLen);
				}
				else
				{
					jtprintf("[%s]GetConfig Wrong:%d,RetLen:%ld  !=  %d , error %d\n"
								, __FUNCTION__,bSuccess,uiReturnLen,sizeof (struParams), NET_DVR_GetLastError());
					return GET_CONFIG_FAILED;
				}
			}
			else
			{
				jtprintf("[%s]curtime(%d) - hkdev->dev.encodeinfo.last_get_time(%d) = %d, use old config\n"
					, __FUNCTION__, curtime, hkdev->dev.encodeinfo.ch_encode_info[req->Channel].ch_last_get_time
					, curtime-hkdev->dev.encodeinfo.ch_encode_info[req->Channel].ch_last_get_time);			
			}

			struct encode_info* einfo = (struct encode_info*)malloc(sizeof(struct encode_info));
			if(req->Codec==0)
			{
				memcpy(einfo, &hkdev->dev.encodeinfo.ch_encode_info[req->Channel].mainencode, sizeof(struct encode_info));
			}
			else
			{
				memcpy(einfo, &hkdev->dev.encodeinfo.ch_encode_info[req->Channel].sub1encode, sizeof(struct encode_info));
			}

			rsp->Config = (char*)einfo;
			rsp->Size = 1;

			printf_device_encode_info(dev);
			
			break;
		}
	}

	

	return SUCCESS;
}
static int hk_set_config(struct device *, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	return NOT_IMPLEMENT;

	return SUCCESS;
}
BOOL CALLBACK hk_alarm_callback_V0(LONG lCommand, char* sDVRIP, char *pBuf, DWORD dwBufLen, DWORD dwUser)
{


	return 0;
}

static int hk_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);	

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	hkdevice *hkdev = (hkdevice *)dev;
	if(hkdev==NULL)
	{
		jtprintf("[%s]hkdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	//海康的要先设置报警回调函数?????
	////if(!NET_DVR_SetDVRMessageCallBack(hk_alarm_callback_V0, (DWORD)0))
	if(!NET_DVR_SetDVRMessageCallBack_V30(hk_alarm_callback, hkdev))
	{
		jtprintf("[%s]NET_DVR_SetDVRMessageCallBack_V30 error %d\n", __FUNCTION__, NET_DVR_GetLastError());
	
		return OPEN_ALARM_STREAM_FAILED;
	}
	
	hkdev->alarmhandle = NET_DVR_SetupAlarmChan_V30(hkdev->loginid);
	if(HK_INVALIDE_HANDLE==hkdev->alarmhandle)
	{
		DWORD err = NET_DVR_GetLastError();
		if(err==NET_DVR_NOSUPPORT)
		{
			jtprintf("[%s]NET_DVR_NOSUPPORT\n", __FUNCTION__);
		}
		
		jtprintf("[%s]hkdev NET_DVR_SetupAlarmChan_V30 failed, error %d, try NET_DVR_SetupAlarmChan\n", __FUNCTION__, NET_DVR_GetLastError());
		//如果失败，试着调用低版本的接口
		/*hkdev->alarmhandle = NET_DVR_SetupAlarmChan(hkdev->loginid);
		if(HK_INVALIDE_HANDLE==hkdev->alarmhandle)
		{
			jtprintf("[%s]hkdev NET_DVR_SetupAlarmChan failed, error %d\n", __FUNCTION__, NET_DVR_GetLastError());
			NET_DVR_SetDVRMessageCallBack_V30(NULL, 0);
			return OPEN_ALARM_STREAM_FAILED;
		}
		else
		{
			jtprintf("[%s]hkdev NET_DVR_SetupAlarmChan ok, alarmcallback %p\n"
				, __FUNCTION__, dev->alarmcallback);
			
			if(dev->alarmcallback)
				dev->alarmcallback(CALLBACK_TYPE_ALARM_STREAM_OPENED, (void*)SUCCESS, &dev->alarmuserdata);

			//设置报警回调函数
			//海康的要先设置报警回调函数?????
			//if(!NET_DVR_SetDVRMessageCallBack_V30(hk_alarm_callback, hkdev))
			//{
			//	jtprintf("[%s]NET_DVR_SetDVRMessageCallBack_V30 error %d\n", __FUNCTION__, NET_DVR_GetLastError());
			//	return OPEN_ALARM_STREAM_FAILED;
			//}

			dev->alarmcallback = req->Callback;
			dev->alarmuserdata = req->UserData;
			rsp->DeviceHandle = (long long)(unsigned long)hkdev;
		}*/

		return OPEN_ALARM_STREAM_FAILED;
	}
	else
	{
		jtprintf("[%s]hkdev NET_DVR_SetupAlarmChan_V30 ok, alarmcallback %p\n"
			, __FUNCTION__, dev->alarmcallback);
		if(dev->alarmcallback)
			dev->alarmcallback(CALLBACK_TYPE_ALARM_STREAM_OPENED, (void*)SUCCESS, &dev->alarmuserdata);

		//海康的要先设置报警回调函数?????
		/*if(!NET_DVR_SetDVRMessageCallBack_V30(hk_alarm_callback, hkdev))
		{
			jtprintf("[%s]NET_DVR_SetDVRMessageCallBack_V30 error %d\n", __FUNCTION__, NET_DVR_GetLastError());
		
			return OPEN_ALARM_STREAM_FAILED;
		}*/

		dev->alarmcallback = req->Callback;
		dev->alarmuserdata = req->UserData;
		rsp->DeviceHandle = (long long)(unsigned long)hkdev;
	}

	return SUCCESS;
}
static int  hk_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);
	
	hkdevice *hkdev = (hkdevice *)dev;
	if(hkdev==NULL)
	{
		jtprintf("[%s]hkdev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(dev->alarmcallback)
		dev->alarmcallback(CALLBACK_TYPE_ALARM_STREAM_CLOSEED, (void*)0, &dev->alarmuserdata);
	dev->alarmcallback = NULL;

	if(NET_DVR_CloseAlarmChan_V30(hkdev->alarmhandle))
	{
		jtprintf("[%s]hkdev NET_DVR_CloseAlarmChan_V30 ok\n", __FUNCTION__);
	}
	else
	{
		jtprintf("[%s]hkdev NET_DVR_CloseAlarmChan_V30 failed\n", __FUNCTION__);
		return CLOSE_ALARM_STREAM_FAILED;
	}

	return SUCCESS;
}

static DWORD hk_ptz_speed_convert(int speed)
{
	//将0~100映射到1~7
	int innerspeed = (7/100.0)*speed+1;
	jtprintf("[hk_ptz_speed_convert]speed convert to %d\n", speed, innerspeed);

	return innerspeed;
}

static DWORD hk_ptz_direction_convert(int jtdirection)
{
	switch(jtdirection)
	{
		case JPTZ_UP:
			return TILT_UP;
		case JPTZ_RIGHT_UP:
			return UP_RIGHT;
		case JPTZ_RIGHT:
			return PAN_RIGHT;
		case JPTZ_RIGHT_DOWN:
			return DOWN_RIGHT;
		case JPTZ_DOWN:
			return TILT_DOWN;
		case JPTZ_LEFT_DOWN:
			return DOWN_LEFT;
		case JPTZ_LEFT:
			return PAN_LEFT;
		case JPTZ_LEFT_UP:
			return UP_LEFT;
	}

	return TILT_DOWN;
}

int hk_stream_valid_handle(struct stream *stm, void* data)
{
	struct hkstream *hkstm = (struct hkstream *)stm;
	if(hkstm->playhandle != HK_INVALIDE_HANDLE)
	{
		return 1;
	}

	return 0;
}

static int hk_ptz_control(struct device * dev, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	jtprintf("[%s]ip %s, port %u, Channel %d, Action %u, Speed %d\n"
		, __FUNCTION__, dev->ip, dev->port, req->Channel, req->Action, req->Speed);

	hkdevice *hkdev = (hkdevice *)dev;
	switch(req->Action)
	{
		case JPTZ_STOP://停止
		
			jtprintf("[%s]ip %s, port %u, Channel %d, Action %u, Speed %d\n"
			, __FUNCTION__, dev->ip, dev->port, req->Channel, req->Action, req->Speed);
			NET_DVR_PTZControlWithSpeed_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), hk_ptz_direction_convert(req->Action), 1, 0);
		break;
		case JPTZ_UP://8个方向
		case JPTZ_RIGHT_UP:
		case JPTZ_RIGHT:
		case JPTZ_RIGHT_DOWN:
		case JPTZ_DOWN:
		case JPTZ_LEFT_DOWN:
		case JPTZ_LEFT:
		case JPTZ_LEFT_UP:

			jtprintf("[%s]ip %s, port %u, Channel %d, Action %u, Speed %d\n"
			, __FUNCTION__, dev->ip, dev->port, req->Channel, req->Action, req->Speed);

			NET_DVR_PTZControlWithSpeed_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), hk_ptz_direction_convert(req->Action), 0, hk_ptz_speed_convert(req->Speed));
				
		break;
		case JPTZ_PUSH_FAR://拉远
			NET_DVR_PTZControlWithSpeed_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), ZOOM_OUT , 0, hk_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_PULL_NEAR://推近
			NET_DVR_PTZControlWithSpeed_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), ZOOM_IN, 0, hk_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_IRIS_ADD://光圈加
			NET_DVR_PTZControlWithSpeed_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), IRIS_OPEN, 0, hk_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_IRIS_SUB://光圈减
			NET_DVR_PTZControlWithSpeed_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), IRIS_CLOSE, 0, hk_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_FOCUS_FAR://焦距++
			NET_DVR_PTZControlWithSpeed_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), FOCUS_FAR, 0, hk_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_FOCUS_NEAR://焦距--
			NET_DVR_PTZControlWithSpeed_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), FOCUS_NEAR, 0, hk_ptz_speed_convert(req->Speed));
		break;
		case JSET_PRESET://设置预置点
			NET_DVR_PTZPreset_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), SET_PRESET, hk_presetnum_convert(req->PresetNum));
		break;
		case JCLEAR_PRESET://清除预置点
			NET_DVR_PTZPreset_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), CLE_PRESET, hk_presetnum_convert(req->PresetNum));
		break;
		case JGOTO_PRESET://转到预置点
			NET_DVR_PTZPreset_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel), GOTO_PRESET, hk_presetnum_convert(req->PresetNum));
		break;
		case JADD_TO_LOOP://添加预置点到巡航
			//删除预置点
			for(int i = 0; i< 32; ++i)
			{
				NET_DVR_PTZCruise_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel)
				, CLE_PRE_SEQ
				, hk_cruisenum_convert(req->TourNum)
				, i+1
				, 0);
			}

			//添加
			for(int i = 0; i< req->SequenceNum; ++i)
			{
				NET_DVR_PTZCruise_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel)
				, FILL_PRE_SEQ
				, hk_cruisenum_convert(req->TourNum)
				, i+1
				, hk_presetnum_convert(req->SequenceGroup[i]));			
			}

			//停顿时间
			NET_DVR_PTZCruise_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel)
				, SET_SEQ_DWELL, hk_cruisenum_convert(req->TourNum), 0, req->TourDelayTime);			

			//速度
			NET_DVR_PTZCruise_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel)
				, SET_SEQ_SPEED, hk_cruisenum_convert(req->TourNum), 0, req->Speed);	
		break;
		case JDEL_FROM_LOOP://将预置点从巡航删除
			//H264_DVR_PTZControlEx(hkdev->loginid, req->Channel, EXTPTZ_DELFROMLOOP, req->TourNum, req->PresetNum, 3, 0);
		break;
		case JSTART_LOOP://开始巡航
			NET_DVR_PTZCruise_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel)
				, RUN_SEQ, hk_cruisenum_convert(req->TourNum), 0, 0);
			break;
		case JSTOP_LOOP://停止巡航
			NET_DVR_PTZCruise_Other(hkdev->loginid, hk_channelnum_convert(hkdev, req->Channel)
				, STOP_SEQ, hk_cruisenum_convert(req->TourNum), 0, 0);	
			break;
		case STOP_SEQ://删除巡航
			//H264_DVR_PTZControlEx(hkdev->loginid, req->Channel, EXTPTZ_CLOSELOOP, req->TourNum, 2, 3, 0);
		break;
		case JPTZ_RESET://云台复位
			//H264_DVR_PTZControlEx(hkdev->loginid, req->Channel, EXTPTZ_OPERATION_RESET, req->PresetNum, 2, 3, 0);
		break;
		default:
			jtprintf("[%s]dev %p, PTZ, unkonwn cmd %d\n", __FUNCTION__, dev, req->Action);
		return UNKONWN_PTZ_COMMAND;
	}

	return 0;
}
static int hk_set_system_time(struct device *dev, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	jtprintf("[%s]try set time to %d-%d-%d %d:%d:%d\n"
		, __FUNCTION__, req->year, req->month, req->day, req->hour, req->minute, req->second);

	hkdevice *hkdev = (hkdevice *)dev;

	NET_DVR_TIME hktime;
	hktime.dwYear = req->year;
	hktime.dwMonth = req->month;
	hktime.dwDay = req->day;
	hktime.dwHour = req->hour;
	hktime.dwMinute = req->minute;
	hktime.dwSecond = req->second;
	DWORD dwInBufferSize = sizeof(NET_DVR_TIME);
	
	if(!NET_DVR_SetDVRConfig(hkdev->loginid, NET_DVR_SET_TIMECFG, 0, &hktime, dwInBufferSize))
	{
		jtprintf("[%s]NET_DVR_SetDVRConfig NET_DVR_SET_TIMECFG failed\n", __FUNCTION__);
		return SET_SYSTEM_TIME_FAILED;
	}
	else
	{
		jtprintf("[%s]NET_DVR_SetDVRConfig NET_DVR_SET_TIMECFG ok\n", __FUNCTION__);
	}

	return SUCCESS;
}
static int hk_start_talk(struct device *dev, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	hkdevice *hkdev = (hkdevice *)dev;

	if(hkdev->info.byAudioChanNum==0)
	{
		return NOT_SURPORT;
	}
	
	struct hkchannel* chn = (struct hkchannel*)get_channel(&hkdev->dev.channels, req->Channel);
	if(chn==NULL)
	{
		return INVALID_CHANNEL_NO_FAILED;
	}

	chn->voicehandle =  NET_DVR_StartVoiceCom_V30(hkdev->loginid
		, hk_audiochannelnum_convert(hkdev, req->Channel), 0, NULL,  NULL);
	if(HK_INVALIDE_HANDLE==chn->voicehandle)
	{
		jtprintf("[%s]NET_DVR_StartVoiceCom_V30 error %d\n", __FUNCTION__, NET_DVR_GetLastError());
		return START_TALK_FAILED;
	}

	return SUCCESS;
}
static int hk_stop_talk(struct device *dev, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	hkdevice *hkdev = (hkdevice *)dev;

	if(hkdev->info.byAudioChanNum==0)
	{
		return NOT_SURPORT;
	}
	
	struct hkchannel* chn = (struct hkchannel*)get_channel(&hkdev->dev.channels, req->Channel);
	if(chn==NULL)
	{
		jtprintf("[%s]INVALID_CHANNEL_NO_FAILED\n", __FUNCTION__);
		return INVALID_CHANNEL_NO_FAILED;
	}

	if(NET_DVR_StopVoiceCom(chn->voicehandle))
	{
		jtprintf("[%s]NET_DVR_StartVoiceCom_V30 error %d\n", __FUNCTION__, NET_DVR_GetLastError());
		chn->voicehandle = HK_INVALIDE_HANDLE;
		return START_TALK_FAILED;
	}

	chn->voicehandle = HK_INVALIDE_HANDLE;

	return SUCCESS;

}
static int hk_send_talk_data(struct device *dev, struct stSendTalkData_Req *req, struct stSendTalkData_Rsp *rsp)
{
	hkdevice *hkdev = (hkdevice *)dev;
	if(hkdev->info.byAudioChanNum==0)
	{
		return NOT_SURPORT;
	}
	
	struct hkchannel* chn = (struct hkchannel*)get_channel(&hkdev->dev.channels, req->Channel);
	if(chn==NULL)
	{
		jtprintf("[%s]INVALID_CHANNEL_NO_FAILED\n", __FUNCTION__);
		return INVALID_CHANNEL_NO_FAILED;
	}

	if(NET_DVR_VoiceComSendData(chn->voicehandle, (char*)req->Data, req->DataLen))
	{
		jtprintf("[%s]NET_DVR_VoiceComSendData error %d\n", __FUNCTION__, NET_DVR_GetLastError());
		return SEND_TALK_DATA_FAILED;
	}
	
	return SUCCESS;
}

static int hk_get_video_effect(struct device *, stGetVideoEffect_Req *req, stGetVideoEffect_Rsp *rsp)
{

	return NOT_IMPLEMENT;
}
static int hk_set_video_effect(struct device *, stSetVideoEffect_Req *req, stSetVideoEffect_Rsp *rsp)
{
	return NOT_IMPLEMENT;
}

