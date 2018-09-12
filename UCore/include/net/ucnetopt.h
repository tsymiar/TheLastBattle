#ifndef UCODE_SDNETOPT_H
#define UCODE_SDNETOPT_H

#include "ucbase.h"

namespace UCODE{

/**
* @brief The following flags start with CONNECTION_OPT_ are belong to ISDConnection.
*/

/**
* @brief Set the socket's option flag, which implemented by calling setsockopt of system
*/
const UINT32 CONNECTION_OPT_SOCKOPT = 1;

/**
* @brief CONNECTION_OPT_SOCKOPT is a data structure, its members correspond the parameters of setsockopt
*/
struct SConnectionOptSockopt
{
    INT32       nLevel;
    INT32       nOptName;
    const char* pOptVal;
    INT32       nOptLen;
};

/**
* @brief The following flags start with LISTENER_OPT_ are the option flag of CSDGateListener.
*/

/**
* @brief This option is used to set the SessionFactory of CSDGateListener, which is derived from
* ISDSessionFactory used to communicate with session manager class of GateServer.
* please refer to sdnet.h
*/
const UINT32 LISTENER_OPT_SESSION_FACTORY = 101;

/**
* @brief Set the option of the  SessionFactory class yourself defined, please refer sdnet.h
*/
struct SListenerOptSessionFactory
{
    void* pSessionFactory;
};
/**
* @brief the extern option for sdnet module
* You should set it before creating the sdnet module, or else it will make no effect.
*/

/**
* @brief This option is used to set the max connection limit number for sdnet.
* Note: this flag is only apply for linux version of sdnet
*/
const UINT32 NETLIN_OPT_MAX_CONNECTION = 201;

struct SNetLinOptMaxConnection
{
    INT32 nMaxConnection;
};

/**
* @brief Set the size of some queues of the sdnet module. You could adjust these parameters
* to satisfied your reserve. In most cases, the default value could satisfied. 
* Note: this flag is only apply for linux version of sdnet
*/
const UINT32 NETLIN_OPT_QUEUE_SIZE = 202;

struct SNetLinOptQueueSize
{
    INT32 nRecvBufSize;         // total recv buffer size, -1 means use default value
    INT32 nSendBufSize;         // total send buffer size,-1 means use default value
    INT32 nRequestQueueSize;    // request queue size,-1 means use default value
    INT32 nEventQueueSize;      // event queue size,-1 means use default value
};

/**
* @brief This flag is used to set some advance flag for sdnet developer, the user should
* not touch it.
* Note: this flag is only apply for linux version of sdnet
*/
const UINT32 NETLIN_OPT_ADVANCE_PARAM = 203;

struct SNetLinOptAdvanceParam
{
    INT32 nParam1;  // -1 means use default value
    INT32 nParam2;  // -1 means use default value
};

/**
* @brief This option is used to set the max connection limit number for sdnet.
* Note: this flag is only apply for windows version of sdnet
*/
const UINT32 NETWIN_OPT_MAX_CONNECTION = 301;

struct SNetWinOptMaxConnection
{
    INT32 nMaxConnection;
};

/**
* @brief Set the size of some queues of the sdnet module. You could adjust these parameters
* to satisfied your reserve. In most cases, the default value could satisfied. 
* Note: this flag is only apply for windows version of sdnet
*/
const UINT32 NETWIN_OPT_QUEUE_SIZE = 302;

struct SNetWinOptQueueSize
{
    INT32 nRecvBufSize;         // total recv buffer size, -1 means use default value
    INT32 nEventQueueSize;      // event queue size, -1 means use default value
};

/**
* @brief This flag is used to set some advance flag for sdnet developer, the user should
* not touch it.
* Note: this flag is only apply for windows version of sdnet
*/
const UINT32 NETWIN_OPT_ADVANCE_PARAM = 303;

struct SNetWinOptAdvanceParam
{
    INT32 nParam1;  // -1 means use default value
};


/**
* @brief To bind the socket's IP and Port
*/
const UINT32 CONNECTOR_OPT_SOCKBIND = 401;

/**
* @brief	CONNECTION_OPT_SOCKBIND associates with SConnectorOptSockBind data structure, 
* which points out the IP and port to be bound
*/
struct SConnectorOptSockBind
{	
	const char*	pszIP;	// 0-end IP string , if NULL won't bind
	UINT16			wPort;	// host byte order, if 0 won't bind
};

/**
* @brief To bind the socket's IP and Port
*/
const UINT32 TCP_NO_DELAY = 501;

/**
* @brief	CONNECTION_OPT_SOCKBIND associates with SConnectorOptSockBind data structure, 
* which points out the IP and port to be bound
*/
struct STcpOptSockNoDelay
{	
	BOOL    bNodelay;
};
/**
* @brief To bind the socket's IP and Port
*/
const UINT32 IOCP_THREAD_NUM = 601;

/**
* @brief	CONNECTION_OPT_SOCKBIND associates with SConnectorOptSockBind data structure, 
* which points out the IP and port to be bound
*/
struct SIocpOptThreadNum
{	
	UINT8    byThreadNum;
};

/**
* @brief To bind the socket's IP and Port
*/
const UINT32 THREAD_LOOP_SIZE = 701;

/**
* @brief	CONNECTION_OPT_SOCKBIND associates with SConnectorOptSockBind data structure, 
* which points out the IP and port to be bound
*/
struct SThreadLoopSize
{	
	UINT32 dwLoopSize;
};

/**
* @brief Send strategy
*/
const UINT32 SEND_STRATEGY = 801;

/**
* @brief	SEND_STRATEGY associates with SSendStrategy data structure, 
*/
struct SSendStrategy
{	
    UINT32 dwSendBuffFullDelayInMilliSec;
    UINT32 dwSendBuffFullRetryCountBeforeDisconnect;
};

}

#endif

