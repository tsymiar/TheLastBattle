#ifndef UC_CONFIG_H
#define UC_CONFIG_H

#include "ucmacros.h"

#define SDENT_HAS_RECV_QUEUE  // default is defined 

/**
* @file sdu.h
* @brief SD Platform Definitions
* @remark This is a generated header generated from include/sdu.h.in by
* ./configure, or copied from include/sdu.hw or include/sdu.hnw 
* for Win32 or Netware by those build environments, respectively.
*/

#if defined(WIN32) || defined(WIN64)

/**
* @brief Ignore most warnings (back down to /W3) for poorly constructed headers
*/
#if defined(_MSC_VER) && _MSC_VER >= 1200
#pragma warning(push, 3)
#endif



/**
* @brief Has windows.h already been included?  If so, our preferences don't matter,
* but we will still need the winsock things no matter what was included.
* If not, include a restricted set of windows headers to our tastes.
*/
#ifndef _WINDOWS_
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif /* !_WINDOWS_ */

/**
* @brief Done with badly written headers
*/

#endif /* WIN32 */

#endif // 

