#include "vsem.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <limits.h>
#include <stdio.h>

#define SEM_DEFAULT_DIR "/dev/shm/"

static key_t create_semkey(const char *filename, int create)
{
	key_t semkey = (key_t)-1;
	char SemPath[PATH_MAX] = SEM_DEFAULT_DIR;

	strcat(SemPath, filename);
	if(access(SemPath, F_OK) && create)//
	{
		if(create)
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
		else
		{
			perror("file not exist and not create it");
			return -1;
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
			
		}
		else if (errno == EEXIST)
		{
			if ((semid = semget(semkey, 0, 0)) == -1)
			{
				perror("IPC error 1: semget");
				return -1;
			}
		}
		else
		{
			perror("IPC error 2: semget"); 
			return -1;
		}
	}

	return semid;
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
	sb.sem_flg=SEM_UNDO ;
	if(semop(semid,&sb,1)==-1)
	{
		perror("post_vsem error"); 
		return -1;
	}

	return 0;
}
int wait_vsem(int semid)
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

