#include <semaphore.h>

sem_t* open_or_create_sem(const char *filename);
sem_t* open_sem(const char *filename);
int close_sem(sem_t* handle);
int remove_sem(const char * filename);

