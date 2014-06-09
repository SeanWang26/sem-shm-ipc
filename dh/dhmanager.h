#ifndef DHMANAGER_H
#define DHMANAGER_H

#include "../frontdevice.h"

#if (defined(WIN32) || defined(WIN64))
	#if (defined(WIN32))
	#warning "using windows i586 include file"
	#include "i586/dhnetsdk.h"
	#else
	#warning "using windows x64 include file"
	#include "x64/dhnetsdk.h"
	#endif
#else
	#ifndef _LP64
	#warning "using linux i586 include file"
	#include "i586/dhnetsdk.h"
	#else
	#warning "using linux x64 include file"
	#include "x64/dhnetsdk.h"
	#endif
#endif

struct dhdevice
{
	struct device 	             dev;
	LLONG    		             loginid;
	long			             voicehandle;//�Խ����
	long                         audiohandle;//              ��Ƶ��Ϣ;
	NET_DEVICEINFO               info;
	int                          ProtoVer;  //���Լ�������Э��汾��
	DHDEV_SYSTEM_ATTR_CFG        sysconfig;
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

#define DH_INVALIDE_HANDLE     0
//#define DH_INVALIDE_LOGINID    0
//#define DH_INVALIDE_PLAYHANDLE 0

#endif
