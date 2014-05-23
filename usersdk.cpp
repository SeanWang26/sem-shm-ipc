#include "frontdevice.h"
#include "usersdk.h"
#include "jtprintf.h"

int JT_CALL_TYPE jt_init(void* data)
{
	return gloal_init();
}

void* JT_CALL_TYPE jt_create_device(unsigned int type)
{
	//没有就创建一个
	struct device* dev = (struct device*)alloc_device(type);

	unique_lock_devicelist(__FUNCTION__);
 
	if(dev && add_device(dev))
	{
		jtprintf("[jt_create_device]add_device %d\n", type);
	}

	unlock_devicelist(__FUNCTION__);

	return dev;
}
int JT_CALL_TYPE jt_distory_device(void* handle)
{
	unique_lock_devicelist(__FUNCTION__);
	struct device *dev = remove_device((struct device *)handle);
	unlock_devicelist(__FUNCTION__);
	if(dev)
	{
		struct stLogout_Req req; 
		struct stLogout_Rsp rsp;
		dev->ops->logout(dev, &req, &rsp);
		free_device(dev);
	}
	return 0;
}

int JT_CALL_TYPE jt_login(void* handle, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	struct device* dev = get_device((struct device*)handle); 
	if(dev && SUCCESS==dev->ops->login(dev, req, rsp))
	{
		rsp->DeviceHandle = (long long)((unsigned long)dev);
		unlock_devicelist(__FUNCTION__);
		return 0;
	}
	unlock_devicelist(__FUNCTION__);
	return -1;
}
int JT_CALL_TYPE jt_login_ex(void* handle, int type, struct stLogin_Req *req, struct stLogin_Rsp *rsp)
{
	if(handle==NULL && type)
	{
		unique_lock_devicelist(__FUNCTION__);
		struct device* dev = get_device_by_address(req->Ip, req->Port);
		if(dev==NULL)
		{
			//没有就创建一个
			dev = (struct device*)alloc_device(type);
			if(dev && add_device(dev))
			{
				jtprintf("[jt_login_ex]add_device %s, %d\n", req->Ip, req->Port);
			}
		}

		handle = dev;
		
		unlock_devicelist(__FUNCTION__);
	}
	
	share_lock_devicelist(__FUNCTION__);
	struct device* dev = get_device((struct device*)handle); 
	if(dev)
	{
		int res = dev->ops->login(dev, req, rsp);	
		if(SUCCESS==res)
		{
			rsp->DeviceHandle = (long long)((unsigned long)dev);
		}
		
		unlock_devicelist(__FUNCTION__);
		return res;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;

}

int JT_CALL_TYPE jt_logout(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);

	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		unlock_devicelist(__FUNCTION__);
		return dev->ops->logout(dev, req, rsp);
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}

int JT_CALL_TYPE jt_logout_ex(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp)
{
	unique_lock_devicelist(__FUNCTION__);

	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		remove_device(dev);
		
		unlock_devicelist(__FUNCTION__);
		
		int res = dev->ops->logout(dev, req, rsp);

		free_device(dev);

		return res;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}

int JT_CALL_TYPE jt_open_video_stream(void* handle, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->open_video_stream(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}

	jtprintf("[jt_open_video_stream]DEVICE_NO_FOUND\n");
	
	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_close_video_stream(void* handle, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp)
{
	//not safe
	jtprintf("[jt_close_video_stream]enter\n");
	//get_dev_by_stream
	//struct stream* stm = (struct stream* )handle;
	//struct channel* chn =(struct channel*) stm->obj.parent;
	//struct device* dev = (struct device*)chn->obj.parent;
	
	share_lock_devicelist(__FUNCTION__);

	struct device* dev = get_device_by_stream((struct stream*)handle);
	//dev = get_device((struct device *)dev);
	if(dev)
	{
		int res = dev->ops->close_video_stream(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}
	else
	{
		jtprintf("[jt_close_video_stream]not find dev %p\n", dev);
		//unlock_devicelist(__FUNCTION__);
		//return SUCCESS;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_open_audio_stream(void* handle, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->open_audio_stream(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_close_audio_stream(void* handle, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp)
{
	//jtprintf("[jt_close_audio_stream]jt_close_audio_stream chn %p, dev %p\n", chn, dev);
	struct channel* chn = (struct channel*)handle;
	
	share_lock_devicelist(__FUNCTION__);
	struct device* dev = get_device_by_channel(chn);
	//dev = get_device((struct device *)dev);
	if(dev)
	{
		int res = dev->ops->close_audio_stream(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}
	else
	{
		jtprintf("[jt_close_audio_stream]not find dev %p\n", dev);
		//unlock_devicelist(__FUNCTION__);
		//return SUCCESS;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_get_config(void* handle, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->get_config(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_set_config(void* handle, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->set_config(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_open_alarm_stream(void* handle, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->open_alarm_stream(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_close_alarm_stream(void* handle, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->close_alarm_stream(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}
	else
	{
		jtprintf("[jt_close_alarm_stream]not find dev %p\n", dev);
		//unlock_devicelist(__FUNCTION__);
		//return SUCCESS;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_ptz_control(void* handle, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->ptz_control(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}
	else
	{
		jtprintf("[jt_close_alarm_stream]not find dev %p\n", dev);
		//unlock_devicelist(__FUNCTION__);
		//return SUCCESS;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_set_system_time(void* handle, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->set_system_time(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_start_talk(void* handle, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->start_talk(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}
int JT_CALL_TYPE jt_stop_talk(void* handle, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp)
{
	share_lock_devicelist(__FUNCTION__);
	
	struct device *dev = get_device((struct device *)handle);
	if(dev)
	{
		int res = dev->ops->stop_talk(dev, req, rsp);
		unlock_devicelist(__FUNCTION__);
		return res;
	}

	unlock_devicelist(__FUNCTION__);
	return DEVICE_NO_FOUND;
}












