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

	LONG			alarmhandle;//				�������
	NET_DVR_DEVICEINFO_V30 info;
};

struct hkchannel
{
	struct channel  chn;
	long			voicehandle;//				�Խ����
};

struct hkstream
{
	struct stream stm;
	long playhandle;
	int  currentencode;//��ǰ�Ǻ��ֱ��룬xm�Ļص���û��˵�������ﱣ��һ��
};

extern "C" int hk_lib_init();
extern "C" struct hkdevice *hk_alloc_device(void);

#define HK_INVALIDE_HANDLE     -1
#define HK_INVALIDE_LOGINID    -1
#define HK_INVALIDE_PLAYHANDLE -1
#define HK_INVALIDE_PLAYHANDLE -1

#endif
