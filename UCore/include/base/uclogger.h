#ifndef UC_LOGGER_H
#define UC_LOGGER_H

#include "ucbase.h"
#include "ucconfig.h"
#include <stdarg.h>
#include <uctype.h>

namespace UCODE 
{
    /**
	 * @brief FileLogger module name
     */
	const CHAR SDLOGGER_MODULENAME[]        = "SDLOGGER";

    /**
	 * @brief FileLogger version number
     */
	const SUCODEVersion	SDLOGGER_VERSION    = { 1, 0, 0, 0 };
	
/**
 * @brief invalid log level
 */
#define LOGLV_NONE      0x0000

/**
 * @brief log level for debug
 */
#define LOGLV_DEBUG     0x0001

/**
 * @brief log level for information
 */
#define LOGLV_INFO      (0x0001<< 1)

/**
 * @brief log level for warning
 */
#define LOGLV_WARN      (0x0001<< 2)

/**
 * @brief log level for critical error
 */
#define LOGLV_CRITICAL  (0x0001<< 3)


    /**
     * @brief file mode for FileLogger
     */
	enum ELogMode
	{
		LOG_MODE_SINGLE_FILE = 0,	///< Record the log in the single file
		LOG_MODE_DAY_DIVIDE,		///< Record the log in different files partition by days
		LOG_MODE_MONTH_DIVIDE,		///< Record the log in different files partition by month
	};

    /**
     * @brief The based log interface
     */
	class ISDLogger
	{
		public:

            /**
             * @brief Log information in text format
             * @param pszLog : Pointer to the logs to be logged
             * @return returns true if log success, If the pszLog is NULL, LogText returns false
             */
			virtual bool UCAPI LogText(const TCHAR* pszLog) = 0;

            /**
             * @brief Log information in Binary format
             * @param pLog : Pointer to the logs to be logged
             * @param dwLen : Length of the logs to be logged
             * @return true if log success, Otherwise returns false
             */
			virtual bool UCAPI LogBinary(const UINT8* pLog, UINT32 dwLen) = 0;
	};

    /**
     * @brief File Logger class
     */
	class ISDFileLogger : public ISDLogger
	{
		public:
			/**
			* @brief Initialize
			* @param pszDir : logger file directory
			* @param pszFileName : logger file name
			* @param nMode : logger file partition mode
			* @return operation result
			*/
			virtual bool UCAPI Init(const TCHAR* pszDir, const TCHAR* pszFileName, INT32 nMode) = 0;

            /**
             * @brief De-initialize
             */
			virtual void UCAPI Release() = 0;
	};

    /**
     * @brief UDP Logger class
     */
	class ISDUdpLogger :  public ISDLogger
	{
		public:
			/**
			* @brief Initialize
			* @param pszIp : the IP of logger server
			* @param wPort : the Port of logger server
			* @param byServerType : what type server to use the NetLogger module
			* @return operation result
			*/
			virtual bool UCAPI Init(const TCHAR* pszIp, const UINT16 wPort, const TCHAR* pszErrorfile = NULL) = 0;

            /**
             * @brief De-initialize
             */
			virtual void UCAPI Release() = 0;
	};

    /**
     * @brief pure virtual base class is the interface of tcp logger used tcp
     */
	class ISDTcpLogger : public ISDLogger
	{
		public:
            /**
			* @brief Initialize
			* @param pszIp : the IP of logger server
			* @param wPort : the Port of logger server
			* @param byServerType : what type server to use the NetLogger module
			* @return operation result
			*/
			virtual bool UCAPI Init(const TCHAR* pszIp, const UINT16 wPort, const TCHAR* pszErrorfile = NULL) = 0;

            /**
             * @brief De-initialize
             */
			virtual void UCAPI Release() = 0;

	};

#ifdef UCODE_HAS_MSMQ_LOGGER

    /**
     * @brief pure virtual base class is the interface of msmq logger used shared memory
     */
	class ISDMsmqLogger : public ISDLogger
	{
		public:
			virtual BOOL UCAPI Init(const TCHAR* szQueuePath, INT32 sendtype) = 0;
			virtual VOID UCAPI Release() = 0;

	};

#endif 

