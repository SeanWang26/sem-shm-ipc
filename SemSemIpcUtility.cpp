#include "posixsem.h"
#include "jtprintf.h"
#include "posixsem.h"
#include "SemSemIpcUtility.h"

SemShmIpcSet::SemShmIpcSet()
{}
SemShmIpcSet::~SemShmIpcSet()
{
	if(m_Fd >= 0)
	{
		shm_close_jt(m_Fd);
		shm_release_jt(m_DataPtr, m_DataSize);
	}

	if(m_SemCross != (sem_t *)-1)
	{
		sem_close_jt(m_SemCross);
	}

	if(m_SemInter != (sem_t *)-1)
	{
		sem_close_jt(m_SemInter);
	}
}

int SemShmIpcSet::Init(SemShmIpcMode Mode, const string& SemInterName, const string& SemCrossName, const string& ShmName, int ShmLen)
{
	m_SemInterName = SemInterName;
	m_SemCrossName = SemCrossName;
	m_ShmName = m_ShmName;

	if(Mode == AsMaster)
	{
		m_Fd = shm_create_jt(m_ShmName.c_str());
		if(m_Fd==-1)
		{	
			jtprintf("[%s]\n");
			return -1;
		}
		
		m_SemCross = sem_create_jt(m_SemCrossName.c_str(), 0);
		if(m_SemCross==(sem_t *)-1)
		{	
			jtprintf("[%s]\n");
			return -1;
		}

		m_SemInter = sem_create_jt(m_SemInterName.c_str(), 0);
		if(m_SemInter==(sem_t *)-1)
		{	
			jtprintf("[%s]\n");
			return -1;
		}	

		return 0;
	}
	else if(Mode == AsSlave)
	{
		m_Fd = shm_open_jt(m_ShmName.c_str(),ShmLen, m_DataPtr);
		if(m_Fd==-1)
		{	
			jtprintf("[%s]\n");
			return -1;
		}
		
		m_SemCross = sem_open_jt(m_SemCrossName.c_str());
		if(m_SemCross==(sem_t *)-1)
		{	
			jtprintf("[%s]\n");
			return -1;
		}

		m_SemInter = sem_open_jt(m_SemInterName.c_str());
		if(m_SemInter==(sem_t *)-1)
		{	
			jtprintf("[%s]\n");
			return -1;
		}		

		return 0;
	}
	else 
	{
		return -99;
	}

	return 0;
}

int SemShmIpcSet::GetData(unsigned char** data, int *len)
{
	*data = (unsigned char*)m_DataPtr;
	*len = *((int*)m_DataPtr);

	return 0;
}
int SemShmIpcSet::SetData(unsigned char* data, int len)
{

}

int SemShmIpcSet::Wait(int Timeout)
{
	if(m_SemInter != (sem_t *)-1)
	{
		return sem_wait_timeout_jt(m_SemInter, Timeout);
	}

	return -1;
}
int SemShmIpcSet::Post()
{
	if(m_SemCross!=(sem_t *)-1)
	{	
		if(sem_post_jt(m_SemCross))
		{
			return 0;
		}
	}

	return -1;
}

