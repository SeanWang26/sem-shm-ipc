#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "jtprintf.h"


#include "posixsem.h"

typedef enum
{
    SEM_CREAT = 1,
    SEM_OPEN ,
    SEM_CLOSE,
    SEM_UNLINK,
    SEM_GETVALUE,
    SEM_WAIT,
    SEM_WAIT_TIMEOUT,
    SEM_POST,
    SEM_UNKOWN
    
}enSemCommand;

typedef enum
{
    SHM_CREAT = 1,
    SHM_OPEN ,
    SHM_UNLINK,
    SHM_CLOSE,
    SHM_TRUN,
   	SHM_RELEASE,
    SHM_UNKOWN
    
}enShmCommand;

typedef enum
{
	SEM_Type = 1,
	SHM_Type ,
	Type_UNKOWN
}enCommand;

struct DirectCallHead
{
	enCommand		Type;
};

struct stSemReq
{
	struct DirectCallHead	Head;
	enSemCommand	Command;
	char *			Name;
	int				Value;
	long long		Ptr;
};

struct stShmReq
{
	struct DirectCallHead		Head;
	enShmCommand		Command;
	char*				Name;
	unsigned int		Size;
	char*				Ptr;
	int					Fd;
};

union LinuxDirectCall
{
	struct DirectCallHead		Head;
	struct stSemReq				SemReq; 
	struct stShmReq				ShmReq;
};

#define DCALL (*call)

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef HANDLE
#define HANDLE void*
#endif

