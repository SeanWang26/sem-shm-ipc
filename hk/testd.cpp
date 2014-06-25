#include "../PaserPsStream.h"
#include <stdio.h>
#include  <assert.h>


int psconvertorawh264(char* path)
{
	FILE* fpin = fopen(path, "rb+");
	FILE* fpout = fopen("hksy.out", "wb+");

	char readbuf[1024*1024*2] = {0};

	int readlen=0;
	int readlentotal=0;
	int cnt = 0;
	while((readlen = fread(readbuf,1,sizeof(struct pack_start_code),fpin))>0)
	{
		readlentotal += readlen;
		printf("cnt%d, fread new readlen %d\n", cnt, readlen);
		
		++cnt;
		struct pack_start_code *statecode = (struct pack_start_code *)readbuf;
		
		if(statecode->start_code[0]==0x00
			&& statecode->start_code[1]==0x00 
			&& statecode->start_code[2]==0x01)
		{
			if(statecode->stream_id[0]==0xba)
			{
				printf("cnt%d, 0xba\n", cnt);
				
				readlen += fread(readbuf+readlen, 1, 10, fpin);
				program_stream_pack_header *header = (program_stream_pack_header *)readbuf;
				readlentotal += readlen;
 
				unsigned char stuffinglen = header->stuffinglen & '\x07';
				if(stuffinglen) readlentotal += fread(readbuf,1, stuffinglen,fpin);

				printf("cnt%d, 0xba header->stuffinglen(0x%x) & \'\\x07\': %u\n", cnt, header->stuffinglen, stuffinglen);
			}
			else if(statecode->stream_id[0]==0xbb)//
			{
				printf("cnt%d, 0xbb, sizeof(struct st_ps_system_header)=%u\n", cnt, sizeof(struct st_ps_system_header));
				
				readlen += fread(readbuf+readlen, 1, sizeof(struct st_ps_system_header)-sizeof(struct pack_start_code), fpin);


				struct st_ps_system_header *systemhead = (struct st_ps_system_header *)readbuf;

				printf("11111cnt%d, 0xbb, %x, %x, %x, %x, %x, %x, readlen %d\n"
					, cnt
					, readbuf[0], readbuf[1], readbuf[2], readbuf[3], readbuf[4], readbuf[5]
					, readlen);
				
				unsigned char templen = systemhead->header_length.byte[0];
				systemhead->header_length.byte[0] = systemhead->header_length.byte[1];
				systemhead->header_length.byte[1] = templen;

				int readed = fread(readbuf+readlen, 1, systemhead->header_length.length-(sizeof(struct st_ps_system_header)-sizeof(union littel_endian_size)-sizeof(struct pack_start_code)), fpin);

				printf("cnt%d, 0xbb, readed %d\n", cnt, readed);
			}
			else //if(statecode->stream_id[0]!=0xba )
			{
				unsigned char stacode = statecode->stream_id[0];

				printf("22222cnt%d, stacode 0x%02x\n", cnt, stacode);

				
				readlen += fread(readbuf+readlen , 1, 5 ,fpin);
		
				struct program_stream_e *pesstart = (struct program_stream_e *)readbuf;

				printf("33333cnt%d, byte[0]%02x, byte[1]%02x\n"
					, cnt, pesstart->PackLength.byte[0], pesstart->PackLength.byte[1]);

				char temp = pesstart->PackLength.byte[0];
				pesstart->PackLength.byte[0] = pesstart->PackLength.byte[1];
				pesstart->PackLength.byte[1] = temp;

				printf("44444cnt%d, byte[0]%02x, byte[1]%02x\n"
					, cnt, pesstart->PackLength.byte[0], pesstart->PackLength.byte[1]);
				
				//pesstart->PackLength.byte[0] = pesstart->PackLength.byte[0] ^ pesstart->PackLength.byte[1];
				//pesstart->PackLength.byte[1] = pesstart->PackLength.byte[0] ^ pesstart->PackLength.byte[1];
				//pesstart->PackLength.byte[0] = pesstart->PackLength.byte[0] ^ pesstart->PackLength.byte[1];
				
				readlen+=fread(readbuf+readlen , 1, pesstart->stuffing_length, fpin);
				int payload = pesstart->PackLength.length - 3 - pesstart->stuffing_length;

				printf("cnt%d, rpesstart->stuffing_length(%d), pesstart->PackLength.length(%d)\n"
					, cnt, pesstart->stuffing_length, pesstart->PackLength.length);
				
				readlen=fread(readbuf , 1, payload, fpin);
				if(readlen!=payload)
				{
					printf("readlen(%d)!=payload(%d)\n", readlen, payload);
				}

				printf("cnt%d, %02x, %02x, %02x, %02x, %02x, %02x, %02x, \n"
					, cnt, readbuf[0], readbuf[1], readbuf[2]
					, readbuf[3], readbuf[4], readbuf[5], readbuf[6]);
						
				
				if(stacode==0xe0)
				{	
					static int dd = 0;
				
					printf("555555cnt%d, 0xe0, %d\n", cnt, ++dd);
					static int firsti=0;
					if(firsti==0)
					{
						if(readbuf[0]==0x00
							&&readbuf[1]==0x00
							&&readbuf[2]==0x00
							&&readbuf[3]==0x01
							&&readbuf[4]==0x67)
						{
							firsti=1;
						}
					}

					if(firsti)
						fwrite(readbuf, 1, readlen, fpout);
				}
				else if(stacode==0xbc)
				{
					printf("cnt%d, 0xbc\n", cnt);
				}
				else if(stacode==0xc0)
				{
					printf("cnt%d, 0xc0\n", cnt);
				}
				else
				{
					printf("cnt%d, unknown id %x\n", cnt, stacode);
					assert(0);
				}
				
			}
		}
		else
		{
			printf("start code error %x, %x, %x\n"
				, statecode->start_code[0], statecode->start_code[1], statecode->start_code[20]);
			assert(0);
		}
	}
	
	fclose(fpin);
	fclose(fpout);

	return 0;
}


int main(int argc, char** argv)
{
	printf("main\n");
	
	psconvertorawh264(argv[1]);

	return 0;
}
