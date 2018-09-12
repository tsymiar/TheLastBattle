#if (defined(WIN32) || defined(WIN64))
#include <Winsock2.h>
#endif
#include "conndataallocator.h"
#include <ucstring.h>

CBufPool* ConnDataAllocator::FindBufPool(UINT32 dwSize, bool bCreate)
{
	CMapBufPool::iterator it = m_mapBufPool.find(dwSize);
	if(it != m_mapBufPool.end())
	{
		return it->second;
	}

	if(false == bCreate)
	{
		return NULL;
	}

	CBufPool* poBufPool = new CBufPool;
	if(NULL == poBufPool)
	{
		CRITICAL(_SDT("CCPSockMgr::_FindBufPool, new CBufPool failed"));
		return NULL;
	}
	poBufPool->Init(dwSize);
	m_mapBufPool[dwSize] = poBufPool;

	return poBufPool;
}

void ConnDataAllocator::ReleaseBufPool(char * pBuf, UINT32 dwSize)
{
	CBufPool * pBufPool = FindBufPool(dwSize, false); 

	if (pBufPool != NULL)
	{
		pBufPool->Release(pBuf); 
	}
}