static int FindClose(HANDLE handle)
{
	union LinuxDirectCall* call = (union LinuxDirectCall*)handle;

	if(handle == (void*)0 || handle == (void*)-1)
	{
		jtprintf("[%s]error handle=%p\n", __FUNCTION__, handle);
		return FALSE;
	}
	
	if(SEM_Type==DCALL.Head.Type)
	{
		switch(DCALL.SemReq.Command)
		{
			case SEM_POST:
			{
				if(DCALL.SemReq.Ptr == 0 || DCALL.SemReq.Ptr == -1)
				{
					jtprintf("[%s]SEM_POST error, semPtr=%lld\n", __FUNCTION__, DCALL.SemReq.Ptr);
					return FALSE;
				}

				if( 0 != sem_post((sem_t *)DCALL.SemReq.Ptr))//warning: cast to pointer from integer of different size
				{
					jtprintf("[%s]SEM_POST error, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}

				break;
			}
			case SEM_CREAT:
			{
				sem_t* sem;
				if(DCALL.SemReq.Name == NULL)
				{
					jtprintf("[%s]SEM_CREAT error, DCALL.SemReq.Name == NULL\n", __FUNCTION__);
					return FALSE;
				}

				sem = sem_open((const char *)DCALL.SemReq.Name, O_CREAT, 0777, DCALL.SemReq.Value);
				if(SEM_FAILED == sem )
				{
					jtprintf("[%s]SEM_CREATE, error %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}
				DCALL.SemReq.Ptr = (long long)sem;	//warning: cast from pointer to integer of different size
				break;
			}
			case SEM_OPEN:
			{
				sem_t * sem;
				if(DCALL.SemReq.Name == NULL)
				{
					jtprintf("[%s]SEM_OPEN error DCALL.SemReq.Name == NULL\n", __FUNCTION__);
					return FALSE;
				}
				
				sem = sem_open((const char *)DCALL.SemReq.Name, 0);
				if(SEM_FAILED == sem )
				{
					jtprintf("[%s]SEM_OPEN error, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}

				DCALL.SemReq.Ptr = (long long)sem;//warning: cast from pointer to integer of different size
				break;
			}
			case SEM_CLOSE:
			{
				if(DCALL.SemReq.Ptr == 0 || DCALL.SemReq.Ptr == -1)
				{
					jtprintf("[%s]SEM_CLOSE error, semPtr=%lld\n", __FUNCTION__, DCALL.SemReq.Ptr);
					return FALSE;
				}

				if( 0 != sem_close((sem_t *)DCALL.SemReq.Ptr))
				{
					jtprintf("[%s]SEM_CLOSE error, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}

				break;
			}
			case SEM_WAIT:
			{
				int res=0;
				if(DCALL.SemReq.Ptr == 0 || DCALL.SemReq.Ptr == -1)
				{
					jtprintf("[%s]SEM_WAIT error, semPtr=%lld\n", __FUNCTION__, DCALL.SemReq.Ptr);
					return FALSE;
				}

				//int sem_timedwait((sem_t *)DCALL.SemReq.Ptr), const struct timespec *restrict abs_timeout);

				while ((res = sem_wait((sem_t *)DCALL.SemReq.Ptr)) == -1 && errno == EINTR)//
					continue;

				if( 0 != res)
				{
					jtprintf("[%s]SEM_WAIT error, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}

				break;
			}
			case SEM_WAIT_TIMEOUT:
			{
				int res=0;
				if(DCALL.SemReq.Ptr == 0 || DCALL.SemReq.Ptr == -1)
				{
					jtprintf("[%s]SEM_WAIT error, semPtr=%lld\n", __FUNCTION__, DCALL.SemReq.Ptr);
					return FALSE;
				}

				struct timespec stTS;	
				memset(&stTS,0,sizeof(struct timespec));
				clock_gettime(CLOCK_REALTIME, &stTS);
				stTS.tv_sec += DCALL.SemReq.Value;
				while ((res = sem_timedwait((sem_t *)DCALL.SemReq.Ptr, &stTS)) == -1 && errno == EINTR)//改为有timeout的那个
					continue;

				if( 0 != res)
				{
					if(res == ETIMEDOUT)
					{
						jtprintf("[%s]SEM_WAIT timeout, %s\n", __FUNCTION__, strerror(errno));
					}
					else
					{
						jtprintf("[%s]SEM_WAIT error, %s\n", __FUNCTION__, strerror(errno));
					}
					
					return FALSE;
				}

				break;
			}
			case SEM_UNLINK:
			{
				if(0 != sem_unlink((const char *)DCALL.SemReq.Name))
				{
					jtprintf("[%s]SEM_UNLINK 3, error %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}
				
				break;
			}
			case SEM_GETVALUE:
			{
				if(DCALL.SemReq.Ptr == 0 || DCALL.SemReq.Ptr == -1)
				{
					jtprintf("[%s]SEM_GETVALUE error, semPtr=%lld\n", __FUNCTION__, DCALL.SemReq.Ptr);
					return FALSE;
				}
				if( 0 != sem_getvalue((sem_t *)DCALL.SemReq.Ptr, &DCALL.SemReq.Value))
				{
					jtprintf("[%s]SEM_GETVALUE error, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}

				break;
			}
			case SEM_UNKOWN:
			default:
				jtprintf("[%s]FindClose SEM_UNKOWN\n", __FUNCTION__);
				return FALSE;
		}

		//信号量部分在这结束
		return TRUE;
	}
	else if(SHM_Type==DCALL.Head.Type)
	{
		switch(DCALL.ShmReq.Command)
		{
			case SHM_CREAT:
			{
				DCALL.ShmReq.Fd = shm_open((const char *)DCALL.ShmReq.Name, O_CREAT | O_EXCL , 0777);
				if (DCALL.ShmReq.Fd<0)
				{
					jtprintf("[%s]SHM_CREAT error, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}

				break;
			}
			case SHM_OPEN:
			{
				DCALL.ShmReq.Fd = shm_open((const char *)DCALL.ShmReq.Name, O_RDWR, 0777);
				if (DCALL.ShmReq.Fd<0)
				{
					jtprintf("[%s]SHM_OPEN error, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}

				ftruncate(DCALL.ShmReq.Fd, DCALL.ShmReq.Size);
				DCALL.ShmReq.Ptr = (char*)mmap(NULL, DCALL.ShmReq.Size, PROT_READ | PROT_WRITE, MAP_SHARED, DCALL.ShmReq.Fd, 0);
				if (DCALL.ShmReq.Ptr  == MAP_FAILED)
				{
					jtprintf("[%s]SHM_OPEN mmap MAP_FAILED, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}

				break;
			}
			case SHM_UNLINK:
			{                
				if (shm_unlink((const char *)DCALL.ShmReq.Name)<0)
				{
					jtprintf("[%s]SHM_UNLINK error, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}
			
				break;
			}
			case SHM_CLOSE:
			{
				if (close(DCALL.ShmReq.Fd)<0)
				{
					jtprintf("[%s]SHM_CLOSE error, fd %d, %s\n", __FUNCTION__, DCALL.ShmReq.Fd, strerror(errno));
					return FALSE;
				}
				break;
			}
			case SHM_TRUN:
			{
				if (ftruncate(DCALL.ShmReq.Fd, DCALL.ShmReq.Size)<0)
				{
				   jtprintf("[%s]SHM_TRUN error, %s\n", __FUNCTION__, strerror(errno));
				   return FALSE;
				}
			
				break;				
			}
			case SHM_RELEASE:
			{
				if (munmap(DCALL.ShmReq.Ptr, DCALL.ShmReq.Size)<0)
			    {
					jtprintf("[%s]SHM_RELEASE error, %s\n", __FUNCTION__, strerror(errno));
					return FALSE;
				}
	            break;
			}
			case SHM_UNKOWN:
			default:
				jtprintf("[%s]FindClose SHM_UNKOWN\n", __FUNCTION__);
				return FALSE;
		}

		//共享内存部分在这结束
		return TRUE;
	}
	else
	{
		
	}

	printf("[%s]FindClose never here\n", __FUNCTION__);
	return FALSE;
}

//创建一个信号量，并指定初值
int sem_create_jt(const char* Name,unsigned int Value)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_CREAT;
	call.SemReq.Name = (char*)Name;
	call.SemReq.Value = Value;
	return FindClose((HANDLE)&call);
}

//打开一个信号量，并得到指向信号量的指针
sem_t * sem_open_jt(const char* Name)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_OPEN;
	call.SemReq.Name = (char*)Name;

	if (!FindClose((HANDLE)&call))
	{
		return (sem_t*)-1;
	}

	return  (sem_t*)call.SemReq.Ptr;
}

//关闭一个具名信号量
int sem_close_jt(sem_t * semPtr)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_CLOSE;
	call.SemReq.Ptr = (long long)semPtr;

	return FindClose((HANDLE)&call);
}

//删除一个具名信号量 比如要close引用计数为0
int  sem_unlink_jt(const char* Name)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_UNLINK;
	call.SemReq.Name = (char*)Name;

	return FindClose((HANDLE)&call);
}

//得到一个具名信号量的当前值 返回值为-1表示错误
int sem_getvalue_jt(sem_t * semPtr)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_GETVALUE;
	call.SemReq.Ptr = (long long)semPtr;
	if (!FindClose((HANDLE)&call))
	{
		return -1;
	}

	return call.SemReq.Value;
}

//等到一个信号量为正数，并把当前信号量减一
int sem_wait_jt(sem_t * semPtr)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_WAIT;
	call.SemReq.Ptr = (long long)semPtr;

	return FindClose((HANDLE)&call);
}


//在当前的信号量的值上加一
int sem_post_jt(sem_t * semPtr)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_POST;
	call.SemReq.Ptr = (long long)semPtr;

	return FindClose((HANDLE)&call);
}

//共享内存操作
//创建一个具名共享内存,返回一个设备号
int shm_create_jt(const char* Name)
{
	LinuxDirectCall call;
	call.ShmReq.Head.Type = SHM_Type;
	call.ShmReq.Command = SHM_CREAT;
	call.ShmReq.Name = (char*)Name;
	call.ShmReq.Size = 0;
  	if (!FindClose((HANDLE)&call))
	{
		return -1;
	}

	return call.ShmReq.Fd;
}

//打开一个具名共享内存，,返回一个设备号
int shm_open_jt(const char* Name,unsigned int Size,char*& Ptr)
{
	LinuxDirectCall call;
	call.ShmReq.Head.Type = SHM_Type;
	call.ShmReq.Command = SHM_OPEN;
	call.ShmReq.Name = (char*)Name;
	call.ShmReq.Size = Size;
	
	if (!FindClose((HANDLE)&call))///去掉 if
	{
		Ptr=NULL;
		return -1;
	}

	Ptr = call.ShmReq.Ptr;
	return  call.ShmReq.Fd;
}

//查看一个具名共享内存的大小
int shm_getsize(unsigned int fd)
{


	return  0;
}

//改变一个共享具名内存的大小
int shm_trunc_jt(int Fd,unsigned int Size)
{
 	LinuxDirectCall call;
	call.ShmReq.Head.Type = SHM_Type;
	call.ShmReq.Command = SHM_TRUN;
	call.ShmReq.Fd = Fd;
	call.ShmReq.Size = Size;
	return FindClose((HANDLE)&call);
}

//关闭一个具名共享内存
int shm_close_jt(int Fd)
{
 	LinuxDirectCall call;
	call.ShmReq.Head.Type = SHM_Type;
	call.ShmReq.Command = SHM_CLOSE;
	call.ShmReq.Fd = Fd;
	return FindClose((HANDLE)&call);
}

//删除一个具名共享内存
int shm_unlink_jt(int Fd)
{
 	LinuxDirectCall call;
	call.ShmReq.Head.Type = SHM_Type;
	call.ShmReq.Command = SHM_UNLINK;
	call.ShmReq.Fd = Fd;
	return FindClose((HANDLE)&call);
}

int shm_release_jt(char* Ptr, unsigned int Size)
{
  	LinuxDirectCall call;
	call.ShmReq.Head.Type = SHM_Type;
	call.ShmReq.Command = SHM_RELEASE;
	call.ShmReq.Ptr = Ptr;
	call.ShmReq.Size = Size;
	return FindClose((HANDLE)&call);
}

