#ifndef SEAN_H264_UTILITY
#define SEAN_H264_UTILITY

//图像参考序列
inline int ispictureseq(unsigned char byte)
{
	return (0x1f&byte)==0x07;
}
//帧参考序列
inline int isframeseq(unsigned char byte)
{
	return (0x1f&byte)==0x08;
}
//立即刷新单元
inline int isrefrencefream(unsigned char byte)
{
	return (0x1f&byte)==0x05;
}

//解析图像增强单元
static int sn_paser_efi(char *indata, size_t inlen, char** outdata, int *outlen)
{
	unsigned char efi_type = 0;
	unsigned char efi_payload = 0;

	efi_type = indata[5];
	efi_payload = indata[6];

	*outdata = indata + efi_payload + 7 + 1;  //  1 is rbsp_trailing_bits
	*outlen = inlen - efi_payload - 7 - 1;    //  1 is rbsp_trailing_bits

	return *outlen;//返回剩下的长度
}



#endif
