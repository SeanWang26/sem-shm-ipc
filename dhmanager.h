#ifndef DHMANAGER_H
#define DHMANAGER_H

#include "frontdevice.h"
#include "dhnetsdk.h"


struct dhdevice
{
	struct device 	dev;
	long 			loginid;
	long			voicehandle;//对讲句柄
	long            audiohandle;//              音频信息;
	NET_DEVICEINFO  info;
};

struct dhchannel
{
	struct channel chn;
};

struct dhstream
{
	struct stream stm;
	long playhandle;
	int  currentencode;//当前是何种编码，xm的回调里没有说明，
};

struct dhdevice *dh_alloc_device();

#define DH_INVALIDE_LOGINID    -1
#define DH_INVALIDE_PLAYHANDLE -1

#endif
