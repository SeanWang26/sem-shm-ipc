#ifndef SEANSINGLEBUF_H
#define SEANSINGLEBUF_H

struct stsinglebufinfo
{
	unsigned int         len;
	unsigned char*       buf;
	unsigned int         bufusedlen;
};

//字段置零
int init_singlebuf(struct stsinglebufinfo *bufinfo);

//返回分配的长度
unsigned int alloc_memory_to_singlebuf(struct stsinglebufinfo *bufinfo, unsigned int len);

//释放内存
unsigned int free_singlebuf(struct stsinglebufinfo *bufinfo);

//清除数据
int clear_singlebuf(struct stsinglebufinfo *bufinfo);

//可能会分配新的内存，返回未用的内存的大小
unsigned int check_singlebuf(struct stsinglebufinfo *bufinfo, unsigned int needlen);

//写入
unsigned int write_to_singlebuf(struct stsinglebufinfo *bufinfo, unsigned char* data, unsigned int datalen);

//读取buf里的数据，返回读取到的长度
unsigned int read_from_singlebuf(struct stsinglebufinfo *bufinfo, unsigned char** data, unsigned int *datalen);





















#endif
