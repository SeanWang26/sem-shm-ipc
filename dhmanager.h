#ifndef DHMANAGER_H
#define DHMANAGER_H

#include "frontdevice.h"
#include "dhnetsdk.h"


struct dhdevice
{
	struct device 	dev;
	long 			loginid;
	long			voicehandle;//�Խ����
	long            audiohandle;//              ��Ƶ��Ϣ;
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
	int  currentencode;//��ǰ�Ǻ��ֱ��룬xm�Ļص���û��˵����
};

struct dhdevice *dh_alloc_device();

#define DH_INVALIDE_LOGINID    -1
#define DH_INVALIDE_PLAYHANDLE -1

#endif
