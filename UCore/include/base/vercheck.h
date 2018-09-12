#ifndef _VER_CHECK_
#define _VER_CHECK_

#include "ucbase.h"

namespace UCODE
{

UINT32  VerToDWORD(const SUCODEVersion* pstVersion);
BOOL  VerCheck(const SUCODEVersion* pRequestVer, const SUCODEVersion* pRealVer);

}//end namespace

#endif

