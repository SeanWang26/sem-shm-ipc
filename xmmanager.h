#ifndef XMMANAGER_H
#define XMMANAGER_H

#include "frontdevice.h"
#include "netsdk.h"

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

struct xmdevice *xm_alloc_device();

#define XM_INVALIDE_LOGINID    -1
#define XM_INVALIDE_PLAYHANDLE -1

#endif
