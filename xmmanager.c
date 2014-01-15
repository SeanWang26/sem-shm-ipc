#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "xmmanager.h"
#include "devicetype.h"


static void xm_init(struct device *dev);
static void xm_uninit(struct device *dev);
static int xm_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
static int xm_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
static int xm_open_video_stream(struct device *dev, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
static int xm_close_video_stream(struct device *dev);
static int xm_open_audio_stream(struct device *dev);
static int xm_close_audio_stream(struct device *dev);

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
	xm_close_audio_stream
};

static int real_data_callback_v2(long lRealHandle, const PACKET_INFO_EX *pFrame, unsigned int dwUser)
{
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

	printf("xm get frame user %u, type %d, size %u\n", dwUser, pFrame->nPacketType, pFrame->dwPacketSize);
	
	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
	return 0;
}


struct xmdevice *xm_alloc_device()
{
	xmdevice *device = (xmdevice *)alloc_device(xm_ops);
	if(device)
	{
		device->id = 0;
		memset(&device->info, 0 ,sizeof(device->info));
		return device;
	}

	return NULL;
}

static void xm_init(struct device *dev)
{
	printf("[%s]\n", __FUNCTION__);
	if(H264_DVR_Init(NULL,0))
	{
		//success
	}
}

static void xm_uninit(struct device *dev)
{
	H264_DVR_Cleanup();
}

static int xm_login(struct device *dev, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);

	xmdevice *xmdev = (xmdevice *)dev;
	assert(xmdev!=NULL);
	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return -1;
	}
	
	memset(&xmdev->info,0,sizeof(xmdev->info));
	int err = 0;			
	xmdev->id = H264_DVR_Login((char*)req->Ip, req->Port,(char*)req->User,(char*)req->Password,(LPH264_DVR_DEVICEINFO)(&xmdev->info),&err);		
	if(xmdev->id > 0)
	{
		printf("[%s]xmdev login success\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf("[%s]xmdev login failed\n", __FUNCTION__);
		return -2;
	}

	return -2;
}
static int xm_logout(struct device *dev, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	printf("[%s]\n", __FUNCTION__);
	xmdevice *xmdev = (xmdevice *)dev;
	assert(xmdev!=NULL);
	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return -1;
	}

	if(H264_DVR_Logout(xmdev->id))
	{
		xmdev->id = 0;
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
	xmdevice *xmdev = (xmdevice *)dev;
	assert(xmdev!=NULL);
	if(xmdev==NULL)
	{
		printf("[%s]xmdev null\n", __FUNCTION__);
		return -1;
	}

	H264_DVR_CLIENTINFO playstru;
	
	playstru.nChannel = req->Channel;
	playstru.nStream = req->Codec;
	playstru.nMode = 0;
	long m_iPlayhandle = H264_DVR_RealPlay(xmdev->id, &playstru);	
	if(m_iPlayhandle <= 0 )
	{
		printf("[%s]start RealPlay wrong! m_iPlayhandle %ld, channel %d, nStream %d\n"
			, __FUNCTION__, m_iPlayhandle, req->Channel, req->Codec);
	}
	else
	{	
		printf("[%s]start RealPlay ok!\n", __FUNCTION__);
		H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle, real_data_callback_v2, (long)xmdev);
	}

	return 0;
}
static int xm_close_video_stream(struct device *dev)
{
	return 0;
}
static int xm_open_audio_stream(struct device *dev)
{
	printf("[%s]\n", __FUNCTION__);
	return 0;

}
static int xm_close_audio_stream(struct device *dev)
{
	printf("[%s]\n", __FUNCTION__);

	return 0;
}












