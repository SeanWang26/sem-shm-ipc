#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#include "posixsem.h"

typedef enum
{
    SEM_CREAT = 1,
    SEM_OPEN ,
    SEM_CLOSE,
    SEM_UNLINK,
    SEM_GETVALUE,
    SEM_WAIT,
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
		printf("error handle=%p\n", handle);
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
					printf("error SEM_GETVALUE semPtr=%lld\n", DCALL.SemReq.Ptr);
					return FALSE;
				}

				if( 0 != sem_post((sem_t *)DCALL.SemReq.Ptr))//warning: cast to pointer from integer of different size
				{
					perror("FindClose 6");
					return FALSE;
				}

				break;
			}
			case SEM_CREAT:
			{
				sem_t* sem;
				if(DCALL.SemReq.Name == NULL)
				{
					printf("error SEM_CREAT semName=0\n");
					return FALSE;
				}

				sem = sem_open((const char *)DCALL.SemReq.Name, O_CREAT, 0777, DCALL.SemReq.Value);
				if(SEM_FAILED == sem )
				{
					perror("SEM_CREATE\n");
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
					printf("error SEM_OPEN semName=0\n");
					return FALSE;
				}
				
				sem = sem_open((const char *)DCALL.SemReq.Name, 0);
				if(SEM_FAILED == sem )
				{
					perror("SEM_OPEN--------- 1");
					perror((const char *)DCALL.SemReq.Name);
					perror("SEM_OPENc 1");
					return FALSE;
				}

				DCALL.SemReq.Ptr = (long long)sem;//warning: cast from pointer to integer of different size
				break;
			}
			case SEM_CLOSE:
			{
				if(DCALL.SemReq.Ptr == 0 || DCALL.SemReq.Ptr == -1)
				{
					printf("error SEM_CLOSE semPtr=%lld\n", DCALL.SemReq.Ptr);
					return FALSE;
				}

				if( 0 != sem_close((sem_t *)DCALL.SemReq.Ptr))
				{
					perror("SEM_CLOSE 2");
					return FALSE;
				}

				break;
			}
			case SEM_WAIT:
			{
				int res=0;
				if(DCALL.SemReq.Ptr == 0 || DCALL.SemReq.Ptr == -1)
				{
					printf("error SEM_WAIT semPtr=%lld\n", DCALL.SemReq.Ptr);
					return FALSE;
				}

				while ((res = sem_wait((sem_t *)DCALL.SemReq.Ptr)) == -1 && errno == EINTR)
					continue;

				if( 0 != res)
				{
					perror("SEM_WAIT 5");
					return FALSE;
				}

				break;
			}
			case SEM_UNLINK:
			{
				if(0 != sem_unlink((const char *)DCALL.SemReq.Name))
				{
					perror("SEM_UNLINK 3");
					return FALSE;
				}				
				
				break;
			}
			case SEM_GETVALUE:
			{
				if(DCALL.SemReq.Ptr == 0 || DCALL.SemReq.Ptr == -1)
				{
					printf("error SEM_GETVALUE semPtr=%lld\n", DCALL.SemReq.Ptr);
					return FALSE;
				}
				if( 0 != sem_getvalue((sem_t *)DCALL.SemReq.Ptr, &DCALL.SemReq.Value))
				{
					perror("SEM_GETVALUE 4");
					return FALSE;
				}

				break;
			}
			case SEM_UNKOWN:
			default:
				printf("FindClose SEM_UNKOWN\n");
				return FALSE;
		}

		//�ź��������������
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
					perror("SHM_CREAT 7");
					return FALSE;
				}

				break;
			}
			case SHM_OPEN:
			{
				
				DCALL.ShmReq.Fd = shm_open((const char *)DCALL.ShmReq.Name, O_RDWR, 0777);
				if (DCALL.ShmReq.Fd<0)
				{
					perror("SHM_OPEN 8");
					return FALSE;
				}

				ftruncate(DCALL.ShmReq.Fd, DCALL.ShmReq.Size);
				DCALL.ShmReq.Ptr = (char*)mmap(NULL, DCALL.ShmReq.Size, PROT_READ | PROT_WRITE, MAP_SHARED, DCALL.ShmReq.Fd, 0);
				if (DCALL.ShmReq.Ptr  == MAP_FAILED)
				{
					perror("SHM_OPEN 9");
					return FALSE;
				}

				break;
			}
			case SHM_UNLINK:
			{                
				if (shm_unlink((const char *)DCALL.ShmReq.Name)<0)
				{
					perror("SHM_UNLINK");
					return FALSE;
				}
			
				break;
			}
			case SHM_CLOSE:
			{
				if (close(DCALL.ShmReq.Fd)<0)
				{
					printf("SHM_CLOSE error, fd %d", DCALL.ShmReq.Fd);
					return FALSE;
				}
				break;
			}
			case SHM_TRUN:
			{
				if (ftruncate(DCALL.ShmReq.Fd, DCALL.ShmReq.Size)<0)
				{
				   perror("SHM_TRUN 10");
				   return FALSE;
				}
			
				break;				
			}
			case SHM_RELEASE:
			{
				if (munmap(DCALL.ShmReq.Ptr, DCALL.ShmReq.Size)<0)
			    {
					perror("SHM_RELEASE");
					return FALSE;
				}
	            break;
			}
			case SHM_UNKOWN:
			default:
				printf("FindClose SHM_UNKOWN\n");
				return FALSE;
		}

		//�����ڴ沿���������
		return TRUE;
	}
	else
	{
		
	}

	printf("FindClose never here\n");
	return FALSE;
}

