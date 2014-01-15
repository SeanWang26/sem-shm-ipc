#ifndef XMMANAGER_H
#define XMMANAGER_H

#include "frontdevice.h"
#include "netsdk.h"

struct xmdevice
{
	struct device dev;
	long 			id;
	H264_DVR_DEVICEINFO info;
};

struct xmdevice *xm_alloc_device();

#endif
