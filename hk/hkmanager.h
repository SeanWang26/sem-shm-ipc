#ifndef XMMANAGER_H
#define XMMANAGER_H

#include "../frontdevice.h"

#if (defined(WIN32) || defined(WIN64))
	#if (defined(WIN32))
	#warning "using windows i586 include file"
	#include "i586/HCNetSDK.h"
	#else
	#warning "using windows x64 include file"
	#include "x64/HCNetSDK.h"
	#endif
#else
	#ifndef _LP64
	#warning "using linux i586 include file"
	#include "i586/HCNetSDK.h"
	#else
	#warning "using linux x64 include file"
	#include "x64/HCNetSDK.h"
	#endif
#endif

struct hkdevice
{
	struct device 	dev;
	long 			loginid;

	LONG			alarmhandle;//				报警句柄
	NET_DVR_DEVICEINFO_V30 info;
};

struct hkchannel
{
	struct channel  chn;
	long			voicehandle;//				对讲句柄
};

struct hkstream
{
	struct stream stm;
	long playhandle;
	int  currentencode;//当前是何种编码，xm的回调里没有说明，这里保存一个
};

extern "C" int hk_lib_init();
extern "C" struct hkdevice *hk_alloc_device(void);

#define HK_INVALIDE_HANDLE     -1
#define HK_INVALIDE_LOGINID    -1
#define HK_INVALIDE_PLAYHANDLE -1
#define HK_INVALIDE_PLAYHANDLE -1

#endif
