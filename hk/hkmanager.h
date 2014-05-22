#ifndef XMMANAGER_H
#define XMMANAGER_H

#include "../frontdevice.h"
#include "HCNetSDK.h"

struct hkdevice
{
	struct device 	dev;
	long 			loginid;
	long			voicehandle;//				对讲句柄
	NET_DVR_DEVICEINFO_V30 info;
};

struct hkchannel
{
	struct channel chn;
};

struct hkstream
{
	struct stream stm;
	long playhandle;
	int  currentencode;//当前是何种编码，xm的回调里没有说明，这里保存一个
};

extern "C" int hk_lib_init();
extern "C" struct hkdevice *hk_alloc_device(void);

#define HK_INVALIDE_LOGINID    -1
#define HK_INVALIDE_PLAYHANDLE -1

#endif
