#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "list.h"
#include "frontdevice.h"
#include "devicetype.h"

#include "xmmanager.h"

/* grab an object (i.e. increment its refcount) and return the object */

//lock
struct list devicelist;

struct object *grab_object( void *ptr )
{
    struct object *obj = (struct object *)ptr;
    assert( obj->ref < 0x7fffffff );
	__sync_add_and_fetch(&obj->ref,1);
    return obj;
}

/* release an object (i.e. decrement its refcount) */
void release_object( void *ptr )
{
    struct object *obj = (struct object *)ptr;
    assert( obj->ref);

	__sync_fetch_and_sub(&obj->ref,1);
}

struct device *_alloc_device( const struct device_ops *ops)
{
    struct device *dev = (struct device *)malloc(ops->size);
    if (dev)
    {
        dev->obj.ref = 1;
		dev->obj.type = OBJECT_TYPE_DEVICE;
		dev->obj.name = NULL;//name[ops->type];
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
		
        return dev;
    }
	
    return (struct device *)NULL;
}

struct device *alloc_device(unsigned int type)
{
	if(type==DEVICE_XM)
	{
		return (struct device *)xm_alloc_device();
	}

	return (struct device *)NULL;
}

struct device *add_device(struct device *dev)
{
	if(dev==NULL) 
	{
		printf("[%s]dev==NULL\n", __FUNCTION__);
		return NULL;
	}
	
	struct device* device;
	LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry)
	{
		if(device == dev)
		{	
			assert(0);
		}
	}	

	list_add_tail(&devicelist, &dev->entry);

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

struct channel *alloc_channel(size_t size)
{
    struct channel *chn = (struct channel *)malloc(size);
    if (chn)
    {
    	chn->obj.ref = 1;
		chn->obj.parent = NULL;
		chn->obj.type = OBJECT_TYPE_CHANNEL;
		chn->obj.name = (char*)"";
		chn->id	  = -1;
		list_init( &chn->entry);
        list_init( &chn->streams);

		chn->audiocallback = NULL;
		chn->audiouserdata = NULL;
        return chn;
    }
	
    return (struct channel *)NULL;
}

struct channel* add_channel(struct device *dev, struct channel *newchn)
{
	struct channel* chn;

	LIST_FOR_EACH_ENTRY(chn, &dev->channels, struct channel, entry)
	{
		if(chn->id == newchn->id)
		{	
			printf("[%s]find channel record %d\n", __FUNCTION__, chn->id);
			assert(0);
		}
	}

	printf("[%s]add new channel record %d\n", __FUNCTION__, chn->id);

	newchn->obj.parent = (object*)dev;
	
	list_add_tail(&dev->channels, &newchn->entry);

	return newchn;
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

	printf("[%s]no channel record %d\n", __FUNCTION__, chnid);
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
			printf("[%s]find channel record %d\n", __FUNCTION__, chnid);
			return chn;
		}
	}

	printf("[%s]no channel record %d\n", __FUNCTION__, chnid);
	return NULL;
}


struct stream *alloc_stream(size_t size)
{
	printf("[%s]\n", __FUNCTION__);
    struct stream *stm = (struct stream *)malloc(size);
    if (stm)
    {
    	memset(stm, 0, size);
		stm->obj.ref = 1;
		stm->obj.parent = 0;
		stm->obj.name = (char*)"";
		stm->obj.type = OBJECT_TYPE_STREAM;
		stm->id	  = -1;
		stm->pulling = 0;
		list_init( &stm->entry);

		stm->callback = NULL;
		stm->userdata = NULL;
        return stm;
    }
	
    return (struct stream *)NULL;
}

struct stream* add_stream(struct channel* channel, struct stream *newstm)
{
	struct stream* stm = NULL;
	LIST_FOR_EACH_ENTRY(stm, &channel->streams, struct stream, entry)
	{
		if(newstm->id == stm->id)
		{	
			//copy info to old???
			printf("[%s]find stream record %d\n", __FUNCTION__, stm->id);
			assert(0);
			return stm;
		}
	}

	printf("[%s]add new stream record %d\n", __FUNCTION__, stm->id);

	newstm->obj.parent = (object*)channel;

	list_add_tail(&channel->streams, &newstm->entry);

	return newstm;
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
			printf("[%s]find stream %p, stm %p\n", __FUNCTION__, stream, stm);
			if(stm==stream)
			{
				printf("[%s]find stream record, stm %p\n", __FUNCTION__, stm);
				return stm;
			}
		}
	}

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
			printf("[%s]find stream record, stm %p\n", __FUNCTION__, stm);
			return stm;
		}
	}
	
	printf("[%s]no stream record, stm %p\n", __FUNCTION__, stm);
						
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
			printf("[%s]find stream record, stmid %d\n", __FUNCTION__, stmid);
			return stm;
		}
	}
	
	printf("[%s]no stream record, stmid %d\n", __FUNCTION__, stmid);
						
	return NULL;
}


















