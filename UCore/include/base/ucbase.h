#ifndef UCBASE_H
#define UCBASE_H

#include "uctype.h"

namespace UCODE {

struct SUCODEVersion
{
	UINT16 wMajorVersion;
	UINT16 wMinorVersion;
	UINT16 wCompatibleVersion;
	UINT16 wBuildNumber;
};

/**
 * @brief global UCODE version
 */
const SUCODEVersion UCODE_VERSION = { 1, 0, 0, 0 };

/**
 * @brief The base class of modules in namespace UCODE
 *
 * Every module that exports from DLL was inherited from this class
 */
class ISDBase
{
public:
    virtual UCAPI ~ISDBase(void){}

    /**
     * @brief Add the reference of the module instance
     *
     * This method should be called every time the CreateInstance \n
     * method of the module was called
     */
    virtual void UCAPI AddRef(void) = 0;

    /**
     * @brief Query the reference time of the module instance
     * @return the reference number of the instance
     */
    virtual UINT32  UCAPI QueryRef(void) = 0;

    /**
     * @brief Decrease the reference number of the module by 1
     *
     * When decrease to zero, the instance of the module will be destroyed
     */
    virtual void UCAPI Release(void) = 0;

    /**
     * @brief Get the version information of the module
     * @return the version structure of the module
     */
    virtual SUCODEVersion UCAPI GetVersion(void) = 0;

    /**
     * @brief Get the name of the module
     * @return the name string of the module
     */
    virtual const char * UCAPI GetModuleName(void) = 0;
};

#ifdef _UCODE_NET
#define UCODE_NET_FUN __declspec(dllexport)
#else
#define	UCODE_NET_FUN __declspec(dllexport)
#endif

}

#endif

