
struct front_ops
{
    void (*init)(struct device *,int);
    int (*login)(struct device *);
    int  (*logout)(struct device *, struct wait_queue_entry *);
	int  (*open_video_stream)(struct device *,struct wait_queue_entry *);
	int  (*close_video_stream)(struct device *,struct wait_queue_entry *);
	int  (*open_audio_stream)(struct device *,struct wait_queue_entry *);
	int  (*close_audio_stream)(struct device *,struct wait_queue_entry *);
};

struct device
{
    unsigned int              refcount;    /* reference count */
    const struct front_ops  *ops;
};















