#ifndef POSIX_SEM_H
#define POSIX_SEM_H

#include <semaphore.h>

//信号量操作
int sem_create_jt(const char* name,unsigned int value);		//创建一个信号量，并指定初值
sem_t *sem_open_jt(const char* Name);							//打开一个信号量，并得到指向信号量的指针
int sem_close_jt(sem_t * semPtr);								//关闭一个具名信号量
int sem_unlink_jt(const char* name);							//删除一个具名信号量 比如要close引用计数为0
int sem_getvalue_jt(sem_t * semPtr);							//得到一个具名信号量的当前值 返回值为-1表示错误
int sem_wait_jt(sem_t * semPtr);								//等到一个信号量为正数，并把当前信号量减一
int sem_post_jt(sem_t * semPtr);								//在当前的信号量的值上加一

//共享内存操作
int shm_create_jt(const char* Name);							//创建一个具名共享内存,返回一个设备号
int shm_open_jt(const char* Name,unsigned int Size,char*& Ptr);//打开一个具名共享内存，,返回一个设备号
int shm_getsize_jt(unsigned int fd);							//查看一个具名共享内存的大小
int shm_trunc_jt(int fd,unsigned int size);					//改变一个共享具名内存的大小
int shm_close_jt(int fd);										//关闭一个具名共享内存
int shm_unlink_jt(int fd);										//删除一个具名共享内存
int shm_release_jt(char* ptr, unsigned int size);				//


#endif
