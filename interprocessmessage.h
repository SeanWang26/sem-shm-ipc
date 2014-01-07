#include <unistd.h>
#include <semaphore.h>

struct ipcmsg_handle
{
	char name[64];
	sem_t* inter_sem_handle : NULL;
	sem_t* cross_sem_handle : NULL;
	int shm_fd : -1;
	void* shm_ptr : NULL;
	int   shm_size : 0;
};

struct shm_mem_head
{
	unsigned int ref : 0;
	unsigned char ctrl: 0;//
}

ipcmsg_handle* create_interprocess_msg(struct ipcmsg_handle* handle)
{
	

}


































