#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <memory>
#include <sstream>

#include "snmanager.h"
#include "../devicetype.h"
#include "../jtprintf.h"
#include "../seansinglebuf.h"
#include "../create_detached_thread.h"
#include "../seanh264utility.h"
#include "../stringutility.h"

#include <string>
#include <vector>

using namespace std;

static int g_sn_initaled = 0;

static int sn_device_init(struct sndevice *dev);

static int sn_init(struct device *dev);
static int sn_uninit(struct device *dev);
static int sn_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
static int sn_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
static int sn_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
static int sn_close_video_stream(struct device *dev, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
static int sn_open_audio_stream(struct device *dev, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
static int sn_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
static int sn_get_config(struct device *dev, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
static int sn_set_config(struct device *, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
static int sn_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
static int sn_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
static int sn_ptz_control(struct device *, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
static int sn_set_system_time(struct device *, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);
static int sn_start_talk(struct device *, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp);
static int sn_stop_talk(struct device *, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp);
static int sn_send_talk_data(struct device *, struct stSendTalkData_Req *req, struct stSendTalkData_Rsp *rsp);

static struct device_ops sn_ops = 
{
	sizeof(struct sndevice),
	DEVICE_SN,
    sn_init,
    sn_uninit,
    sn_login,
    sn_logout,
	sn_open_video_stream,
	sn_close_video_stream,
	sn_open_audio_stream,
	sn_close_audio_stream,
	sn_get_config,
	sn_set_config,
	sn_open_alarm_stream,
	sn_close_alarm_stream,
	sn_ptz_control,
	sn_set_system_time,
	sn_start_talk,
	sn_stop_talk,
	sn_send_talk_data
};

void sn_disconnect_callback(long lLoginID, char *pchDVRIP, long nDVRPort, unsigned long dwUser)
{
	FIND_DEVICE_BEGIN(struct sndevice,DEVICE_SN)
	{
		if(dev->loginid == lLoginID)
		{
			sn_device_init(dev);
			jtprintf("[%s]%p, %s, %lu\n", __FUNCTION__, (void*)dwUser, pchDVRIP, nDVRPort);
			break;
		}
	}
	FIND_DEVICE_END
}

/*void sn_sub_disconnect_callBack(long lLoginID, SubConnType type, long nChannel, long dwUser)
{
	FIND_DEVICE_BEGIN(struct sndevice,DEVICE_HK)
	{
		if(dev->loginid == lLoginID)
		{
			jtprintf("[%s]%p, %s, %lu\n", __FUNCTION__, (void*)dwUser, type, nChannel);
			break;
		}
	}
	FIND_DEVICE_END
}*/

/*static int sn_pack_type_convert(enum MEDIA_PACK_TYPE type)
{
	if(VIDEO_P_FRAME==type)
		return P_FRAME;
	else if(VIDEO_I_FRAME==type) 
		return I_FRAME;
	else if(VIDEO_B_FRAME==type || VIDEO_BP_FRAME==type ||VIDEO_BBP_FRAME==type)
		return B_FRAME;
	
	return UNKNOWN_FRAME;
}*/
	
static int dosaveraw( unsigned char* data, unsigned int len)
{
	static FILE* fp=fopen("./raw.bin", "wb+");;
	if(fp==NULL)
		return -1;

	return fwrite(data,len,1,fp);
}

/*#pragma pack(1)
struct sn_ps_pack_start_code  
{
    unsigned char start_code[3];  
    unsigned char stream_id[1];  
};

union sn_ps_littel_endian_size  
{  
    unsigned short int  length;  
    unsigned char       byte[2];  
};

struct sn_ps_program_stream_e  
{  
    sn_ps_pack_start_code     PackStart;  
    sn_ps_littel_endian_size  PackLength;//we mast do exchange  
    char                PackInfo1[2];  
    unsigned char       stuffing_length;  
};
#pragma pack()

//只适用于流表示为0xe?的部分
inline unsigned int move_to_video_payload_data(unsigned char *indata, unsigned int indatalen, unsigned char **outdata, unsigned int *outdatalen)
{
    //printf("[%s]%x %x %x %x\n", __FUNCTION__, Pack[0], Pack[1], Pack[2], Pack[3]);  
    sn_ps_program_stream_e* PSEPack = (sn_ps_program_stream_e*)indata;  

    sn_ps_littel_endian_size pse_length;  
    pse_length.byte[0] = PSEPack->PackLength.byte[1];  
    pse_length.byte[1] = PSEPack->PackLength.byte[0];

	//
    *outdatalen = pse_length.length - 2 - 1 - PSEPack->stuffing_length;  
    if(*outdatalen>0)   
        *outdata = (unsigned char *)PSEPack + sizeof(sn_ps_program_stream_e) + PSEPack->stuffing_length;  
          
    // *leftlength = length - pse_length.length - sizeof(pack_start_code) - sizeof(littel_endian_size);  
 
    return *outdatalen;  
}
*/
#if 0
static void CALLBACK sn_real_data_callback_v2(long lPlayHandle, unsigned int dwDataType, unsigned char *pBuffer, unsigned int dwBufSize, void* pUser)
{
	struct snstream* stream = (struct snstream*)pUser;

/*	if(!stream->stm.pulling)
	{	
		static struct stream* _stream = 0;
		if(_stream != (struct stream*)stream)
			jtprintf("[%s]1 stream->stm.pulling %d, stream %p !!!!!!!!\n"
				, __FUNCTION__, _stream->pulling, _stream);
		_stream = (struct stream*)stream;
		return ;
	}


	if(dwDataType == NET_DVR_STREAMDATA)
	{
		struct sn_ps_pack_start_code *startcode = (struct sn_ps_pack_start_code *)pBuffer;
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
		else
		{

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
*/
	return;
}
#endif

#if 0
static void CALLBACK sn_standard_real_data_callback_v2(long lPlayHandle, unsigned int dwDataType, unsigned char *pBuffer, unsigned int dwBufSize, unsigned int pUser)
{
	//创建一个线程局部变量,标识pulling,   todo.............
	struct snstream* stream = (struct snstream*)pUser;
	
	/*jtprintf("[%s]type %d, size %u, %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n"
		, __FUNCTION__, dwDataType, dwBufSize
		, pBuffer[0], pBuffer[1], pBuffer[2], pBuffer[3] ,pBuffer[4]
		, pBuffer[5], pBuffer[6], pBuffer[7], pBuffer[8] ,pBuffer[9]
		, pBuffer[10], pBuffer[11], pBuffer[12], pBuffer[13] , pBuffer[14]
		, pBuffer[15], pBuffer[16], pBuffer[17], pBuffer[18] , pBuffer[19]);
	*/
	if(dwDataType == NET_DVR_STREAMDATA)
	{
		struct sn_ps_pack_start_code *startcode = (struct sn_ps_pack_start_code *)pBuffer;
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
		//stmdata.stream_info.video_stream_info.frametype = sn_pack_type_convert((enum MEDIA_PACK_TYPE)pFrame->nPacketType);
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

void sn_audio_data_callback(long lTalkHandle, char *pDataBuf, long dwBufSize, char byAudioFlag, long dwUser)
{
	//lock
	//struct sndevice

	
	/*FIND_DEVICE_BEGIN(struct sndevice,DEVICE_HK)
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

static inline int sn_handle_alarm(sndevice *device, char *pBuf, unsigned long dwBufLen)
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

/*static bool sn_mess_callback(long lLoginID, char *pBuf, unsigned long dwBufLen, long dwUser)
{
	//lock
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		assert(device->obj.type == OBJECT_TYPE_DEVICE);
		if(!device->deleted && device->ops->type == DEVICE_HK )
		{	
			if(((sndevice*)device)->loginid == lLoginID)
			{
				sn_handle_alarm((sndevice*)device, pBuf, dwBufLen);
				break;
			}
		}
	}
	
	return 1;
}*/

/*static void CALLBACK sn_handle_exception(unsigned int dwType, long lUserID, long lHandle, void *pUser)
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

	
}*/

/*static void CALLBACK sn_mess_callback(unsigned int dwType, long lUserID, long lHandle, void *pUser)
{
	//lock
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		assert(device->obj.type == OBJECT_TYPE_DEVICE);
		if(!device->deleted && device->ops->type == DEVICE_HK )
		{	
			if(((sndevice*)device)->loginid == lUserID)
			{
				sn_handle_exception(dwType, lUserID, lHandle, pUser);
				break;
			}
		}
	}
}*/


static int sn_cgi_command_inquiry_system(char *data, size_t size, size_t nmemb, void *userdata)
{
	struct object *obj = (struct object *)userdata;	
	assert(obj->type == OBJECT_TYPE_DEVICE);

	jtprintf("%s\n", data);

//能力，第几代
	vector<string> statlines;
	char * s1 = data;
	char * t;
	while((t = strsep_s(&s1, "&")))
	{
		statlines.push_back(t);
	}

	for(vector<string>::iterator iter = statlines.begin(); iter!=statlines.end(); ++iter)
	{
		string attri = iter->substr(0, iter->find('='));
		string value = iter->substr(iter->find('=')+1, string::npos);
		//jtprintf("attri %s, value %s\n", attri.c_str(), value.c_str());
	}
	
	return nmemb;	
}



static int sn_alarm_type_convert(unsigned int type)
{
	jtprintf("[%s]sn type %u\n", __FUNCTION__, type);
	switch(type)
	{
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

/*static void CALLBACK sn_alarm_callback(long lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, unsigned int dwBufLen, void* pUser)
{
	struct sndevice* sndev = (struct sndevice*)pUser;
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
			alarm.stream_info.alarm_stream_info.reason = sn_alarm_type_convert(struAlarmInfo.dwAlarmType);
			if(alarm.stream_info.alarm_stream_info.reason != ALARM_TYPE_UNKNOWN)
			{
				for (int i=0; i<MAX_CHANNUM; i++)	//#define MAX_CHANNUM	16	//最大通道数
				{
					if (struAlarmInfo.byChannel[i] == 1)
					{
						if(sndev->dev.alarmcallback)
						{
							alarm.stream_info.alarm_stream_info.channelid = i;
							
							if(sndev->dev.alarmcallback)
								sndev->dev.alarmcallback(CALLBACK_TYPE_ALARM_STREAM, &alarm, &sndev->dev.alarmuserdata);		
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
			alarm.stream_info.alarm_stream_info.reason = sn_alarm_type_convert(struAlarmInfo.dwAlarmType);
			if(alarm.stream_info.alarm_stream_info.reason != ALARM_TYPE_UNKNOWN)
			{
				for (int i=0; i<MAX_CHANNUM; i++)	//#define MAX_CHANNUM	16	//最大通道数
				{
					if (struAlarmInfo.dwChannel[i] == 1)
					{
						if(sndev->dev.alarmcallback)
						{
							alarm.stream_info.alarm_stream_info.channelid = i;
							
							if(sndev->dev.alarmcallback)
								sndev->dev.alarmcallback(CALLBACK_TYPE_ALARM_STREAM, &alarm, &sndev->dev.alarmuserdata);		
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
}*/

static inline long sn_channelnum_convert(struct sndevice* dev, int channelnum)
{
	/*long inner_channel = dev->info.byStartChan + channelnum;
	jtprintf("[%s]channelnum %d convert to inner_channel %d\n"
		, __FUNCTION__, channelnum, inner_channel);
	*/
	return 0;
}
static inline long sn_audiochannelnum_convert(struct sndevice* dev, int audiochannelnum)
{
	return audiochannelnum+1;
}

static inline unsigned int sn_presetnum_convert(int presetnum)
{
	return presetnum+1;
}

static inline unsigned int sn_cruisenum_convert(int cruisenum)
{
	return cruisenum+1;
}

static int sn_resolution_convert(int resolution, int *width, int *height)
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

static int sn_get_encode_mode(int type)
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

static int sn_bitrate_convert(unsigned int bitrate)
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

static int sn_fps_convert(int bitrate)
{
	if(bitrate==0)
		return 25;
	return bitrate;
}

int sn_audio_encode_convert(int type)
{
	 //音频编码类型 0-OggVorbis;1-G711_U;2-G711_A;5-MP2L2;6-G276;7-AAC
/*	 switch(type)
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
	 */
	 return 0;
}
/*static int sn_fill_ch_encode_info(struct device* dev, int channel, NET_DVR_COMPRESSIONCFG_V30 *EncodeConfig)
{
	dev->encodeinfo.ch_encode_info[channel].ch_last_get_time = time(0);

	//主
	dev->encodeinfo.ch_encode_info[channel].mainencode.enable = 1;
	dev->encodeinfo.ch_encode_info[channel].mainencode.encodetype = sn_get_encode_mode(EncodeConfig->struNormHighRecordPara.byVideoEncType);
	dev->encodeinfo.ch_encode_info[channel].mainencode.fps = sn_fps_convert(EncodeConfig->struNormHighRecordPara.dwVideoFrameRate);

	sn_resolution_convert(EncodeConfig->struNormHighRecordPara.byResolution
						, &dev->encodeinfo.ch_encode_info[channel].mainencode.width
						, &dev->encodeinfo.ch_encode_info[channel].mainencode.height);

	dev->encodeinfo.ch_encode_info[channel].mainencode.quality = EncodeConfig->struNormHighRecordPara.byPicQuality;
	dev->encodeinfo.ch_encode_info[channel].mainencode.bitrate = sn_bitrate_convert(EncodeConfig->struNormHighRecordPara.dwVideoBitrate);
	dev->encodeinfo.ch_encode_info[channel].mainencode.bitratectl = EncodeConfig->struNormHighRecordPara.byBitrateType;
	dev->encodeinfo.ch_encode_info[channel].mainencode.gop = EncodeConfig->struNormHighRecordPara.wIntervalFrameI;

	//辅1
	dev->encodeinfo.ch_encode_info[channel].sub1encode.enable = 1;
	dev->encodeinfo.ch_encode_info[channel].sub1encode.encodetype = sn_get_encode_mode(EncodeConfig->struNetPara.byVideoEncType);
	dev->encodeinfo.ch_encode_info[channel].sub1encode.fps = sn_fps_convert(EncodeConfig->struNetPara.dwVideoFrameRate);
	
	sn_resolution_convert(EncodeConfig->struNetPara.byResolution
						, &dev->encodeinfo.ch_encode_info[channel].sub1encode.width
						, &dev->encodeinfo.ch_encode_info[channel].sub1encode.height);
	
	dev->encodeinfo.ch_encode_info[channel].sub1encode.quality = EncodeConfig->struNetPara.byPicQuality;
	dev->encodeinfo.ch_encode_info[channel].sub1encode.bitrate = sn_bitrate_convert(EncodeConfig->struNetPara.dwVideoBitrate);
	dev->encodeinfo.ch_encode_info[channel].sub1encode.bitratectl = EncodeConfig->struNetPara.byBitrateType;
	dev->encodeinfo.ch_encode_info[channel].sub1encode.gop = EncodeConfig->struNetPara.wIntervalFrameI;

	//音频
	dev->encodeinfo.ch_encode_info[channel].audioencode.encodetype = sn_audio_encode_convert(EncodeConfig->struNormHighRecordPara.byAudioEncType);
	dev->encodeinfo.ch_encode_info[channel].audioencode.frequency = 8000;
	dev->encodeinfo.ch_encode_info[channel].audioencode.bitrate = 25*1024;
	dev->encodeinfo.ch_encode_info[channel].audioencode.channel = 1;
	dev->encodeinfo.ch_encode_info[channel].audioencode.depth = 16;

	return 0;
}*/

struct sndevice *sn_alloc_device(void)
{
	sndevice *device = (sndevice *)_alloc_device(&sn_ops);
	if(device)
	{
		sn_device_init(device);
		return device;
	}

	return NULL;
}

int sn_lib_init()
{
	jtprintf("[%s]\n", __FUNCTION__);

	if(!g_sn_initaled)
	{
		curl_global_init(CURL_GLOBAL_DEFAULT);
		jtprintf("[%s]init success %d\n", __FUNCTION__, g_sn_initaled);
		++g_sn_initaled;
	}

	return SUCCESS;
}

static int sn_init(struct device *dev)
{
	jtprintf("[%s]\n", __FUNCTION__);
	/*if(!g_sn_initaled && H264_DVR_Init(sn_disconnect_callback, (unsigned long)&devicelist))
	{
		H264_DVR_SetDVRMessCallBack(sn_mess_callback, (unsigned long)&devicelist);

		H264_DVR_SetSubDisconnectCallBack(sn_sub_disconnect_callBack, (unsigned long)&devicelist);
		jtprintf("[%s]init success %d\n", __FUNCTION__, g_sn_initaled);
		++g_sn_initaled;
	}*/

	return SUCCESS;
}

static int sn_uninit(struct device *dev)
{
	jtprintf("[%s]\n", __FUNCTION__);

	return SUCCESS;
}

static int sn_device_init(struct sndevice *dev)
{
	struct channel* channel;
	LIST_FOR_EACH_ENTRY(channel, &dev->dev.channels, struct channel, entry)
	{
		struct snchannel* snchannel = (struct snchannel*)channel;
		assert(snchannel->chn.obj.type == OBJECT_TYPE_CHANNEL);
		struct stream* stream;
		LIST_FOR_EACH_ENTRY(stream, &snchannel->chn.streams, struct stream, entry)
		{
			struct snstream* snstream = (struct snstream*)stream;
			assert(snstream->stm.obj.type == OBJECT_TYPE_STREAM);

			snstream->currentencode = VIDEO_ENCODE_UNKNOWN;
			//////snstream->playhandle = HK_INVALIDE_HANDLE;

			stream_init(&snstream->stm);
		}

		///////////snchannel->voicehandle = HK_INVALIDE_HANDLE;

		channel_init(&snchannel->chn);
	}

	/////memset(&dev->info, 0, sizeof(dev->info));//清除设备信息
	//////////////dev->alarmhandle = HK_INVALIDE_HANDLE;
	/////////////dev->loginid = HK_INVALIDE_HANDLE;
	dev->generation = 5;    //默认看做第5代

	device_init(&dev->dev);

	return 0;
}

static void sn_show_dev_ability(struct device *dev)
{
	/*jtprintf("[%s]\n", __FUNCTION__);
	struct sndevice *sndev = (sndevice *)dev;   
	jtprintf("[%s]byChanNum %d, byIPChanNum %d, byStartChan %d, byStartDChan %d, byMainProto %d, bySubProto %d, byAudioChanNum %d\n", __FUNCTION__
		, sndev->info.byChanNum,  sndev->info.byIPChanNum
		, sndev->info.byStartChan, sndev->info.byStartDChan, sndev->info.byMainProto, sndev->info.bySubProto, sndev->info.byAudioChanNum);
*/
}

static int sn_ipc_cgi_inquiry(void* callback, void* userdata, char* url, char* user, char* password, int head)
{
	CURLcode code;
	char curlerrorBuffer[CURL_ERROR_SIZE];
	string auth;
	CURL *conn = curl_easy_init();
	if (conn == NULL)
	{
		jtprintf("[%s]Failed to create CURL connection\n", __FUNCTION__);
		return -1;
	}

	code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, curlerrorBuffer);
	if (code != CURLE_OK)
	{
		jtprintf("[%s]Failed to set error buffer [%d]\n", __FUNCTION__, code);
		goto curl_error;
	}

	code = curl_easy_setopt(conn, CURLOPT_URL, url);
	if (code != CURLE_OK)
	{
		jtprintf("[%s]Failed to set URL [%s]\n", __FUNCTION__, curlerrorBuffer);
		goto curl_error;
	}

	code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, callback);//writer
	if(code != CURLE_OK)
	{
		jtprintf("[%s]Failed to set writer [%s]\n", __FUNCTION__, curlerrorBuffer);
		goto curl_error;
	}

	code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, userdata);
	if (code != CURLE_OK)
	{
		jtprintf("[%s]Failed to set write data [%s]\n", __FUNCTION__, curlerrorBuffer);
		goto curl_error;
	}

	code = curl_easy_setopt(conn, CURLOPT_CONNECTTIMEOUT, 4);
	if (code != CURLE_OK)
	{
		jtprintf("[%s]Failed to set connect timeout [%s]\n", __FUNCTION__, curlerrorBuffer);
		goto curl_error;
	}

	auth.append(user).append(":").append(password);
	code = curl_easy_setopt(conn, CURLOPT_USERPWD, auth.c_str());
	if (code != CURLE_OK)
	{
		jtprintf("[%s]Failed to set redirect option [%s]\n", __FUNCTION__, curlerrorBuffer);
		goto curl_error;
	}
	code = curl_easy_setopt(conn, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	if (code != CURLE_OK)
	{
		jtprintf("[%s]Failed to set redirect option [%s]\n", __FUNCTION__, curlerrorBuffer);
		goto curl_error;
	}

	code = curl_easy_setopt(conn, CURLOPT_HTTPGET, 1);
	if (code != CURLE_OK)
	{
		jtprintf("[%s]Failed CURLOPT_HTTPGET [%s]\n", __FUNCTION__, curlerrorBuffer);
		goto curl_error;
	}

	code = curl_easy_setopt(conn, CURLOPT_HEADER, head);
	if (code != CURLE_OK) {
		jtprintf("[%s]Failed to set CURLOPT_HEADER 0\n", __FUNCTION__);
		goto curl_error;
	}

	code = curl_easy_perform(conn);
	if (code != CURLE_OK)
	{
		jtprintf("[%s]Failed to curl_easy_perform [%s]\n", __FUNCTION__, curlerrorBuffer);
		goto curl_error;  
	}
	else
	{
		jtprintf("[%s]curl_easy_perform ok\n", __FUNCTION__);
		return 0;
	}

curl_error:
	curl_easy_cleanup(conn);
	return -1;
}

static int sn_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	jtprintf("[%s]%s, %u, %s, %s\n", __FUNCTION__, req->Ip, req->Port, req->User, req->Password);

	struct sndevice *sndev = (sndevice *)dev;
	if(sndev==NULL)
	{
		jtprintf("[%s]sndev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}
	
	strcpy(dev->ip, req->Ip);
	dev->port = req->Port;
	strcpy(dev->user, req->User);
	strcpy(dev->password, req->Password);

	char strurl[256] = {0};
	sprintf(strurl, "http://%s:%d/command/inquiry.cgi?inq=system", dev->ip, dev->port);
	jtprintf("[%s]strurl %s\n", __FUNCTION__, strurl);

	if(sn_ipc_cgi_inquiry((void*)sn_cgi_command_inquiry_system, sndev, strurl, req->User, req->Password, 0))
	{
		jtprintf("[%s]sn_ipc_cgi_inquiry failed\n", __FUNCTION__);
		return LOGIN_FAILED;
	}
	
	return SUCCESS;
/*	
	sndev->loginid = NET_DVR_Login_V30((char*)dev->ip, dev->port,(char*)dev->user, (char*)dev->password, &sndev->info);
	if(sndev->loginid != -1)
	{
		jtprintf("[%s]sndev login success, %s, %d, %s, %s, %ld\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, sndev->loginid);

		NET_DVR_IPPARACFG_V40 struParams = {0};
		unsigned int uiReturnLen = 0;
		if(NET_DVR_GetDVRConfig(sndev->loginid,  NET_DVR_GET_IPPARACFG_V40, 0, &struParams, sizeof(NET_DVR_IPPARACFG_V40), &uiReturnLen))
		{
			jtprintf("[%s]NET_DVR_GetDVRConfig NET_DVR_GET_IPPARACFG_V40 ok, byAnalogChanEnable %d, dwAChanNum %d, dwDChanNum %d, dwGroupNum %d, dwSize %d, dwStartDChan %d\n", __FUNCTION__
				, struParams.byAnalogChanEnable, struParams.dwAChanNum, struParams.dwDChanNum
				, struParams.dwGroupNum, struParams.dwSize, struParams.dwStartDChan);
		}
		else
		{
			jtprintf("[%s]NET_DVR_GetDVRConfig NET_DVR_GET_IPPARACFG_V40 failed, %d\n", __FUNCTION__, NET_DVR_GetLastError());
		}

		sn_show_dev_ability(dev);
		return SUCCESS;
	}
	else
	{
		jtprintf("[%s]sndev login failed, %s, %d, %s, %s, error %d, loginid %d\n"
			, __FUNCTION__, dev->ip, dev->port, dev->user, dev->password, NET_DVR_GetLastError(), sndev->loginid);
		return LOGIN_FAILED;
	}

	return LOGIN_FAILED;*/
}
static int sn_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	return NOT_IMPLEMENT;	
	/*sndevice *sndev = (sndevice *)dev;
	if(sndev==NULL)
	{
		jtprintf("[%s]sndev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	struct channel* channel;
	LIST_FOR_EACH_ENTRY(channel, &sndev->dev.channels, struct channel, entry)
	{
		struct snchannel* snchannel = (struct snchannel*)channel;
		assert(snchannel->chn.obj.type == OBJECT_TYPE_CHANNEL);
		struct stream* stream;
		LIST_FOR_EACH_ENTRY(stream, &snchannel->chn.streams, struct stream, entry)
		{
			struct snstream* snstream = (struct snstream*)stream;
			assert(snstream->stm.obj.type == OBJECT_TYPE_STREAM);

			if(snstream->playhandle != HK_INVALIDE_HANDLE)
			{
				NET_DVR_StopRealPlay(snstream->playhandle);
				snstream->playhandle = HK_INVALIDE_HANDLE;
			}
		}

		//改为sn_close_video_stream??????
		if(snchannel->voicehandle != HK_INVALIDE_HANDLE)
		{
			NET_DVR_StopVoiceCom(snchannel->voicehandle);
			snchannel->voicehandle = HK_INVALIDE_HANDLE;
		}
		
		channel_init(&snchannel->chn);
	}

	if(sndev->alarmhandle)
	{
		NET_DVR_CloseAlarmChan_V30(sndev->alarmhandle);
		sndev->alarmhandle = HK_INVALIDE_HANDLE;
	}

	if(NET_DVR_Logout(sndev->loginid))
	{
		jtprintf("[%s]sndev sn_logout success\n", __FUNCTION__);
		
		sn_device_init(sndev);
		return SUCCESS;		
	}
	else
	{
		jtprintf("[%s]sndev login failed\n", __FUNCTION__);
		
		sn_device_init(sndev);
		return LOGOUT_FAILED;
	}

	return LOGIN_FAILED;*/
}

static int sn_cgi_command_get_stream(char *data, size_t size, size_t nmemb, void *userdata)
{
	struct snstream* stm = (struct snstream*)userdata;
	
	if(stm->stm.pulling == 1)
	{	
		//curl默认返回最大16384个数据
		if(data[0]==0x00 || data[1]==0x00 || data[2] ==0x00 || data[3] ==0x01)
		{
			unsigned char* vdata = NULL;
			unsigned int vdatalen = 0;
			if(read_from_singlebuf(&stm->stm.videobuf, &vdata, &vdatalen))
			{
				/*jtprintf("[%s]pulling %d, size %d, nmemb %u, %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n"
					, __FUNCTION__, stm->stm.pulling, size, vdatalen
					, vdata[0], vdata[1], vdata[2], vdata[3] ,vdata[4]
					, vdata[5], vdata[6], vdata[7], vdata[8] ,vdata[9]
					, vdata[10], vdata[11], vdata[12], vdata[13] , vdata[14]
					, vdata[15], vdata[16], vdata[17], vdata[18] , vdata[19], vdata[vdatalen-1]);
*/
				struct channel* chn = (struct channel*)stm->stm.obj.parent;
				struct device* dev = (struct device*)chn->obj.parent;

				st_stream_data stmdata;
				stmdata.streamtype = VIDEO_STREAM_DATA;
				stmdata.pdata= (char*)vdata;
				stmdata.datalen = vdatalen;

				if(ispictureseq((unsigned char)vdata[4]) 
					|| isframeseq((unsigned char)vdata[4]) 
					|| isrefrencefream((unsigned char)vdata[4]) )
				{
					stmdata.stream_info.video_stream_info.frametype = I_FRAME;
				}
				else
				{
					stmdata.stream_info.video_stream_info.frametype = P_FRAME;
				}

				if(stm->stm.id==0)//主码流
				{
					stmdata.stream_info.video_stream_info.width = dev->encodeinfo.ch_encode_info[chn->id].mainencode.width;
					stmdata.stream_info.video_stream_info.height = dev->encodeinfo.ch_encode_info[chn->id].mainencode.height;
					stmdata.stream_info.video_stream_info.encode = dev->encodeinfo.ch_encode_info[chn->id].mainencode.encodetype;
					stmdata.stream_info.video_stream_info.fps = dev->encodeinfo.ch_encode_info[chn->id].mainencode.fps;
					stmdata.stream_info.video_stream_info.bitrate = dev->encodeinfo.ch_encode_info[chn->id].mainencode.bitrate*1024;
				}
				else//子码流
				{
					stmdata.stream_info.video_stream_info.width = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.width;
					stmdata.stream_info.video_stream_info.height = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.height;
					stmdata.stream_info.video_stream_info.encode = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.encodetype;
					stmdata.stream_info.video_stream_info.fps = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.fps;
					stmdata.stream_info.video_stream_info.bitrate = dev->encodeinfo.ch_encode_info[chn->id].sub1encode.bitrate*1024;
				}
			
				stmdata.year = 2014;
				stmdata.month = 7;
				stmdata.day = 1;
				stmdata.hour = 1;
				stmdata.minute = 1;
				stmdata.second = 1;
				
				struct timespec ts;
				clock_gettime(CLOCK_REALTIME, &ts);
				if(stm->stm.llbegintime==0ULL)
				{
					stmdata.llbegintime = ts.tv_sec* + ts.tv_nsec/100;//100ns
					stmdata.llrelativetimetick = ts.tv_sec* + ts.tv_nsec/100;//100ns
				}
				else
				{
					stmdata.llbegintime = stm->stm.llbegintime;
					stmdata.llrelativetimetick = ts.tv_sec* + ts.tv_nsec/100 - stmdata.llbegintime;//100ns
				}

				if(stm->stm.callback)
					stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM, &stmdata, &stm->stm.userdata);
			}

			clear_singlebuf(&stm->stm.videobuf);
		}

		write_to_singlebuf(&stm->stm.videobuf, (unsigned char*)data, nmemb);

		return nmemb;
	}

	return 0;
}

static void* sn_cgi_command_get_stream2(void* userdata)
{
	struct snstream* stm = (struct snstream*)userdata;
	struct device *dev = (struct device *)stm->stm.obj.parent->parent;

	char strurl[512]={0};
	sprintf(strurl, "http://%s:%d/image", dev->ip, dev->port);
	jtprintf("[%s]111%s\n", __FUNCTION__, strurl);
	stm->stm.pulling = 1;
	if(!sn_ipc_cgi_inquiry((void*)sn_cgi_command_get_stream, stm, strurl, dev->user, dev->password, 0))
	{
		
	}
	jtprintf("[%s]2222%s\n", __FUNCTION__, strurl);

	return NULL;
}

static int sn_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	
	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	sndevice *sndev = (sndevice *)dev;
	if(sndev==NULL)
	{
		jtprintf("[%s]sndev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	//lock??
	struct snchannel* chn = NULL;
	struct snstream* stm = NULL;
	chn = (struct snchannel*)get_channel(&dev->channels, req->Channel);
	if(chn)
	{
		stm = (struct snstream*)get_stream_by_id(&chn->chn.streams, req->Codec);
	}
	else
	{
		chn = (struct snchannel*)alloc_channel(sizeof(struct snchannel));
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
		stm = (struct snstream*)alloc_stream_with_videobuf(sizeof(struct snstream), req->Codec, 1024*1024);
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

	stm->stm.pulling = 0;
	
	clear_singlebuf(&stm->stm.videobuf);

	//方便用户修改上一次的 userdata数据
	if(stm->stm.callback)
		stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_OPENED, NULL, &stm->stm.userdata);

	pthread_t tid;
	if(create_detached_thread(&tid, sn_cgi_command_get_stream2, stm))
	{
		stm->stm.pulling = 0;
		return OPEN_VIDEO_STREAM_FAILED;
	}

	stm->stm.pulling = 1;
	stm->stm.llbegintime = 0;///改为时间?????
	stm->currentencode = VIDEO_ENCODE_VIDEO_H264;//先默认了//从dev里得到他来填一个
	stm->stm.callback = (jt_stream_callback)req->Callback;
	stm->stm.userdata = req->UserData;
	
	rsp->StreamHandle = (long)stm;
	
	/*

	NET_DVR_PREVIEWINFO playstru;
	memset(&playstru, 0 , sizeof(NET_DVR_PREVIEWINFO));
	playstru.lChannel = sn_channelnum_convert(sndev, req->Channel);//有待验证
	playstru.dwStreamType = req->Codec;
	playstru.dwLinkMode = 0;		//0为tcp模式
	playstru.hPlayWnd = 0;
	playstru.bBlocked = 0;       //
	playstru.bPassbackRecord = 0;//不启用录像回传
	playstru.byPreviewMode = 0;  //正常预览
	///////playstru.byStreamID = 0;
	playstru.byProtoType = 0;

	//方便用户修改上一次的 userdata数据
	if(stm->stm.callback)
		stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_OPENED, NULL, &stm->stm.userdata);

	clear_singlebuf(&stm->stm.videobuf);//

	//long handle = NET_DVR_RealPlay_V40(sndev->loginid, &playstru, NULL, NULL);
	long handle = NET_DVR_RealPlay_V40(sndev->loginid, &playstru, sn_real_data_callback_v2, stm);
	//long handle = NET_DVR_RealPlay_V40(sndev->loginid, &playstru, NULL, NULL);
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
		
	}
	*/

	return SUCCESS;
}
static int sn_close_video_stream(struct device *dev, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	sndevice *sndev = (sndevice *)dev;
	if(sndev==NULL)
	{
		jtprintf("[%s]sndev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	struct snstream* stm = (struct snstream*)get_stream_by_dev(dev, (struct stream*)req->StreamHandle);
	if(stm==NULL)
	{
		jtprintf("[%s]stm null\n", __FUNCTION__);
		return INVALID_STREAM_NO_FAILED;
	}

	if(stm->stm.pulling==0)
	{
		jtprintf("[%s]stm->playhandle==0, aready closed\n", __FUNCTION__);
		stm->stm.pulling = 0;
		stm->playhandle = 0L;
		stm->stm.callback = NULL;
		stm->stm.llbegintime = 0;
		return SUCCESS;
	}

	stm->stm.pulling = 0;

	if(stm->stm.callback)
		stm->stm.callback(CALLBACK_TYPE_VIDEO_STREAM_CLOSEED, SUCCESS, &stm->stm.userdata);

	stm->playhandle = 0L;
	stm->stm.callback = NULL;
	stm->stm.llbegintime = 0;

	clear_singlebuf(&stm->stm.videobuf);

	/*if(NET_DVR_SetRealDataCallBack(stm->playhandle, NULL, 0)==0)
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
	*/
	return SUCCESS;
}
static int sn_operator_channel(struct channel *chn, int type, void* data)
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
static int sn_open_audio_stream(struct device *dev, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	return NOT_IMPLEMENT;
	/*sndevice *sndev = (sndevice *)get_device(dev);
	if(sndev==NULL)
	{
		jtprintf("[%s]sndev null\n", __FUNCTION__);
		return DEVICE_NO_FOUND;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, sndev->dev.ip, sndev->dev.port);

	if(sndev->info.byAudioChanNum==0)
	{
		jtprintf("[%s]device not suport audio\n", __FUNCTION__);
		return NOT_SURPORT;
	}

	struct snchannel* chn = NULL;
	chn = (struct snchannel*)get_channel(&(sndev->dev.channels), req->Channel);
	if(chn == NULL)
	{
		return INVALID_CHANNEL_NO_FAILED;
	}

	rsp->ChannelHandle = (long)do_channel(&dev->channels, req->Channel, sn_operator_channel, START_AUDIO, req);

	return SUCCESS;*/
}
static int sn_close_audio_stream(struct device *dev, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);
	return NOT_IMPLEMENT;
	/*sndevice *sndev = (sndevice *)get_device(dev);
	if(sndev==NULL)
	{
		jtprintf("[%s]sndev null\n", __FUNCTION__);
		return DEVICE_NO_FOUND;
	}

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, sndev->dev.ip, sndev->dev.port);

	struct snchannel* chn = NULL;
	chn = (struct snchannel*)get_channel(&dev->channels, req->Channel);
	if(chn == NULL)
	{
		jtprintf("[%s]no channel %d\n", __FUNCTION__, req->Channel);
		return INVALID_CHANNEL_NO_FAILED;
	}

	do_channel(&dev->channels, req->Channel, sn_operator_channel, STOP_AUDIO, NULL);

	return SUCCESS;*/
}


/*static void printf_sndevice_encode_info(NET_DVR_COMPRESSIONCFG_V30 *struParams)
{
	jtprintf("[%s]byStreamType %d, byVideoEncType %d, dwVideoFrameRate %d, dwVideoBitrate %d, byResolution %d\n", __FUNCTION__
		, struParams->struNormHighRecordPara.byStreamType
		, struParams->struNormHighRecordPara.byVideoEncType
		, struParams->struNormHighRecordPara.dwVideoFrameRate
		, struParams->struNormHighRecordPara.dwVideoBitrate
		, struParams->struNormHighRecordPara.byResolution);
}*/

static int sn_fill_encode_info(struct device* dev, vector<string> &codecinfoS)
{
	for(vector<string>::iterator iter = codecinfoS.begin(); iter!=codecinfoS.end(); ++iter)
	{
		string attri = iter->substr(0, iter->find('='));
		string value = iter->substr(iter->find('=')+1, string::npos);
		jtprintf("[sn_fill_encode_info]str %s, attri %s, value %s\n", iter->c_str(), attri.c_str(), value.c_str());

		if(attri=="ImageCodec1")//
		{
			if(value=="h264")
			{
				jtprintf("[sn_fill_encode_info]ImageCodec1 h264\n");
				dev->encodeinfo.ch_encode_info[0].mainencode.enable = 1;
				dev->encodeinfo.ch_encode_info[0].mainencode.encodetype = VIDEO_ENCODE_VIDEO_H264;
			}
			else if(value=="mpeg4")
			{
				jtprintf("[sn_fill_encode_info]ImageCodec1 mpeg4\n");
				dev->encodeinfo.ch_encode_info[0].mainencode.enable = 1;
				dev->encodeinfo.ch_encode_info[0].mainencode.encodetype = VIDEO_ENCODE_VIDEO_MPEG4;
			}
			else if(value=="jpeg")
			{
				jtprintf("[sn_fill_encode_info]ImageCodec1 jpeg\n");
				dev->encodeinfo.ch_encode_info[0].mainencode.enable = 1;
				dev->encodeinfo.ch_encode_info[0].mainencode.encodetype = VIDEO_ENCODE_JPEG;
			}
			else if(value=="off")
			{
				jtprintf("[sn_fill_encode_info]ImageCodec1 off\n");
				dev->encodeinfo.ch_encode_info[0].mainencode.enable = 0;
			}
		}
		else if(attri=="ImageCodec2")
		{
			if(value=="h264")
			{
				jtprintf("[sn_fill_encode_info]ImageCodec2 h264\n");
				dev->encodeinfo.ch_encode_info[0].sub1encode.enable = 1;
				dev->encodeinfo.ch_encode_info[0].sub1encode.encodetype = VIDEO_ENCODE_VIDEO_H264;
			}
			else if(value=="mpeg4")
			{
				jtprintf("[sn_fill_encode_info]ImageCodec2 mpeg4\n");
				dev->encodeinfo.ch_encode_info[0].sub1encode.enable = 1;
				dev->encodeinfo.ch_encode_info[0].sub1encode.encodetype = VIDEO_ENCODE_VIDEO_MPEG4;
			}
			else if(value=="jpeg")
			{
				jtprintf("[sn_fill_encode_info]ImageCodec2 jpeg\n");
				dev->encodeinfo.ch_encode_info[0].sub1encode.enable = 1;
				dev->encodeinfo.ch_encode_info[0].sub1encode.encodetype = VIDEO_ENCODE_JPEG;
			}
			else if(value=="off")
			{
				jtprintf("[sn_fill_encode_info]ImageCodec2 off\n");
				dev->encodeinfo.ch_encode_info[0].sub1encode.enable = 0;
			}
		}
		else if(attri=="ImageSize1")
		{
			string width = value.substr(0, value.find(','));
			string height = value.substr(value.find(',')+1, string::npos);

			stringstream convert;
			convert << width;
			convert >> dev->encodeinfo.ch_encode_info[0].mainencode.width;

			convert.clear();

			convert << height;
			convert >> dev->encodeinfo.ch_encode_info[0].mainencode.height;
		}
		else if(attri=="ImageSize2")
		{
			string width = value.substr(0, value.find(','));
			string height = value.substr(value.find(',')+1, string::npos);

			stringstream convert;
			convert << width;
			convert >> dev->encodeinfo.ch_encode_info[0].sub1encode.width;

			convert.clear();

			convert << height;
			convert >> dev->encodeinfo.ch_encode_info[0].sub1encode.height;
		}		
		else if(attri=="FrameRate1")
		{
			stringstream convert;
			convert << value;
			convert >> dev->encodeinfo.ch_encode_info[0].mainencode.fps;
		}
		else if(attri=="FrameRate2")
		{
			stringstream convert;
			convert << value;
			convert >> dev->encodeinfo.ch_encode_info[0].sub1encode.fps;
		}
		else if(attri=="BitRate1")
		{
			stringstream convert;
			convert << value;
			convert >> dev->encodeinfo.ch_encode_info[0].mainencode.bitrate;
		}
		else if(attri=="BitRate2")
		{
			stringstream convert;
			convert << value;
			convert >> dev->encodeinfo.ch_encode_info[0].sub1encode.bitrate;
		}

	}

	return 0;
	
//主
/*dev->encodeinfo.ch_encode_info[i].mainencode.enable = EncodeConfig->vEncodeConfigAll[i].dstMainFmt.bVideoEnable;
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

*/
}

static int sn_cgi_command_inquiry_codec(char *data, size_t size, size_t nmemb, void *userdata)
{
	jtprintf("%s\n", data);
	struct device* dev = (struct device*)userdata;

	vector<string> codecinfoS;
	char * s1 = data;
	char * t;
	while((t = strsep_s(&s1, "&")))
	{
		codecinfoS.push_back(t);
	}

	sn_fill_encode_info(dev, codecinfoS);

	return nmemb;
}

static int sn_get_config(struct device *dev, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);

	assert(dev!=NULL);	
	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	sndevice *sndev = (sndevice *)dev;
	switch(req->Type)
	{
		case GET_ENCODE_CONFIG://获取编码配置
		{
			jtprintf("[%s]GET_ENCODE_CONFIG, chn %d, codec %d\n"
				, __FUNCTION__, req->Channel, req->Codec);
			long curtime = time(0);
			if(curtime - sndev->dev.encodeinfo.last_get_time > 120)
			{
				//大于120秒，重新获取
				jtprintf("[%s]re get encode info\n", __FUNCTION__);
				
				sndev->dev.encodeinfo.last_get_time = curtime;

				char strurl[512]={0};
				sprintf(strurl, "http://%s:%d/command/inquiry.cgi?inq=camera", dev->ip, dev->port);
				if(sn_ipc_cgi_inquiry((void*)sn_cgi_command_inquiry_codec, dev, strurl, dev->user, dev->password, 0))
				{
					jtprintf("[%s]get encode info failed\n", __FUNCTION__);
					return GET_CONFIG_FAILED;  
				}				
			}
			else
			{
				jtprintf("[%s]curtime(%d) - sndev->dev.encodeinfo.last_get_time(%d) = %d, use old config\n"
					, __FUNCTION__, curtime, sndev->dev.encodeinfo.ch_encode_info[req->Channel].ch_last_get_time
					, curtime-sndev->dev.encodeinfo.ch_encode_info[req->Channel].ch_last_get_time);			
			}

			struct encode_info* einfo = (struct encode_info*)malloc(sizeof(struct encode_info));
			if(req->Codec==0)
			{
				memcpy(einfo, &sndev->dev.encodeinfo.ch_encode_info[req->Channel].mainencode, sizeof(struct encode_info));
			}
			else
			{
				memcpy(einfo, &sndev->dev.encodeinfo.ch_encode_info[req->Channel].sub1encode, sizeof(struct encode_info));
			}

			rsp->Config = (char*)einfo;
			rsp->Size = 1;

			printf_device_encode_info(dev);
			
			break;
		}
	}

	return SUCCESS;
}
static int sn_set_config(struct device *, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	return NOT_IMPLEMENT;

	return SUCCESS;
}
static int sn_open_alarm_stream(struct device *dev, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	assert(dev!=NULL);	
	return NOT_IMPLEMENT;

	/*jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);

	sndevice *sndev = (sndevice *)dev;
	if(sndev==NULL)
	{
		jtprintf("[%s]sndev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}
	
	sndev->alarmhandle = NET_DVR_SetupAlarmChan_V30(sndev->loginid);
	if(HK_INVALIDE_HANDLE==sndev->alarmhandle)
	{
		jtprintf("[%s]sndev NET_DVR_SetupAlarmChan_V30 failed\n", __FUNCTION__);
		return OPEN_ALARM_STREAM_FAILED;
	}
	else
	{
		jtprintf("[%s]sndev NET_DVR_SetupAlarmChan_V30 ok, alarmcallback %p\n"
			, __FUNCTION__, dev->alarmcallback);
		if(dev->alarmcallback)
			dev->alarmcallback(CALLBACK_TYPE_ALARM_STREAM_OPENED, (void*)SUCCESS, &dev->alarmuserdata);

		//设置报警回调函数
		NET_DVR_SetDVRMessageCallBack_V30(sn_alarm_callback, sndev);

		dev->alarmcallback = req->Callback;
		dev->alarmuserdata = req->UserData;
		rsp->DeviceHandle = (long long)(unsigned long)sndev;
	}

	return SUCCESS;*/
}
static int  sn_close_alarm_stream(struct device *dev, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	return NOT_IMPLEMENT;
	/*assert(dev!=NULL);

	jtprintf("[%s]ip %s, port %u\n", __FUNCTION__, dev->ip, dev->port);
	
	sndevice *sndev = (sndevice *)dev;
	if(sndev==NULL)
	{
		jtprintf("[%s]sndev null\n", __FUNCTION__);
		return DEVICE_NULL_FAILED;
	}

	if(dev->alarmcallback)
		dev->alarmcallback(CALLBACK_TYPE_ALARM_STREAM_CLOSEED, (void*)0, &dev->alarmuserdata);
	dev->alarmcallback = NULL;

	if(NET_DVR_CloseAlarmChan_V30(sndev->alarmhandle))
	{
		jtprintf("[%s]sndev NET_DVR_CloseAlarmChan_V30 ok\n", __FUNCTION__);
	}
	else
	{
		jtprintf("[%s]sndev NET_DVR_CloseAlarmChan_V30 failed\n", __FUNCTION__);
		return CLOSE_ALARM_STREAM_FAILED;
	}

	return SUCCESS;*/
}

static unsigned int sn_ptz_speed_convert(int speed)
{
	//将0~100映射到1~7
	int innerspeed = (7/100.0)*speed+1;
	jtprintf("[sn_ptz_speed_convert]speed convert to %d\n", speed, innerspeed);

	return innerspeed;
}

static unsigned int sn_ptz_direction_convert(int jtdirection)
{
	return NOT_IMPLEMENT;

	/*switch(jtdirection)
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

	return TILT_DOWN;*/
}

int sn_stream_valid_handle(struct stream *stm, void* data)
{
	return NOT_IMPLEMENT;

	/*struct snstream *snstm = (struct snstream *)stm;
	if(snstm->playhandle != HK_INVALIDE_HANDLE)
	{
		return 1;
	}

	return 0;*/
}

static int sn_ptz_control(struct device * dev, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);
	return NOT_IMPLEMENT;

	/*jtprintf("[%s]ip %s, port %u, Channel %d, Action %u, Speed %d\n"
		, __FUNCTION__, dev->ip, dev->port, req->Channel, req->Action, req->Speed);

	sndevice *sndev = (sndevice *)dev;
	switch(req->Action)
	{
		case JPTZ_STOP://停止
		
			jtprintf("[%s]ip %s, port %u, Channel %d, Action %u, Speed %d\n"
			, __FUNCTION__, dev->ip, dev->port, req->Channel, req->Action, req->Speed);
			NET_DVR_PTZControlWithSpeed_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), sn_ptz_direction_convert(req->Action), 1, 0);
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

			NET_DVR_PTZControlWithSpeed_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), sn_ptz_direction_convert(req->Action), 0, sn_ptz_speed_convert(req->Speed));
				
		break;
		case JPTZ_PUSH_FAR://拉远
			NET_DVR_PTZControlWithSpeed_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), ZOOM_OUT , 0, sn_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_PULL_NEAR://推近
			NET_DVR_PTZControlWithSpeed_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), ZOOM_IN, 0, sn_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_IRIS_ADD://光圈加
			NET_DVR_PTZControlWithSpeed_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), IRIS_OPEN, 0, sn_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_IRIS_SUB://光圈减
			NET_DVR_PTZControlWithSpeed_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), IRIS_CLOSE, 0, sn_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_FOCUS_FAR://焦距++
			NET_DVR_PTZControlWithSpeed_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), FOCUS_FAR, 0, sn_ptz_speed_convert(req->Speed));
		break;
		case JPTZ_FOCUS_NEAR://焦距--
			NET_DVR_PTZControlWithSpeed_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), FOCUS_NEAR, 0, sn_ptz_speed_convert(req->Speed));
		break;
		case JSET_PRESET://设置预置点
			NET_DVR_PTZPreset_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), SET_PRESET, sn_presetnum_convert(req->PresetNum));
		break;
		case JCLEAR_PRESET://清除预置点
			NET_DVR_PTZPreset_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), CLE_PRESET, sn_presetnum_convert(req->PresetNum));
		break;
		case JGOTO_PRESET://转到预置点
			NET_DVR_PTZPreset_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel), GOTO_PRESET, sn_presetnum_convert(req->PresetNum));
		break;
		case JADD_TO_LOOP://添加预置点到巡航
			//删除预置点
			for(int i = 0; i< 32; ++i)
			{
				NET_DVR_PTZCruise_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel)
				, CLE_PRE_SEQ
				, sn_cruisenum_convert(req->TourNum)
				, i+1
				, 0);
			}

			//添加
			for(int i = 0; i< req->SequenceNum; ++i)
			{
				NET_DVR_PTZCruise_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel)
				, FILL_PRE_SEQ
				, sn_cruisenum_convert(req->TourNum)
				, i+1
				, sn_presetnum_convert(req->SequenceGroup[i]));			
			}

			//停顿时间
			NET_DVR_PTZCruise_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel)
				, SET_SEQ_DWELL, sn_cruisenum_convert(req->TourNum), 0, req->TourDelayTime);			

			//速度
			NET_DVR_PTZCruise_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel)
				, SET_SEQ_SPEED, sn_cruisenum_convert(req->TourNum), 0, req->Speed);	
		break;
		case JDEL_FROM_LOOP://将预置点从巡航删除
			//H264_DVR_PTZControlEx(sndev->loginid, req->Channel, EXTPTZ_DELFROMLOOP, req->TourNum, req->PresetNum, 3, 0);
		break;
		case JSTART_LOOP://开始巡航
			NET_DVR_PTZCruise_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel)
				, RUN_SEQ, sn_cruisenum_convert(req->TourNum), 0, 0);
			break;
		case JSTOP_LOOP://停止巡航
			NET_DVR_PTZCruise_Other(sndev->loginid, sn_channelnum_convert(sndev, req->Channel)
				, STOP_SEQ, sn_cruisenum_convert(req->TourNum), 0, 0);	
			break;
		case STOP_SEQ://删除巡航
			//H264_DVR_PTZControlEx(sndev->loginid, req->Channel, EXTPTZ_CLOSELOOP, req->TourNum, 2, 3, 0);
		break;
		case JPTZ_RESET://云台复位
			//H264_DVR_PTZControlEx(sndev->loginid, req->Channel, EXTPTZ_OPERATION_RESET, req->PresetNum, 2, 3, 0);
		break;
		default:
			jtprintf("[%s]dev %p, PTZ, unkonwn cmd %d\n", __FUNCTION__, dev, req->Action);
		return UNKONWN_PTZ_COMMAND;
	}

	return 0;*/
}
static int sn_set_system_time(struct device *dev, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);
	return NOT_IMPLEMENT;

	/*jtprintf("[%s]try set time to %d-%d-%d %d:%d:%d\n"
		, __FUNCTION__, req->year, req->month, req->day, req->hour, req->minute, req->second);

	sndevice *sndev = (sndevice *)dev;

	NET_DVR_TIME sntime;
	sntime.dwYear = req->year;
	sntime.dwMonth = req->month;
	sntime.dwDay = req->day;
	sntime.dwHour = req->hour;
	sntime.dwMinute = req->minute;
	sntime.dwSecond = req->second;
	unsigned int dwInBufferSize = sizeof(NET_DVR_TIME);
	
	if(!NET_DVR_SetDVRConfig(sndev->loginid, NET_DVR_SET_TIMECFG, 0, &sntime, dwInBufferSize))
	{
		jtprintf("[%s]NET_DVR_SetDVRConfig NET_DVR_SET_TIMECFG failed\n", __FUNCTION__);
		return SET_SYSTEM_TIME_FAILED;
	}
	else
	{
		jtprintf("[%s]NET_DVR_SetDVRConfig NET_DVR_SET_TIMECFG ok\n", __FUNCTION__);
	}

	return SUCCESS;*/
}
static int sn_start_talk(struct device *dev, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	return NOT_IMPLEMENT;

	/*sndevice *sndev = (sndevice *)dev;

	if(sndev->info.byAudioChanNum==0)
	{
		return NOT_SURPORT;
	}
	
	struct snchannel* chn = (struct snchannel*)get_channel(&sndev->dev.channels, req->Channel);
	if(chn==NULL)
	{
		return INVALID_CHANNEL_NO_FAILED;
	}

	chn->voicehandle =  NET_DVR_StartVoiceCom_V30(sndev->loginid
		, sn_audiochannelnum_convert(sndev, req->Channel), 0, NULL,  NULL);
	if(HK_INVALIDE_HANDLE==chn->voicehandle)
	{
		jtprintf("[%s]NET_DVR_StartVoiceCom_V30 error %d\n", __FUNCTION__, NET_DVR_GetLastError());
		return START_TALK_FAILED;
	}

	return SUCCESS;*/
}
static int sn_stop_talk(struct device *dev, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp)
{
	jtprintf("[%s]\n", __FUNCTION__);
	
	assert(dev!=NULL);

	return NOT_IMPLEMENT;

	/*sndevice *sndev = (sndevice *)dev;

	if(sndev->info.byAudioChanNum==0)
	{
		return NOT_SURPORT;
	}
	
	struct snchannel* chn = (struct snchannel*)get_channel(&sndev->dev.channels, req->Channel);
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

	return SUCCESS;*/

}
static int sn_send_talk_data(struct device *dev, struct stSendTalkData_Req *req, struct stSendTalkData_Rsp *rsp)
{
	return NOT_IMPLEMENT;

	/*sndevice *sndev = (sndevice *)dev;
	if(sndev->info.byAudioChanNum==0)
	{
		return NOT_SURPORT;
	}
	
	struct snchannel* chn = (struct snchannel*)get_channel(&sndev->dev.channels, req->Channel);
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
	
	return SUCCESS;*/
}


