//#include "../frontdevice.h"
#include "../jtprintf.h"

#include <unistd.h>


#if (defined(WIN32) || defined(WIN64))
	#if (defined(WIN32))
	#warning "using windows i586 include file"
	#include "i586/netsdk.h"
	#else
	#warning "using windows x64 include file"
	#include "x64/netsdk.h"
	#endif
#else
	#ifndef _LP64
	#warning "using linux i586 include file"
	#include "i586/netsdk.h"
	#else
	#warning "using linux x64 include file"
	#include "x64/netsdk.h"
	#endif
#endif

void CALL_METHOD xm_disconnect_callback(long lLoginID, char *pchDVRIP, long nDVRPort, unsigned long dwUser)
{
	jtprintf("[%s]enter\n", __FUNCTION__);
}
static bool CALL_METHOD xm_mess_callback(long lLoginID, char *pBuf, unsigned long dwBufLen, long dwUser)
{
	//lock
	jtprintf("[%s]enter\n", __FUNCTION__);
	return 1;
}

static void CALL_METHOD xm_sub_disconnect_callBack(long lLoginID, SubConnType type, long nChannel, long dwUser)
{
	jtprintf("[%s]enter\n", __FUNCTION__);

}

int main()
{

	H264_DVR_Init(xm_disconnect_callback, 0);
	
	//H264_DVR_SetDVRMessCallBack(xm_mess_callback, 0);
	
	//H264_DVR_SetSubDisconnectCallBack(xm_sub_disconnect_callBack, 0);

	H264_DVR_DEVICEINFO info;
	int err = 0;
	long loginid = H264_DVR_Login("192.168.0.171", 34567, "admin", "", (LPH264_DVR_DEVICEINFO)&info, &err, TCPSOCKET);		
	if(loginid == 0)
	{
		jtprintf("[%s]1xmdev login wrong, err %d\n", __FUNCTION__, err);
	}
	
	jtprintf("[%s]xmdev login success, err %d\n", __FUNCTION__, err);

	sleep(9000000);
	return 0;
}

