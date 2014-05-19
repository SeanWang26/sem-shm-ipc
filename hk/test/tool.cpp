#include "tool.h"
#include <stdio.h>
#include <string.h>

int Demo_DVRIPByResolveSvr()
{
	//char *pDevSerial = "DS-9116HF-ST1620111226BBRR406437740WCVU";
	char *pDevSerial = "DS-9016HF-ST1620110524BBRR403574653WCVU";
	int iDevSerialLen = strlen(pDevSerial);

	char *pDevName = NULL;//"DS-9016HF-ST1620110524BBRR403574";
	int iDevNameLen = 0;//strlen(pDevName);

	char strIP[16] = {0};
	DWORD sPort = 0;

	if (NET_DVR_GetDVRIPByResolveSvr_EX("172.9.6.91", 7071, (BYTE *)pDevName, iDevNameLen, (BYTE *)pDevSerial, iDevSerialLen, strIP, &sPort))
	{
		printf("ip:%s, port:%d\n", strIP, sPort);
	}
	else
	{
		printf("NET_DVR_GetDVRIPByResolveSvr_EX fail.\n");
	}

	return 0;
}


