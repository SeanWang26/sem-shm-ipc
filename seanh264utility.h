#ifndef SEAN_H264_UTILITY
#define SEAN_H264_UTILITY

//ͼ��ο�����
inline int ispictureseq(unsigned char byte)
{
	return (0x1f&byte)==0x07;
}
//֡�ο�����
inline int isframeseq(unsigned char byte)
{
	return (0x1f&byte)==0x08;
}
//����ˢ�µ�Ԫ
inline int isrefrencefream(unsigned char byte)
{
	return (0x1f&byte)==0x05;
}

//���ʵ�Ԫ�ָ���
inline int is_access_unit_delimiter_sean(unsigned char byte)
{
	return byte==0x09;
}

//������ǿ��Ϣ��Ԫ��SEI��
inline int is_SEI_sean(unsigned char byte)
{
	return byte==0x06;
}

//������Ϣ��ǿ��Ԫ����
static int paser_efi_sean(char *indata, size_t inlen, char** outdata, int *outlen)
{
	unsigned char efi_type = 0;
	unsigned char efi_payload = 0;

	efi_type = indata[5];
	efi_payload = indata[6];

	*outdata = indata + efi_payload + 7 + 1;  //  1 is rbsp_trailing_bits
	*outlen = inlen - efi_payload - 7 - 1;    //  1 is rbsp_trailing_bits

	return *outlen;//����ʣ�µĳ���
}



#endif
