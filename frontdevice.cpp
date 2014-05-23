#if (defined(WIN32) || defined(WIN64))

#include "StdAfx.h"
#include "xxmanager.h"

#else

#include <dlfcn.h>
#include <pthread.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "frontdevice.h"
#include "devicetype.h"
#include "jtprintf.h"


#define ALLOC_SPEC_DEVICE(type)   											\
	do{																		\
		static void* dl##type = NULL;										\
		if(dl##type==NULL)													\
		{																	\
			dl##type = dlopen("./libjt"#type".so", RTLD_NOW | RTLD_GLOBAL); \
			if(dl##type==NULL) 											\
			{																	\
				jtprintf("open lib ""./libjt"#type".so failed\n");    \
				return NULL;	                                         \
			}                                                             \
			int (*type##_lib_init)() = NULL; 	dlerror();											\
			type##_lib_init = (int(*)())dlsym(dl##type, #type"_lib_init");	\
			if(type##_lib_init==NULL || type##_lib_init()) 										\
			{ 																			\
				jtprintf("ALLOC_SPEC_DEVICE failed, lib %s, dl %p, interface %p, %s, errno %s\n", "./libjt"#type".so", dl##type, type##_lib_init, ""#type"_lib_init", dlerror());  \
				return NULL; 															\
			} 			            													\
		}																				\
		struct type##device *(*type##_alloc_device)() = NULL; 										\
		type##_alloc_device = (struct type##device *(*)())dlsym(dl##type, ""#type"_alloc_device");	\
		if(type##_alloc_device) 																	\
			return (struct device *)type##_alloc_device(); 											\
	}while(0)

int g_initial=0;
//#include "list.h"
/* grab an object (i.e. increment its refcount) and return the object */

//lock
pthread_rwlock_t rwlockdevicelist;

struct list devicelist = {&devicelist, &devicelist};

int share_lock_devicelist(const char* FunName)
{
	pthread_rwlock_rdlock(&rwlockdevicelist);
	jtprintf("[%s]FUNC %s S Locked\n", __FUNCTION__, FunName);
	return 0;
}
int unique_lock_devicelist(const char* FunName)
{
	pthread_rwlock_wrlock(&rwlockdevicelist);
	jtprintf("[%s]FUNC %s U Locked\n", __FUNCTION__, FunName);
	return 0;
}
int unlock_devicelist(const char* FunName)
{
	pthread_rwlock_unlock(&rwlockdevicelist);
	jtprintf("[%s]FUNC %s Unlocked\n", __FUNCTION__, FunName);
	return 0;
}

struct object *grab_object( void *ptr )
{
    struct object *obj = (struct object *)ptr;
    assert( obj->ref < 0x7fffffff );
	//__sync_add_and_fetch(&obj->ref,1);
    return obj;
}

/* release an object (i.e. decrement its refcount) */
void release_object( void *ptr )
{
    struct object *obj = (struct object *)ptr;
    assert( obj->ref);

	//__sync_fetch_and_sub(&obj->ref,1);
}


int gloal_init()
{
	if(g_initial==0)
	{
		pthread_rwlockattr_t attr;
		pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
		int ret = pthread_rwlock_init(&rwlockdevicelist, &attr);
		if(ret)
		{
			jtprintf("[%s]pthread_rwlock_init failed\n", __FUNCTION__);
		}
		g_initial = 1;
	}
	
	return 0;
}

struct device *_alloc_device( const struct device_ops *ops)
{
    struct device *dev = (struct device *)calloc(1, ops->size);
    if (dev)
    {
        dev->obj.ref = 1;
		dev->obj.type = OBJECT_TYPE_DEVICE;
		dev->obj.name = (char*)"device";//name[ops->type];
		dev->obj.parent = NULL;
        dev->ops      = ops;
		dev->dev_type = ops->type;
		dev->deleted = 0;
		list_init(&dev->entry);//should add to devices list
		
        list_init(&dev->channels);
		list_init(&dev->inputs);
		list_init(&dev->outputs);

		dev->alarmcallback = NULL;
		dev->alarmuserdata = NULL;

		dev->talkcallback = NULL;
		dev->talkuserdata = NULL;
		
        return dev;
    }
	
    return (struct device *)NULL;
}

struct device *alloc_device(unsigned int type)
{
	//if(list_count(&devicelist)>1)
	//{
	//	jtprintf("[%s]device count up to limit\n", __FUNCTION__);
	//	return NULL;
	//}

#if (defined(WIN32) || defined(WIN64))
	if(DEVICE_DH == type
	|| DEVICE_HK == type
	|| DEVICE_SN == type
	|| DEVICE_XM == type
	|| DEVICE_JY == type
	|| DEVICE_HH == type
	)
	{
		return (struct device *)xx_alloc_device(type);
	}
#else
	if(type==DEVICE_XM)
	{
		ALLOC_SPEC_DEVICE(xm);
	}
	else if(type==DEVICE_DH)
	{
		ALLOC_SPEC_DEVICE(dh);
	}
	else if(type==DEVICE_HK)
	{
		ALLOC_SPEC_DEVICE(hk);
	}

#endif

	return (struct device *)NULL;
}

int free_device(struct device * dev)
{
	if(dev->alarmcallback)
		dev->alarmcallback(CALLBACK_TYPE_DEVICE_DELETED, NULL, &dev->alarmuserdata);

	if(dev->talkcallback)
		dev->talkcallback(CALLBACK_TYPE_DEVICE_DELETED, NULL, &dev->talkuserdata);

	free(dev);
	
	return 0;
}

int destroy_device(struct device *dev)
{
	struct channel *chn1, chn2;
	LIST_FOR_EACH_ENTRY_SAFE(chn1, chn2, &dev->channels, struct channel, entry)
	{
		struct stream *stm1, stm2;
		LIST_FOR_EACH_ENTRY_SAFE(stm1, stm2, &chn1->streams, struct stream, entry)
		{
			list_remove(&stm1->entry);
			free_stream(stm1);
		}
		
		list_remove(&chn1->entry);
		free_channel(chn1);
	}

	list_remove(&dev->entry);
	free_device(dev);

	return -1;
}

int device_init(struct device *dev)
{
	if(dev->alarmcallback)
		dev->alarmcallback(CALLBACK_TYPE_DEVICE_INIT, NULL, &dev->alarmuserdata);
	dev->alarmcallback = NULL;

	if(dev->talkcallback)
		dev->talkcallback(CALLBACK_TYPE_DEVICE_INIT, NULL, &dev->alarmuserdata);
	dev->talkcallback = NULL;

	memset(&dev->encodeinfo, 0, sizeof(dev->encodeinfo));//Çå³ý±àÂëÐÅÏ¢
	memset(dev->ip, 0, sizeof(dev->ip));
	dev->port = 0;
	memset(dev->user, 0, sizeof(dev->user));
	memset(dev->password, 0, sizeof(dev->password));

	dev->logined = 0;
	return 0;
}

struct device *add_device(struct device *dev)
{
	if(dev==NULL) 
	{
		jtprintf("[%s]dev==NULL\n", __FUNCTION__);
		return NULL;
	}
	
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		if(device == dev)
		{
			jtprintf("[%s]dev==dev %p\n", __FUNCTION__, device);
			assert(0);
		}
	}	

	list_add_tail(&devicelist, &dev->entry);

	return dev;
}

struct device *remove_device(struct device *dev)
{
	if(dev==NULL) 
	{
		jtprintf("[%s]dev==NULL\n", __FUNCTION__);
		return NULL;
	}
	
	struct device* cursor1, *cursor2;
	LIST_FOR_EACH_ENTRY_SAFE(cursor1, cursor2, &devicelist, struct device, entry)
	{
		if(cursor1 == dev)
		{
			cursor1->deleted = true;
			list_remove(&cursor1->entry);
		}
	}

	return dev;
}

struct device *get_device(struct device *dev)
{
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		if(device == dev)
		{	
			return dev;
		}
	}	

	return NULL;
}
struct device *get_device_by_address(char* ip, unsigned int port)
{
	assert(ip != NULL && strlen(ip)>6);

	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		if(strcmp(device->ip, ip)==0 && device->port==port)
		{	
			return device;
		}
	}

	return NULL;
}

struct device *get_device_by_channel(struct channel* chn)
{
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		struct channel *chn_;
		LIST_FOR_EACH_ENTRY(chn_, &device->channels, struct channel, entry)
		{
			if(chn_==chn)
				return device;
		}
	}

	return NULL;
}

struct device *get_device_by_stream(stream* stm)
{
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		struct channel *chn;
		LIST_FOR_EACH_ENTRY(chn, &device->channels, struct channel, entry)
		{
			struct stream *stream;
			LIST_FOR_EACH_ENTRY(stream, &chn->streams, struct stream, entry)
			{
				if(stm==stream)
				{
					//jtprintf("[%s]find device record, stm %p\n", __FUNCTION__, device);
					return device;
				}
			}
		}
	}

	return NULL;
}

void printf_device_encode_info(struct device *dev)
{
	for(int i=0; i<MAX_CHANNEL_ENCODE_INFO; ++i)
	{
		jtprintf("[%s]%d main, enable %d, encodetype %d, fps %d, width %d, height %d, quality %d, bitrate %d, bitratectl %d, gop %d\n", __FUNCTION__, i,
			dev->encodeinfo.ch_encode_info[i].mainencode.enable,
			dev->encodeinfo.ch_encode_info[i].mainencode.encodetype,
			dev->encodeinfo.ch_encode_info[i].mainencode.fps,
			dev->encodeinfo.ch_encode_info[i].mainencode.width,
			dev->encodeinfo.ch_encode_info[i].mainencode.height,
			dev->encodeinfo.ch_encode_info[i].mainencode.quality,
			dev->encodeinfo.ch_encode_info[i].mainencode.bitrate,
			dev->encodeinfo.ch_encode_info[i].mainencode.bitratectl,
			dev->encodeinfo.ch_encode_info[i].mainencode.gop);

		jtprintf("[%s]%d sub1, enable %d, encodetype %d, fps %d, width %d, height %d, quality %d, bitrate %d, bitratectl %d, gop %d\n", __FUNCTION__, i,
			dev->encodeinfo.ch_encode_info[i].sub1encode.enable,
			dev->encodeinfo.ch_encode_info[i].sub1encode.encodetype,
			dev->encodeinfo.ch_encode_info[i].sub1encode.fps,
			dev->encodeinfo.ch_encode_info[i].sub1encode.width,
			dev->encodeinfo.ch_encode_info[i].sub1encode.height,
			dev->encodeinfo.ch_encode_info[i].sub1encode.quality,
			dev->encodeinfo.ch_encode_info[i].sub1encode.bitrate,
			dev->encodeinfo.ch_encode_info[i].sub1encode.bitratectl,
			dev->encodeinfo.ch_encode_info[i].sub1encode.gop);

		jtprintf("[%s]%d audio, enable %d, encodetype %d, frequency %d, bitrate %d, channel %d, depth %d\n", __FUNCTION__, i, 
			dev->encodeinfo.ch_encode_info[i].audioencode.enable,
			dev->encodeinfo.ch_encode_info[i].audioencode.encodetype,
			dev->encodeinfo.ch_encode_info[i].audioencode.frequency,
			dev->encodeinfo.ch_encode_info[i].audioencode.bitrate,
			dev->encodeinfo.ch_encode_info[i].audioencode.channel,
			dev->encodeinfo.ch_encode_info[i].audioencode.depth);	
	}


}

struct channel *alloc_channel(size_t size)
{
    struct channel *chn = (struct channel *)calloc(1, size);
    if (chn)
    {
    	chn->obj.ref = 1;
		chn->obj.parent = NULL;
		chn->obj.type = OBJECT_TYPE_CHANNEL;
		chn->obj.name = (char*)"channel";
		chn->id	  = -1;
		list_init( &chn->entry);
        list_init( &chn->streams);

		chn->audiocallback = NULL;
		chn->audiouserdata = NULL;
        return chn;
    }
	
    return (struct channel *)NULL;
}

int free_channel(struct channel *chn)
{
	if(chn->audiocallback)
		chn->audiocallback(CALLBACK_TYPE_DEVICE_DELETED, NULL, &chn->audiouserdata);
	return 0;
}

struct channel* add_channel(struct device *dev, struct channel *newchn)
{
	struct channel* chn;

	LIST_FOR_EACH_ENTRY(chn, &dev->channels, struct channel, entry)
	{
		if(chn->id == newchn->id)
		{	
			jtprintf("[%s]find channel record %d\n", __FUNCTION__, chn->id);
			assert(0);
		}
	}

	jtprintf("[%s]add new channel record %d\n", __FUNCTION__, chn->id);

	newchn->obj.parent = (object*)dev;
	
	list_add_tail(&dev->channels, &newchn->entry);

	return newchn;
}

int channel_init(struct channel *chn)
{
	if(chn->audiocallback)
		chn->audiocallback(CALLBACK_TYPE_DEVICE_INIT, NULL, &chn->audiouserdata);
	chn->audiocallback = NULL;

	chn->insendtalkdata = 0;
	return 0;
}

struct channel* do_channel(struct list *channels, int chnid, operator_channel ope, int optype, void* data)
{
	struct channel *chn;
	if(channels==NULL)
	{
		return NULL;
	}

	LIST_FOR_EACH_ENTRY(chn, channels, struct channel, entry)
	{
		if(chn->id == chnid)
		{	
			ope(chn, optype, data);
			return chn;
		}
	}

	jtprintf("[%s]no channel record %d\n", __FUNCTION__, chnid);
	return NULL;
}
int do_each_channel(struct list *channels, operator_channel ope, int optype, void* data)
{
	struct channel *chn;
	assert(channels);

	LIST_FOR_EACH_ENTRY(chn, channels, struct channel, entry)
	{
		if(ope(chn, optype, data))
			break;
	}

	return 0;
}

struct channel* get_channel(struct list *channels, int chnid)
{
	struct channel *chn;
	
	if(channels==NULL)
	{
		return NULL;
	}

	LIST_FOR_EACH_ENTRY(chn, channels, struct channel, entry)
	{
		if(chn->id == chnid)
		{	
			jtprintf("[%s]find channel record %d, chn %p\n", __FUNCTION__, chnid, chn);
			return chn;
		}
	}

	jtprintf("[%s]no channel record %d\n", __FUNCTION__, chnid);
	return NULL;
}

struct stream *alloc_stream(size_t size, int streamid)
{
	jtprintf("[%s]\n", __FUNCTION__);
    struct stream *stm = (struct stream *)calloc(1, size);
    if (stm)
    {
    	memset(stm, 0, size);
		stm->obj.ref = 1;
		stm->obj.parent = 0;
		stm->obj.name = (char*)"stream";
		stm->obj.type = OBJECT_TYPE_STREAM;
		stm->id	  = streamid;
		stm->pulling = 0;
		list_init( &stm->entry);

		stm->callback = NULL;
		stm->userdata = NULL;

		init_singlebuf(&stm->videobuf);
		
        return stm;
    }
	
    return (struct stream *)NULL;
}

int free_stream(struct stream *stm)
{
	assert(stm);

	if(stream->videobuf.buf)
		free_singlebuf(&stream->videobuf);

	if(stm->callback)
		stm->callback(CALLBACK_TYPE_DEVICE_DELETED, NULL, &stm->userdata);

	free(stm);
	return 0;
}

struct stream *alloc_stream_with_videobuf(size_t size, int streamid, unsigned int len)
{
	struct stream *stm = alloc_stream(size, streamid);
	if(stm)
	{
		if(alloc_memory_to_singlebuf(&stm->videobuf, len)==0)
		{
			jtprintf("[%s]alloc_memory_to_singlebuf %u failed, \n", __FUNCTION__, len);
		}
	}

	jtprintf("[%s]ok, streamid %d, stm %p\n", __FUNCTION__, streamid, stm);
	
	return stm;
}

struct stream* add_stream(struct channel* channel, struct stream *newstm)
{
	struct stream* stm = NULL;
	LIST_FOR_EACH_ENTRY(stm, &channel->streams, struct stream, entry)
	{
		if(newstm->id == stm->id)
		{	
			//copy info to old???
			jtprintf("[%s]find stream record id %d\n", __FUNCTION__, stm->id);
			assert(0);
			return stm;
		}
	}

	jtprintf("[%s]add new stream record id %d, stm %p\n", __FUNCTION__, newstm->id, newstm);

	newstm->obj.parent = (object*)channel;

	list_add_tail(&channel->streams, &newstm->entry);

	return newstm;
}

int stream_init(struct stream *stm)
{
	stm->pulling = 0;
	if(stm->callback)
		stm->callback(CALLBACK_TYPE_DEVICE_INIT, NULL, &stm->userdata);
	stm->callback = NULL;

	clear_singlebuf(&stm->videobuf);

	stm->llbegintime = 0;

	return 0;
}

//add witch dev type???
struct stream* get_stream_by_dev(struct device *dev, struct stream* stm)
{
	struct channel *chn;
	LIST_FOR_EACH_ENTRY(chn, &dev->channels, struct channel, entry)
	{
		struct stream *stream;
		LIST_FOR_EACH_ENTRY(stream, &chn->streams, struct stream, entry)
		{
			jtprintf("[%s]find stream %p, stm %p\n", __FUNCTION__, stream, stm);
			if(stm==stream)
			{
				jtprintf("[%s]find stream record, stm %p\n", __FUNCTION__, stm);
				return stm;
			}
		}
	}

	return NULL;
}

struct stream* get_special_stream(struct list *streams, operator_stream ope, void* data)
{
	struct stream *stm = NULL;

	if(streams==NULL)
	{
		return NULL;
	}

	LIST_FOR_EACH_ENTRY(stm, streams, struct stream, entry)
	{
		if(ope(stm, data)!=0)
			return stm;
	}
	
	jtprintf("[%s]no stream record, data %p\n", __FUNCTION__, data);
						
	return NULL;
}

struct stream* get_stream(struct list *streams, struct stream* stm)
{
	struct stream *stream = NULL;
	if(streams==NULL)
	{
		return NULL;
	}

	LIST_FOR_EACH_ENTRY(stream, streams, struct stream, entry)
	{
		if(stm==stream)
		{
			jtprintf("[%s]find stream record, stm %p\n", __FUNCTION__, stm);
			return stm;
		}
	}
	
	jtprintf("[%s]no stream record, stm %p\n", __FUNCTION__, stm);
						
	return NULL;
}

struct stream* get_stream_by_id(struct list* streams, int stmid)
{
	struct stream *stm = NULL;

	if(streams==NULL)
	{
		return NULL;
	}

	LIST_FOR_EACH_ENTRY(stm, streams, struct stream, entry)
	{
		if(stm->id==stmid)
		{
			jtprintf("[%s]find stream record, stmid %d\n", __FUNCTION__, stmid);
			return stm;
		}
	}
	
	jtprintf("[%s]no stream record, stmid %d\n", __FUNCTION__, stmid);
						
	return NULL;
}





















