#include "vsem.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/shm.h>

#define SEM_DEFAULT_DIR "/dev/shm/"
//#define SEM_DEFAULT_DIR "/dev/shm/"

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};

//i need a file lock to lock the create?

#define IF_NOT_EXIST_CREATE 1
#define IF_NOT_EXIST_NOT_CREATE 2
#define IF_EXIST_DELETE_RECREATE 3

static key_t create_semkey(const char *filename, int create)
{
	key_t semkey = (key_t)-1;
	char SemPath[PATH_MAX] = SEM_DEFAULT_DIR;

	strcat(SemPath, filename);

	if(access(SemPath, F_OK))//
	{
		//not exist
		if(create==IF_NOT_EXIST_CREATE)
		{
			perror("file not exist then create it");
			//create file
			int fd = open(SemPath, O_CREAT|IPC_EXCL);
			if(fd < 0)
			{	
				if(EEXIST!=errno)
				{
					perror("file create error"); 
					return -1;
				}
				perror("create but file exist"); 
			}
			else 
				close(fd);
		}
		else if(create==IF_NOT_EXIST_NOT_CREATE)
		{
			perror("file not exist, not create it");
			return -1;
		}
	}
	else
	{
		//exist
		if(create==IF_EXIST_DELETE_RECREATE)
		{	
			perror("file exist, re create it");
			int cnt = 0;
			int res = -1;
			while(res && cnt < 5)
			{
				res = unlink(SemPath);

				//if(res && EBUSY==errno) 
  				//{
				//	sleep(0);
				//	++cnt;
				//	continue;
				//}
			}

			if(res) return -1;

			int fd = open(SemPath, O_CREAT|IPC_EXCL);
			if(fd < 0)
			{	
				if(EEXIST!=errno)
				{
					perror("file create error"); 
					return -1;
				}
				perror("create but file exist"); 
			}
			else 
				close(fd);
			
		}
	}

	if ((semkey = ftok(SemPath, 'a')) == (key_t) -1)
	{
		perror("IPC error: ftok"); 
		return -1;
	}
	
	return semkey;
}

int open_or_create_vsem(const char *filename)
{
	int semid = -1;
	key_t semkey = create_semkey(filename, 1);
	if(semkey==-1)
	{	
		return -1;
	}

	if ((semid = semget(semkey, 0, 0)) == -1) 
	{
		/* Semaphore does not exist - Create. */
		//change 0 too large number to try?? what will happen??
		if ((semid = semget(semkey, 1, IPC_CREAT | IPC_EXCL | S_IRUSR |
			S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) != -1)
		{
			union semun arg;
			arg.val=0;
			semctl(semid, 0, SETVAL, arg);
		}
		else if (errno == EEXIST)
		{
			if ((semid = semget(semkey, 0, 0)) == -1)
			{
				perror("IPC error 1: semget");
				return -1;
			}
			
			//union semun arg;
			//arg.val=0;
			//semctl(semid, 0, SETVAL, arg);
		}
		else
		{
			perror("IPC error 2: semget"); 
			return -1;
		}
	}

	printf("[open_or_create_vsem]getvalue_vsem %d\n", getvalue_vsem(semid));
	
	return semid;
}


//如遇同名的先删除再创建
int create_vsem(const char *filename)
{
	int semid = -1;
	key_t semkey = create_semkey(filename, IF_EXIST_DELETE_RECREATE);
	if(semkey==-1)
	{	
		return -1;
	}

	
}

int open_vsem(const char *filename)
{
	//int semid = -1;
	key_t semkey = create_semkey(filename, 0);
	if(semkey==-1)
	{	
		return -1;
	}

	return semget(semkey, 0, 0);
}

int post_vsem(int semid)
{
	struct sembuf sb; 
	sb.sem_num=0;
	sb.sem_op=1;
	sb.sem_flg=0;//SEM_UNDO
	if(semop(semid,&sb,1)==-1)
	{	
		printf("[post_vsem]getvalue_vsem %d\n", getvalue_vsem(semid));
		perror("[post_vsem]error"); 
		return -1;
	}

	return 0;
}
int wait_vsem(int semid)
{
	struct sembuf sb; 
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=0;//SEM_UNDO
	if(semop(semid,&sb,1)==-1)
	{	
		printf("[wait_vsem]getvalue_vsem %d\n", getvalue_vsem(semid));
		perror("[wait_vsem] error"); 
		return -1;
	}

	return 0;
}

int waittimeout_vsem(int semid, struct timespec *timeout)
{
	struct sembuf sb; 
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=SEM_UNDO ;
	if(semop(semid,&sb,1)==-1)
	{
		perror("wait_vsem error"); 
		return -1;
	}

	return 0;
}

int getvalue_vsem(int semid)
{
	return semctl(semid, 0, GETVAL, 0);
}

int close_vsem(int semid)
{
	return semctl(semid, 0, IPC_RMID);
}

int remove_vsem(const char * filename)
{
	char SemPath[PATH_MAX] = SEM_DEFAULT_DIR;

	strcat(SemPath, filename);
	if(access(SemPath, F_OK)==-1)
	{	
		printf("[remove_vsem]not exist %s\n", SemPath); 
		return 0;
	}

	unlink(SemPath);
	return 0;
}

