#ifndef VSEM_H
#define VSEM_H
#include <time.h>

int open_or_create_vsem(const char *filename);
int open_vsem(const char *filename);
int post_vsem(int semid);
int wait_vsem(int semid);
int waittimeout_vsem(int semid, struct timespec *timeout);
int close_vsem(int semid);
int remove_vsem(const char * filename);

#endif
