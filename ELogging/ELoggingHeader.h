#ifndef _ELOG_HEADER_
#define _ELOG_HEADER_

#include "DefLogging.h"

#define ELOG(level, msg, ...) ELog(level, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)
#define Assert(exp, ...) ((exp) ? 0 : (ELog(LOG_ASSERT,__FUNCTION__, __LINE__, #exp, ## __VA_ARGS__), assert(0)))

#endif