//����һ���ź�������ָ����ֵ
int sem_create_jt(const char* Name,unsigned int Value)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_CREAT;
	call.SemReq.Name = (char*)Name;
	call.SemReq.Value = Value;
	return FindClose((HANDLE)&call);
}

//��һ���ź��������õ�ָ���ź�����ָ��
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

//�ر�һ�������ź���
int sem_close_jt(sem_t * semPtr)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_CLOSE;
	call.SemReq.Ptr = (long long)semPtr;

	return FindClose((HANDLE)&call);
}

//ɾ��һ�������ź��� ����Ҫclose���ü���Ϊ0
int  sem_unlink_jt(const char* Name)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_UNLINK;
	call.SemReq.Name = (char*)Name;

	return FindClose((HANDLE)&call);
}

//�õ�һ�������ź����ĵ�ǰֵ ����ֵΪ-1��ʾ����
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

//�ȵ�һ���ź���Ϊ���������ѵ�ǰ�ź�����һ
int sem_wait_jt(sem_t * semPtr)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_WAIT;
	call.SemReq.Ptr = (long long)semPtr;

	return FindClose((HANDLE)&call);
}


//�ڵ�ǰ���ź�����ֵ�ϼ�һ
int sem_post_jt(sem_t * semPtr)
{
	LinuxDirectCall call;
	call.SemReq.Head.Type = SEM_Type;
	call.SemReq.Command = SEM_POST;
	call.SemReq.Ptr = (long long)semPtr;

	return FindClose((HANDLE)&call);
}

//�����ڴ����
//����һ�����������ڴ�,����һ���豸��
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

//��һ�����������ڴ棬,����һ���豸��
int shm_open_jt(const char* Name,unsigned int Size,char*& Ptr)
{
	LinuxDirectCall call;
	call.ShmReq.Head.Type = SHM_Type;
	call.ShmReq.Command = SHM_OPEN;
	call.ShmReq.Name = (char*)Name;
	call.ShmReq.Size = Size;
	
	if (!FindClose((HANDLE)&call))///ȥ�� if
	{
		Ptr=NULL;
		return -1;
	}

	Ptr = call.ShmReq.Ptr;
	return  call.ShmReq.Fd;
}

//�鿴һ�����������ڴ�Ĵ�С
int shm_getsize(unsigned int fd)
{


	return  0;
}

//�ı�һ����������ڴ�Ĵ�С
int shm_trunc_jt(int Fd,unsigned int Size)
{
 	LinuxDirectCall call;
	call.ShmReq.Head.Type = SHM_Type;
	call.ShmReq.Command = SHM_TRUN;
	call.ShmReq.Fd = Fd;
	call.ShmReq.Size = Size;
	return FindClose((HANDLE)&call);
}

//�ر�һ�����������ڴ�
int shm_close_jt(int Fd)
{
 	LinuxDirectCall call;
	call.ShmReq.Head.Type = SHM_Type;
	call.ShmReq.Command = SHM_CLOSE;
	call.ShmReq.Fd = Fd;
	return FindClose((HANDLE)&call);
}

//ɾ��һ�����������ڴ�
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

