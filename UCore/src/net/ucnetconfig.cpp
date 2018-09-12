#include "ucnetconfig.h"
#include "ucnet.h"

INT32 MAX_CONNECTION			= 10000;
INT32 RECV_LOOP_BUF_SIZE		= (10*1024*1024);	//10M
INT32 SEND_LOOP_BUF_SIZE		= (10*1024*1024);	//10M
INT32 MAX_NET_EVENT			= (10*1024*1024);		//10M
UINT32 DELAY_RELEASE_CPSOCK_TIME= 3;

INT32 MAX_NET_REQUEST			= (1024*1024);		//1M
INT32 POP_TO_BUF_COUNT			= 10;
INT32 VAL_SO_SNDLOWAT			= 128*1024;

BOOL  g_bNodelay = TRUE;
INT32 g_nThreadNum = -1;
INT32 g_nLoopSize = 4096*1024*6;

UINT32 g_dwNetLogLevel = 0;
CSDLogger g_oNetLogger;

bool UCAPI UCODE::SDNetSetLogger(ISDLogger* poLogger, UINT32 dwLevel)
{
	if(NULL == poLogger)
	{
		return false;
	}

	g_dwNetLogLevel = dwLevel;
	g_oNetLogger.SetLogger(poLogger);
	return true;
}

