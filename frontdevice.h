#ifndef FRONTDEVICE_H
#define FRONTDEVICE_H

#include "command.h"
#include "list.h"

struct device_ops;

struct device
{
    unsigned int			refcount;    /* reference count */
	unsigned int			type;
	const struct device_ops *ops;
	struct list             channels;
	struct list             inputs;
	struct list             outputs;
};

struct device_ops
{
	size_t size;
	unsigned int type;
    void (*init)(struct device *);
	void (*uninit)(struct device *);
    int (*login)(struct device *, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
    int  (*logout)(struct device *, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
	int  (*open_video_stream)(struct device *, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
	int  (*close_video_stream)(struct device *);
	int  (*open_audio_stream)(struct device *);
	int  (*close_audio_stream)(struct device *);
};

struct device_debug
{
	
};

struct device  *alloc_device( const struct device_ops *ops);

#endif
