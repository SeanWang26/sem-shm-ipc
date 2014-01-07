#ifndef SHM_H
#define SHM_H

int open_or_create_shm(const char *filename);
int open_shm(const char *filename);
int remove_shm(const char *filename);

#endif
