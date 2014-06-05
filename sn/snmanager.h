#ifndef SNMANAGER_H
#define SNMANAGER_H

#include "../frontdevice.h"

#if (defined(WIN32) || defined(WIN64))
	#if (defined(WIN32))
	#warning "using windows i586 include file"
	//#include "i586/HCNetSDK.h"
	#else
	#warning "using windows x64 include file"
	//#include "x64/HCNetSDK.h"
	#endif
#else
	#ifndef _LP64
	#warning "using linux i586 include file"
	//#include "i586/HCNetSDK.h"
	#else
	#warning "using linux x64 include file"
	//#include "x64/HCNetSDK.h"
	#endif
#endif

#include <curl/curl.h>

struct sndevice
{
	struct device 	dev;
	long 			loginid;

	long			alarmhandle;//				�������
	//NET_DVR_DEVICEINFO_V30 info;
	char            curlerrorBuffer[CURL_ERROR_SIZE];
	//CURL *          curl;
	
};

struct snchannel
{
	struct channel  chn;
	long			voicehandle;//				�Խ����
};

struct snstream
{
	struct stream stm;
	long playhandle;
	int  currentencode;//��ǰ�Ǻ��ֱ��룬xm�Ļص���û��˵�������ﱣ��һ��
};

extern "C" int sn_lib_init();
extern "C" struct sndevice *sn_alloc_device(void);

#define SN_INVALIDE_HANDLE     -1
#define SN_INVALIDE_LOGINID    -1
#define SN_INVALIDE_PLAYHANDLE -1
#define SN_INVALIDE_PLAYHANDLE -1

#define HK_INVALIDE_HANDLE     -1
#define HK_INVALIDE_LOGINID    -1
#define HK_INVALIDE_PLAYHANDLE -1
#define HK_INVALIDE_PLAYHANDLE -1


#endif