	/**
	* @brief SDCreateNetLogger function type
	* inc sdlogger.h
	*/
	typedef ISDUdpLogger* (UCAPI *PFN_SDCreateUdpLogger)(const SUCODEVersion* pstVersion);

	/**
	* @brief Module function getting method which return the pointer of ISDUdpLogger 
	* inc sdlogger.h
	* @param pstVersion : the version number of target module, please passed in SDLOGGER_MODULENAME
	* @return	he pointer of ISDUdpLogger if succeeded£¬NULL for failed
	  */
	ISDUdpLogger* UCAPI SDCreateUdpLogger(const SUCODEVersion* pstVersion);

	/**
	* @brief typedef SDCreateTcpLogger function type inc sdlogger.h
    */
	typedef ISDTcpLogger* (UCAPI *PFN_SDCreateTcpLogger)(const SUCODEVersion* pstVersion);

	/**
	* @brief Module function getting method which return the pointer of ISDTcpLogger 
	* inc sdlogger.h
	* @param pstVersion : the version number of target module, please passed in SDLOGGER_MODULENAME
	* @return	the pointer of ISDTcpLogger if succeeded£¬NULL for failed
	  */
	ISDTcpLogger* UCAPI SDCreateTcpLogger(const SUCODEVersion* pstVersion);

	/**
	* @brief typedef SDCreateFileLogger function type inc sdlogger.h
	*/
	typedef ISDFileLogger* (UCAPI *PFN_SDCreateFileLogger)(const SUCODEVersion* pstVersion);

	/**
	* @brief Module function getting method which return the pointer of ISDFileLogger
	* inc sdlogger.h
	* @param pstVersion : the version number of target module, please passed in SDLOGGER_MODULENAME
	* @return the pointer of ISDFileLogger if succeeded£¬NULL for failed
    */
	ISDFileLogger* UCAPI SDCreateFileLogger(const SUCODEVersion* pstVersion);

#ifdef UCODE_HAS_MSMQ_LOGGER
	/**
	* @brief Module function getting method which return the pointer of ISDMsmqLogger
	* inc sdlogger.h
	* @param pstVersion : the version number of target module, please passed in SDLOGGER_MODULENAME
	* @return the pointer of ISDMsmqLogger if succeeded£¬NULL for failed
	*/
	ISDMsmqLogger* UCAPI SDCreateMsmqLogger(const SUCODEVersion* pstVersion);

	/**
	* @brief typedef SDCreateMsmqLogger function type inc sdlogger.h
	*/
	typedef ISDMsmqLogger* (UCAPI *PFN_SDCreateMsmqLogger)(const SUCODEVersion* pstVersion);
#else 
	/**
	* @brief for sdcommon.def
	*/
	void * UCAPI SDCreateMsmqLogger(const SUCODEVersion* pstVersion);

#endif //

    /**
     * @brief Logger wrapper
     */
	class CSDLogger
	{
		public:
            /**
             * @brief constructor
             */
			CSDLogger();

            /**
             * @brief destructor
             */
			virtual ~CSDLogger();

            /**
             * @brief set logger implementation
             * @param pLogger : logger implementation
             */
			void UCAPI SetLogger(ISDLogger* pLogger);

            /**
             * @brief log by LOGLV_CRITICAL
             * @param format : same as printf
             */
			void UCAPI Critical(const TCHAR *format, ...);

            /**
             * @brief log by LOGLV_INFO
             * @param format : same as printf
             */
			void UCAPI Info(const TCHAR *format, ...);

            /**
             * @brief log by LOGLV_WARN
             * @param format : same as printf
             */
			void UCAPI Warn(const TCHAR *format, ...);

            /**
             * @brief log by LOGLV_DEBUG
             * @param format : same as printf
             */
			void UCAPI Debug(const TCHAR *format, ...);

            /**
             * @brief log by given log level
             * @param dwLevel : log level
             * @param pszFormat : same as printf
             * @param argptr : parameter list
             */
			void _Log(UINT32 dwLevel, const TCHAR *pszFormat, va_list argptr);
		protected:    
			const CHAR* m_aszLogPrefix[9]; ///< log prefix
			ISDLogger* m_pLogger;          ///< log implementation
	};


}
#endif
 


