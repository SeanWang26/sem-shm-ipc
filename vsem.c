#include "vsem.h"

int open_or_create_vsem(const char *filename)
{
	char Path[MAX_PATH] = {'/', 'd', 'e', 'v', '/', 's', 'h', 'm', '/', '\0'};
	strcat(Path, filename);
	if(access(Path, F_OK))
	{
		//create file
		int fd = open(Path, O_CREAT, );
		if(fd < 0)
		{
			return -1;
		}
	}

	
}
int open_vsem(const char *filename)
{

}
int close_vsem(int semid)
{

}
int remove_vsem(const char * filename)
{

}




























