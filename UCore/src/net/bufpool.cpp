#include "ucnetconfig.h"
#include "bufpool.h"

CBufPool::CBufPool(void)
{
	m_dwSize = 0;
}

CBufPool::~CBufPool(void)
{
}

void CBufPool::Init(UINT32 dwSize)
{
	m_dwSize = dwSize;
}

void CBufPool::Uninit()
{
	for(CSetBuf::iterator it = m_oSetBuf.begin(); it != m_oSetBuf.end(); ++it)
	{
		delete[] *it;
	}
	
	m_oSetBuf.clear();
	m_oListBuf.clear();
}

UINT32 CBufPool::GetSize()
{
	return m_dwSize;
}

char* CBufPool::Create()
{
	char* pBuf = NULL;
	if(!m_oListBuf.empty())
	{
		pBuf = m_oListBuf.front();
		SDASSERT(pBuf != NULL);
		m_oListBuf.pop_front();

#ifdef _DEBUG
		CSetBuf::iterator it = m_oSetFree.find(pBuf);
		if(it == m_oSetFree.end())
		{
			SDASSERT(false);
		}
		else
		{
			m_oSetFree.erase(it);
		}
#endif
	}
	else
	{
		pBuf = new char[m_dwSize];
		memset(pBuf, 0, m_dwSize);
		m_oSetBuf.insert(pBuf);
	}

	return pBuf;
}

void CBufPool::Release(char* pBuf)
{
	if(NULL == pBuf)
	{
		return;
	}

#ifdef _DEBUG
	memset(pBuf, 0, m_dwSize);
	CSetBuf::iterator it = m_oSetBuf.find(pBuf);
	if(it == m_oSetBuf.end())
	{
		SDASSERT(false);
	}

	it = m_oSetFree.find(pBuf);
	if(it != m_oSetFree.end())
	{
		SDASSERT(false);
	}
	else
	{
		m_oSetFree.insert(pBuf);
	}
#endif

	m_oListBuf.push_back(pBuf);
}

