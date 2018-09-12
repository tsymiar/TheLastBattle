#include "vercheck.h"
#include <memory.h>

namespace UCODE
{
	UINT32  VerToDWORD(const SUCODEVersion* pstVersion)
	{
		UINT32 dwValue;
		char* p = (char*)&dwValue;
		memcpy(p,&pstVersion->wMinorVersion,sizeof(UINT16));
		memcpy(p + sizeof(UINT16),&pstVersion->wMajorVersion,sizeof(UINT16));
		return dwValue;
	}

	BOOL  VerCheck(const SUCODEVersion * pRequestVer, const SUCODEVersion * pRealVer)
	{
		if( !pRequestVer ) return TRUE;
		if( !pRealVer ) return FALSE;

		UINT16 dwUser,dwDll;

		dwUser = VerToDWORD(pRequestVer);
		dwDll = VerToDWORD(pRealVer);

		if (dwDll == dwUser)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
        return TRUE;
	}

}//end namespace

