#ifndef XMMANAGER_H
#define XMMANAGER_H

#include "frontdevice.h"
#include "netsdk.h"

struct xmdevice
{
	struct device 	dev;
	long 			loginid;
	long			voicehandle;//				对讲句柄
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
	int  currentencode;//当前是何种编码，xm的回调里没有说明，这里保存一个
};

struct xmdevice *xm_alloc_device();

#define XM_INVALIDE_LOGINID    -1
#define XM_INVALIDE_PLAYHANDLE -1

#endif
