#ifndef SEMSHMIPCUTILITY_H
#define SEMSHMIPCUTILITY_H

#include <string>

using namespace std;

enum SemShmIpcMode
{
	AsMaster,
	AsSlave,
};

class SemShmIpcSet
{
private:
	string m_ShmName;
	string m_SemInterName;
	string m_SemCrossName;
	
	int m_Fd;
	char* m_DataPtr;
	int   m_DataSize;
	
	sem_t *m_SemCross; 
	sem_t *m_SemInter;

	SemShmIpcMode m_Mode;
public:
	SemShmIpcSet();
	~SemShmIpcSet();
	int Init(SemShmIpcMode Mode, const string& SemInterName, const string& SemCrossName, const string& ShmName, int ShmLen);
	int GetData(unsigned char** data, int *len);
	int SetData(unsigned char** data, int *len);
	int Wait(int Timeout);
	int Post();
};






























#endif
