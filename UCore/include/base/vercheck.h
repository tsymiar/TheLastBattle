#ifndef _VER_CHECK_
#define _VER_CHECK_

#include "ucbase.h"

namespace UCORE
{

UINT32  VerToDWORD(const SUCOREVersion* pstVersion);
BOOL  VerCheck(const SUCOREVersion* pRequestVer, const SUCOREVersion* pRealVer);

}//end namespace

#endif

