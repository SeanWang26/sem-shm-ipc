#include "frontdevice.h"
#include "usersdk.h"
#include "jtprintf.h"

int CALL_TYPE jt_init(void* data)
{
	return gloal_init();
}

void* CALL_TYPE jt_create_device(unsigned int type)
{
	//没有就创建一个
	struct device* dev = (struct device*)alloc_device(type);

	unique_lock_devicelist();
 
	if(dev && add_device(dev))
	{
		jtprintf("add_device %d\n", type);
	}

	unlock_devicelist();

	return dev;
}
int CALL_TYPE jt_distory_device(void* handle)
{
	unique_lock_devicelist();
	struct device *dev = remove_device((struct device *)handle);
	unlock_devicelist();
	if(dev)
	{
		struct stLogout_Req req; 
		struct stLogout_Rsp rsp;
		dev->ops->logout(dev, &req, &rsp);
		free_device(dev);
	}
	return 0;
}

int CALL_TYPE jt_login(void* handle, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	share_lock_devicelist();
	struct device* dev = get_device((struct device*)handle); 
	if(dev && SUCCESS==dev->ops->login(dev, req, rsp))
	{
		rsp->DeviceHandle = (long long)((unsigned long)dev);
		unlock_devicelist();
		return 0;
	}
	unlock_devicelist();
	return -1;
}
int CALL_TYPE jt_login_ex(void* handle, int type, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	if(handle==NULL && type)
	{
		unique_lock_devicelist();
		struct device* dev = get_device_by_address(req->Ip, req->Port);
		if(dev==NULL)
		{
			//没有就创建一个
			dev = (struct device*)alloc_device(type);
			if(dev && add_device(dev))
			{
				jtprintf("add_device %s, %d\n", req->Ip, req->Port);
			}
		}
		unlock_devicelist();
	}

	share_lock_devicelist();
	struct device* dev = get_device((struct device*)handle); 
	if(dev)
	{
		int res = dev->ops->login(dev, req, rsp);	
		if(SUCCESS==res)
		{
			rsp->DeviceHandle = (long long)((unsigned long)dev);
			
			unlock_devicelist();
			return 0;
		}
		else
		{
			unlock_devicelist();
			return res;
		}
	}
	else
	{
		unlock_devicelist();
		return DEVICE_NO_FOUND;
	}

	unlock_devicelist();
	return -1;
}

int CALL_TYPE jt_logout(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	share_lock_devicelist();

	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->logout(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}

int CALL_TYPE jt_logout_ex(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	unique_lock_devicelist();

	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		remove_device(dev);
		
		unlock_devicelist();
		
		int res = dev->ops->logout(dev, req, rsp);

		free_device(dev);

		return res;
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}

int CALL_TYPE jt_open_video_stream(void* handle, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp)
{
	share_lock_devicelist();
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->open_video_stream(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}
int CALL_TYPE jt_close_video_stream(void* handle, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp)
{
	//not safe
	jtprintf("[jt_close_video_stream]enter\n");
	//get_dev_by_stream
	//struct stream* stm = (struct stream* )handle;
	//struct channel* chn =(struct channel*) stm->obj.parent;
	//struct device* dev = (struct device*)chn->obj.parent;
	
	struct device* dev = get_device_by_stream((struct stream*)handle);

	share_lock_devicelist();
	dev = get_device((struct device *)dev);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->close_video_stream(dev, req, rsp);
	}
	else
	{
		jtprintf("[jt_close_video_stream]not find dev %p\n", dev);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}
int CALL_TYPE jt_open_audio_stream(void* handle, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp)
{
	//grab_devcie?
	share_lock_devicelist();
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->open_audio_stream(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}
int CALL_TYPE jt_close_audio_stream(void* handle, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp)
{
	struct channel* chn = (struct channel*)handle;
	struct device* dev =(struct device*) chn->obj.parent;					

	share_lock_devicelist();
	dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->close_audio_stream(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}
int CALL_TYPE jt_get_config(void* handle, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp)
{
	share_lock_devicelist();
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->get_config(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}
int CALL_TYPE jt_set_config(void* handle, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp)
{
	share_lock_devicelist();
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->set_config(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}
int CALL_TYPE jt_open_alarm_stream(void* handle, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp)
{
	share_lock_devicelist();
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->open_alarm_stream(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}
int CALL_TYPE jt_close_alarm_stream(void* handle, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp)
{
	share_lock_devicelist();
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->close_alarm_stream(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}
int CALL_TYPE jt_ptz_control(void* handle, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp)
{
	share_lock_devicelist();
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->ptz_control(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}
int CALL_TYPE jt_set_system_time(void* handle, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp)
{
	share_lock_devicelist();
	
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist();
		return dev->ops->set_system_time(dev, req, rsp);
	}

	unlock_devicelist();
	return DEVICE_NO_FOUND;
}











