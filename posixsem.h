#ifndef POSIX_SEM_H
#define POSIX_SEM_H

#include <semaphore.h>

//�ź�������
int sem_create_jt(const char* name,unsigned int value);		//����һ���ź�������ָ����ֵ
sem_t *sem_open_jt(const char* Name);							//��һ���ź��������õ�ָ���ź�����ָ��
int sem_close_jt(sem_t * semPtr);								//�ر�һ�������ź���
int sem_unlink_jt(const char* name);							//ɾ��һ�������ź��� ����Ҫclose���ü���Ϊ0
int sem_getvalue_jt(sem_t * semPtr);							//�õ�һ�������ź����ĵ�ǰֵ ����ֵΪ-1��ʾ����
int sem_wait_jt(sem_t * semPtr);								//�ȵ�һ���ź���Ϊ���������ѵ�ǰ�ź�����һ
int sem_post_jt(sem_t * semPtr);								//�ڵ�ǰ���ź�����ֵ�ϼ�һ

//�����ڴ����
int shm_create_jt(const char* Name);							//����һ�����������ڴ�,����һ���豸��
int shm_open_jt(const char* Name,unsigned int Size,char*& Ptr);//��һ�����������ڴ棬,����һ���豸��
int shm_getsize_jt(unsigned int fd);							//�鿴һ�����������ڴ�Ĵ�С
int shm_trunc_jt(int fd,unsigned int size);					//�ı�һ����������ڴ�Ĵ�С
int shm_close_jt(int fd);										//�ر�һ�����������ڴ�
int shm_unlink_jt(int fd);										//ɾ��һ�����������ڴ�
int shm_release_jt(char* ptr, unsigned int size);				//


#endif
