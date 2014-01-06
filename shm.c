#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>		 /* For mode constants */
#include <fcntl.h>			 /* For O_* constants */
#include <assert.h>
#include <errno.h>
#include "shm.h"

//open or create, default all permissions
int open_or_create_shm(const char *filename)
{
   assert(filename!=NULL);
   int handle=-1;
   while(1){
      handle = shm_open(filename, O_CREAT | O_RDWR, S_IRWXO | S_IRWXG | S_IRWXU);
      if(handle >= 0){
         fchmod(handle, S_IRWXO | S_IRWXG | S_IRWXU);
         break;
      }
      else if(errno == EEXIST){
	  	 printf("[open_or_create_shm]errno == EEXIST\n");
         if((handle = shm_open(filename, O_CREAT | O_RDWR, S_IRWXO | S_IRWXG | S_IRWXU)) >= 0 || errno != ENOENT){
            break;
         }
      }
   }

   return handle;
}

int open_shm(const char *filename)
{
	assert(filename!=NULL);
	return shm_open(filename,  O_CREAT | O_RDWR,  S_IRWXO | S_IRWXG | S_IRWXU);
}

int remove_shm(const char *filename)
{
	assert(filename!=NULL);
	return shm_unlink(filename);
}

















