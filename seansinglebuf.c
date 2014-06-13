#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32 //windows platform

#else

#include <unistd.h>

#endif
#include "jtprintf.h"
#include "seansinglebuf.h"

#define EXTRA_ALLOC_LEN 1024

int init_singlebuf(struct stsinglebufinfo *bufinfo)
{
	bufinfo->buf = NULL;
	bufinfo->len = 0;
	bufinfo->bufusedlen = 0;
	return 0;
}

int clear_singlebuf(struct stsinglebufinfo *bufinfo)
{
	bufinfo->bufusedlen = 0;
	return 0;
}

unsigned int alloc_memory_to_singlebuf(struct stsinglebufinfo *bufinfo, unsigned int len)
{
	bufinfo->buf = (unsigned char*)malloc(len);
	if(bufinfo->buf)
	{
		bufinfo->len = len;
		bufinfo->bufusedlen = 0;
	}
	else
	{
		jtprintf("[%s]malloc stsinglebufinfo failed\n", __FUNCTION__);
		return 0;
	}

	return bufinfo->len;
}

unsigned int free_singlebuf(struct stsinglebufinfo *bufinfo)
{
	bufinfo->len = 0;
	if(bufinfo->buf)
		free(bufinfo->buf);
	bufinfo->buf = NULL;
	bufinfo->bufusedlen = 0;
	return 0;
}

unsigned int check_singlebuf(struct stsinglebufinfo *bufinfo, unsigned int needlen)
{
	if((bufinfo->len - bufinfo->bufusedlen) < needlen)
	{
		bufinfo->buf = (unsigned char*)realloc(bufinfo->buf, bufinfo->bufusedlen + needlen + EXTRA_ALLOC_LEN);
		if(bufinfo->buf==NULL)
		{
			bufinfo->len = 0;
			bufinfo->bufusedlen = 0;
			return 0;
		}
		
		bufinfo->len = needlen+EXTRA_ALLOC_LEN;
	}

	int unusedlen = bufinfo->len - bufinfo->bufusedlen;
	if(unusedlen<0)
	{
		jtprintf("[%s]unusedlen %d, bufinfo->len %d, bufinfo->bufusedlen\n"
			, __FUNCTION__, unusedlen, bufinfo->len, bufinfo->bufusedlen);
	}
		
	assert(unusedlen>=0);
	return unusedlen;
	
}

unsigned int write_to_singlebuf(struct stsinglebufinfo *bufinfo, unsigned char* data, unsigned int datalen)
{
	if(check_singlebuf(bufinfo, datalen) < datalen)
	{	
		assert(0);
		return 0;
	}

	memcpy(bufinfo->buf + bufinfo->bufusedlen, data, datalen);
	bufinfo->bufusedlen += datalen;

	assert(bufinfo->bufusedlen<=bufinfo->len);
	
	return datalen;
}

//返回读出的长度
unsigned int read_from_singlebuf(struct stsinglebufinfo *bufinfo, unsigned char** data, unsigned int *datalen)
{
	*data = bufinfo->buf;
	*datalen = bufinfo->bufusedlen;

	unsigned int temp = bufinfo->bufusedlen;

	bufinfo->bufusedlen = 0;
	
	return temp;
}




