#ifndef FRONTDEVICE_H
#define FRONTDEVICE_H

#include "command.h"
#include "list.h"
#include "plugerror.h"
#include "devicetype.h"

#define OBJECT_TYPE_DEVICE	1
#define OBJECT_TYPE_CHANNEL	2
#define OBJECT_TYPE_INPUT	3
#define OBJECT_TYPE_OUTPUT	4
#define OBJECT_TYPE_STREAM	5

struct object
{
    unsigned int        ref;    /* reference count */
	int                 type;
	char*               name;
	struct object*      parent;
};

struct device_ops;

struct device
{
	object                    obj;
	const struct device_ops*  ops;
	int                       dev_type;
	char                      ip[32];
	unsigned short            port;
	char					  user[32];
	char					  password[32];
	struct list               entry;
	int                       deleted;
	//lock					lock;//锁住list用????
	struct list               channels;
	struct list               inputs;
	struct list               outputs;
};

struct device_ops
{
	size_t size;
	unsigned int type;
    int  (*init)(struct device *);
	int  (*uninit)(struct device *);
    int  (*login)(struct device *, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
    int  (*logout)(struct device *, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
	int  (*open_video_stream)(struct device *, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
	int  (*close_video_stream)(struct device *, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
	int  (*open_audio_stream)(struct device *, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
	int  (*close_audio_stream)(struct device *, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
	int  (*get_config)(struct device *, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
	int  (*set_config)(struct device *);
	int  (*open_alarm_stream)(struct device *, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
	int  (*close_alarm_stream)(struct device *, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Req *rsp);
	int  (*ptz_control)(struct device *, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
};

struct channel
{
	object          obj;
	int             id;
	struct list     entry;
	struct list     streams;
};

struct st_stream_data;
typedef int (*stream_callback)(struct st_stream_data* data, void* user);

struct stream
{
	object               obj;
	int                  id;
	int                  pulling;
	struct channel*      chn;
	struct list          entry;
	stream_callback      callback;
	void*                userdata;
};

struct device_debug
{
	
};

typedef int (*real_staream_callback)(stream* stm, const void *pFrame, unsigned int user);

struct device* alloc_device(unsigned int type);
struct device *_alloc_device( const struct device_ops *ops);//放到ops里去
struct device *add_device(struct device *dev);
struct device *get_device(struct device *dev);
struct device *get_device_by_address(struct device *dev, char* ip, unsigned int port);//老的接口没有提供增加设备的命令，就直接登陆了，所以要检查是否有同ip，port的设备了

struct channel *alloc_channel(size_t size);
struct channel* add_channel(struct device *dev, struct channel *newchn);
struct channel* get_channel(struct list *channels, int chnid);

struct stream *alloc_stream(size_t size);
struct stream* add_stream(struct channel* channel, struct stream *newstm);
struct stream* get_stream(struct list *streams, int stmid);

extern struct list devicelist;

#define FIND_DEVICE_BEGIN(ctype,opstype)    struct device* device; 												\
	  										LIST_FOR_EACH_ENTRY(device, &devicelist, struct device, entry) \
	  										{ 																\
	  											assert(device->obj.type == OBJECT_TYPE_DEVICE); 			\
	  											if(!device->deleted && device->ops->type == opstype )			 \
	  											{ \
	  												ctype* dev = (ctype*)device; \
									


#define FIND_DEVICE_END    		} \
							}


struct st_stream_data
{
	unsigned int streamtype;
	char* pdate;
	int   datalen;
	union 
	{
		struct
		{
			unsigned int encode;
			unsigned int frametype;
			unsigned int width;
			unsigned int height;
			float        fps;
			unsigned int bitrate;
		}video_stream_info;
		struct
		{
			unsigned int  encode;
			unsigned char channel;
			int           samples;
			unsigned char depth;
			unsigned int  reserved;//bitrate,  bps
		}audio_stream_info;
		struct
		{
			int           reason;
		}alarm_stream_info;
	}stream_info;
	int year;					// 时标:年		
	int month; 					// 时标:月
	int day;					// 时标:日
	int hour;					// 时标:时
	int minute;					// 时标:分
	int second;					// 时标:秒

	unsigned int reserved[3];
};

#endif
