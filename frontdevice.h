#ifndef FRONTDEVICE_H
#define FRONTDEVICE_H

#include "devicetype.h"

#include "command.h"
#include "list.h"
#include "plugerror.h"

#include "seansinglebuf.h"



#ifdef __cplusplus
extern "C"
{
#endif


struct object
{
    unsigned int        ref;    /* reference count */
	int                 type;
	char*               name;
	struct object*      parent;
};

struct device_ops;

struct audio_info 
{
	int enable;
	int encodetype;
	int frequency;
	int bitrate;
	int channel;
	int depth;
};

struct channel_encode_info
{
	int ch_last_get_time;//如果可以一次只能获取一个, 用这个
	struct encode_info mainencode;
	struct encode_info sub1encode;
	struct audio_info  audioencode;
};

#define MAX_CHANNEL_ENCODE_INFO 32
struct dev_encode_info
{	
	int last_get_time;//如果可以一次获取所有,用这个
	struct channel_encode_info ch_encode_info[MAX_CHANNEL_ENCODE_INFO];
};

struct device
{
	struct object             obj;
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

	jt_stream_callback        alarmcallback;//报警的回调
	void*                     alarmuserdata;//报警的用户数据	

	jt_talk_callback          talkcallback;
	void*					  talkuserdata;
	unsigned long int         talkthreadid;
		
	struct dev_encode_info    encodeinfo;
	int                       logined;
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
	int  (*set_config)(struct device *, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
	int  (*open_alarm_stream)(struct device *, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
	int  (*close_alarm_stream)(struct device *, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
	int  (*ptz_control)(struct device *, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
	int  (*set_system_time)(struct device *, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);
	int  (*start_talk)(struct device *, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp);
	int  (*stop_talk)(struct device *, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp);
	int  (*send_talk_data)(struct device *, stSendTalkData_Req *req, stSendTalkData_Rsp *rsp);

	//int  (*capture_picture)(struct device *, char *req, unsigned long req);
	//
};

struct channel
{
	struct object   obj;
	int             id;
	struct list     entry;
	struct list     streams;

	jt_stream_callback    audiocallback;//音频的回调
	void*                 audiouserdata;//音频的用户数据

	int insendtalkdata;
};

struct stream
{
	struct object        obj;
	int                  id;
	int                  pulling;
	struct list          entry;

	char                 get_first_i;
	unsigned long long   llbegintime;//开始时间, 100ns
	jt_stream_callback   callback;//视频的回调
	void*                userdata;//视频的用户数据

	struct stsinglebufinfo     videobuf;//是否放到每个具体的流去，因为有的用不着

	void*                decoder;  //
	jt_stream_callback   callback2;//视频的回调	
};

struct device_debug
{
	int d;
};

typedef int (*real_staream_callback)(struct stream* stm, const void *pFrame, unsigned int user);

int gloal_init();

int unique_lock_devicelist(const char *FunName);
int share_lock_devicelist(const char* FunName);
int unlock_devicelist(const char* FunName);

struct device* alloc_device(unsigned int type);
struct device *_alloc_device( const struct device_ops *ops);//放到ops里去
struct device *add_device(struct device *dev);
int device_init(struct device *dev);//
struct device *remove_device(struct device *dev);
int destroy_device(struct device * device);

struct device *get_device(struct device *dev);
struct device *get_device_by_address(char* ip, unsigned int port);//老的接口没有提供增加设备的命令，就直接登陆了，所以要检查是否有同ip，port的设备了
struct device *get_device_by_channel(struct channel* chn);
struct device *get_device_by_stream(struct stream* stm);

void printf_device_encode_info(struct device *dev);

struct channel *alloc_channel(size_t size);
struct channel* add_channel(struct device *dev, struct channel *newchn);
int channel_init(struct channel *chn);
struct channel* get_channel(struct list *channels, int chnid);

#define START_AUDIO         1
#define STOP_AUDIO          2
#define CHECK_AUDIO_CHANNEL 3
typedef int (*operator_channel)(struct channel *chn, int optype, void* data);
struct channel* do_channel(struct list *channels, int chnid, operator_channel ope, int optype, void* data);
int do_each_channel(struct list *channels, operator_channel ope, int optype, void* data);

struct stream *alloc_stream(size_t size, int streamid);
struct stream *alloc_stream_with_videobuf(size_t size, int streamid, unsigned int len);

struct stream* add_stream(struct channel* channel, struct stream *newstm);
int stream_init(struct stream *stm);
struct stream* get_stream(struct list *streams, struct stream* stm);
struct stream* get_stream_by_id(struct list *streams, int stmid);
struct stream* get_stream_by_dev(struct device *dev, struct stream* stm);

typedef int (*operator_stream)(struct stream *stm, void* data);
struct stream* get_special_stream(struct list *streams, operator_stream ope, void* data);

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


#ifdef __cplusplus
}
#endif

#endif
