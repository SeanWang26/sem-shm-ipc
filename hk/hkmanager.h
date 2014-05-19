#ifndef XMMANAGER_H
#define XMMANAGER_H

#include "../frontdevice.h"
#include "netsdk.h"

struct hkdevice
{
	struct device 	dev;
	long 			loginid;
	long			voicehandle;//				�Խ����
	///H264_DVR_DEVICEINFO info;
};

struct hkchannel
{
	struct channel chn;
};

struct hkstream
{
	struct stream stm;
	long playhandle;
	int  currentencode;//��ǰ�Ǻ��ֱ��룬xm�Ļص���û��˵�������ﱣ��һ��
};

extern "C" int hk_lib_init();
extern "C" struct hkdevice *hk_alloc_device(void);

#define XM_INVALIDE_LOGINID    -1
#define XM_INVALIDE_PLAYHANDLE -1

#endif
