#ifndef XMMANAGER_H
#define XMMANAGER_H

#include "../frontdevice.h"

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

struct xmdevice
{
	struct device 	dev;
	long 			loginid;
	long			voicehandle;//				�Խ����
	H264_DVR_DEVICEINFO info;
};

struct xmchannel
{
	struct channel chn;
};

struct xmstream
{
	struct stream stm;
	long playhandle;
	int  currentencode;//��ǰ�Ǻ��ֱ��룬xm�Ļص���û��˵�������ﱣ��һ��
};

extern "C" int xm_lib_init();
extern "C" struct xmdevice *xm_alloc_device(void);

#define XM_INVALIDE_LOGINID    -1
#define XM_INVALIDE_PLAYHANDLE -1

#endif
