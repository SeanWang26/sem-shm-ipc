#ifndef SEANSINGLEBUF_H
#define SEANSINGLEBUF_H

struct stsinglebufinfo
{
	unsigned int         len;
	unsigned char*       buf;
	unsigned int         bufusedlen;
};

//�ֶ�����
int init_singlebuf(struct stsinglebufinfo *bufinfo);

//���ط���ĳ���
unsigned int alloc_memory_to_singlebuf(struct stsinglebufinfo *bufinfo, unsigned int len);

//�ͷ��ڴ�
unsigned int free_singlebuf(struct stsinglebufinfo *bufinfo);

//�������
int clear_singlebuf(struct stsinglebufinfo *bufinfo);

//���ܻ�����µ��ڴ棬����δ�õ��ڴ�Ĵ�С
unsigned int check_singlebuf(struct stsinglebufinfo *bufinfo, unsigned int needlen);

//д��
unsigned int write_to_singlebuf(struct stsinglebufinfo *bufinfo, unsigned char* data, unsigned int datalen);

//��ȡbuf������ݣ����ض�ȡ���ĳ���
unsigned int read_from_singlebuf(struct stsinglebufinfo *bufinfo, unsigned char** data, unsigned int *datalen);





















#endif
