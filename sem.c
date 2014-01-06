#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>			 /* For O_* constants */
#include <sys/stat.h>		 /* For mode constants */
#include <string.h>

#include "sem.h"

sem_t* open_or_create_sem(const char *filename)
{
	return sem_open(filename, O_CREAT , S_IRWXO | S_IRWXG | S_IRWXU, 0);//O_EXCL
}

sem_t* open_sem(const char *filename)
{
	return sem_open(filename, 0);
}

int close_sem(sem_t* handle)
{
	return sem_close(handle);
}

int remove_sem(const char * filename)
{
	if(sem_unlink(filename))
	{
		printf("[remove_sem]errno %d, %s\n", errno, strerror(errno));
		return -1;
	}

	return 0;
}
