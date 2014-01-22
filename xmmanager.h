#ifndef XMMANAGER_H
#define XMMANAGER_H

#include "frontdevice.h"
#include "netsdk.h"
#include "devicetype.h"

struct xmdevice
{
	struct device 	dev;
	long 			loginid;
	long			voicehandle;//¶Ô½²¾ä±ú
	void*           voice_callback;
	void*           userdata;
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
	long audiohandle;
};

struct xmdevice *xm_alloc_device();

#define XM_INVALIDE_LOGINID -1

#endif
