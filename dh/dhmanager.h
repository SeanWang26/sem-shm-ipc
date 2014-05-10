#ifndef DHMANAGER_H
#define DHMANAGER_H

#include "../frontdevice.h"
#include "dhnetsdk.h"


struct dhdevice
{
	struct device 	dev;
	LLONG    		loginid;
	long			voicehandle;//�Խ����
	long            audiohandle;//              ��Ƶ��Ϣ;
	NET_DEVICEINFO  info;
	int             ProtoVer;  //���Լ�������Э��汾��
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

extern "C" int dh_lib_init();
extern "C" struct dhdevice *dh_alloc_device();

#define DH_INVALIDE_LOGINID    -1
#define DH_INVALIDE_PLAYHANDLE -1

#endif
