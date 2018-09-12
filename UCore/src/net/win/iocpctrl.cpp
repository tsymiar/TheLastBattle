#include "ucnetconfig.h"
#include "iocpctrl.h"
#include <process.h>
#include "cpsock.h"
#include "cplistener.h"
#include <ucstring.h>
//定义每个CPU跑2个线程
#define THREAD_PER_CPU		2

IMPLEMENT_SINGLETON(CIocpCtrl)

int CIocpCtrl::m_acceptCount = 0;

CIocpCtrl::CIocpCtrl() : 
m_hCompletionPort(NULL),
m_nNumberOfWorkers(0),
m_WorkerArray(NULL)
{
}

CIocpCtrl::~CIocpCtrl()
{
	Uninit();
}

bool CIocpCtrl::Init()
{
	//创建IO完成端口句柄
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hCompletionPort == NULL)
	{
		CRITICAL(_SDT("CIocpCtrl::Init, CreateIoCompletionPort failed, Error %d \n"), ::WSAGetLastError()); 
		return false;
	}

	//获取当前服务器的CPU核数
	SYSTEM_INFO stSysInfo;
	GetSystemInfo(&stSysInfo);
	m_nNumberOfWorkers = stSysInfo.dwNumberOfProcessors * THREAD_PER_CPU;

    if (g_nThreadNum > 0)
    {
        m_nNumberOfWorkers = g_nThreadNum;
    }
	m_WorkerArray = new HANDLE[m_nNumberOfWorkers];
	for (INT32 i = 0; i < m_nNumberOfWorkers; i++) 
	{
		m_WorkerArray[i] = INVALID_HANDLE_VALUE;
	}

	//创建m_nNumberOfWorkers个线程
	UINT dwThreadID = 0;
	for (INT32 j = 0; j < m_nNumberOfWorkers; j++) 
	{
		m_WorkerArray[j] = (HANDLE)_beginthreadex(	NULL,					// Security
													0,						// Stack size - use default
													ThreadFunc,     		// Thread fn entry point
													(void*)this,			// Param for thread
													0,						// Init flag
													&dwThreadID);			// Thread address

		if (NULL == m_WorkerArray[j]) 
		{
			m_nNumberOfWorkers = j;
			this->Uninit();
			CRITICAL(_SDT("CIocpCtrl::Init, Create Worker thread failed, Close Handler\n")); 
			return false;
		}
		
	}
	return true;
}

void CIocpCtrl::Uninit()
{
	if (0 == m_nNumberOfWorkers)
	{
		return;
	}

	INT32 i;
	for(i = 0; i < m_nNumberOfWorkers; i++)
	{
		PostQueuedCompletionStatus(m_hCompletionPort, 0, NULL, NULL);
	}

	WaitForMultipleObjects(m_nNumberOfWorkers, m_WorkerArray, true, 3000);
	for(i = 0; i < m_nNumberOfWorkers; i++)
	{
		CloseHandle(m_WorkerArray[i]);
	}
	delete [] m_WorkerArray;
	m_WorkerArray = NULL;
	
	CloseHandle(m_hCompletionPort);
	m_hCompletionPort = NULL;
	m_nNumberOfWorkers = 0;
}

unsigned CIocpCtrl::ThreadFunc(LPVOID pParam)
{
	CIocpCtrl* poCtrl = (CIocpCtrl*)pParam;
	poCtrl->m_threadBufPool.CreateThreadBuffer();
	poCtrl->OnExecute();
	poCtrl->m_threadBufPool.ReleaseThreadBuffer();
	return 0;
}

void CIocpCtrl::OnExecute()
{
	SPerHandleData* pstPerHandleData;
	SPerIoData*		pstPerIoData;
	CCPSock*		poSock;
	CCpListener*	poListener;
	BOOL			bRet;
	DWORD			dwByteTrabsferred;
	
	while(true)
	{
		pstPerHandleData	= NULL;
		pstPerIoData		= NULL;
		dwByteTrabsferred	= 0;

		bRet = GetQueuedCompletionStatus(
			m_hCompletionPort,
			&dwByteTrabsferred,
			(PULONG_PTR)&pstPerHandleData,
			(LPOVERLAPPED*)&pstPerIoData,
			INFINITE);

		// 检查是否是线程退出
		if(NULL == pstPerHandleData)
		{
			return;
		}

		//当有客户端请求创建连接时
		if(pstPerHandleData->bListen)
		{
			// for listen event
			poListener = (CCpListener*)pstPerHandleData->ptr;
			if(NULL != poListener &&  NULL != pstPerIoData)
			{
				poListener->OnAccept(bRet, pstPerIoData);
				//printf("Accpet Count:%d \n", InterlockedIncrement((LONG*)&m_acceptCount) ); 

			}
			else 
			{
				SDASSERT(false);
			}
		}
		else 
		{
			//for non-listen event 
			poSock = (CCPSock*)pstPerHandleData->ptr;
			if ( NULL == poSock )
			{
				continue;
			}
			if( FALSE == bRet || NULL == pstPerIoData )
			{				
                if (::WSAGetLastError()!=ERROR_IO_PENDING)
                {
                    INFO(_SDT("[%s:%d]CCPSock connID=%d error %d, close it"), 
                        MSG_MARK, poSock->GetConnectionID(), ::WSAGetLastError());
                    poSock->OnClose();
                }				
			}
			else
			{			
				switch(pstPerIoData->nOp)
				{
				case IOCP_RECV:
					{
						poSock->DecPostRecv();
						if (dwByteTrabsferred > 0)
						{
							poSock->OnRecv(dwByteTrabsferred);
						}
						else
						{
							INFO(_SDT("[%s:%d]CCPSock connID=%d error %d, close it, socket :%d "), 
								MSG_MARK, poSock->GetConnectionID(), ::WSAGetLastError(), poSock->GetSock());
							poSock->OnClose();
						}
					}
					break;
				case IOCP_SEND:
					{
						poSock->DecPostSend();
						if (dwByteTrabsferred > 0)
						{
							poSock->OnSend(dwByteTrabsferred);
						}
						else
						{
							INFO(_SDT("[%s:%d]CCPSock connID=%d error %d, close it"), 
								MSG_MARK, poSock->GetConnectionID(), ::WSAGetLastError());
							poSock->OnClose();
						}
					}
					break;
				case IOCP_CLOSE:
					{
						poSock->OnClose(false);
					}
					break;
			 
				default:
					;
				}
			}
		}
	}
}

bool CIocpCtrl::AssociateWithIocp(SOCKET hSock, SPerHandleData* pstData)
{
	if (NULL == m_hCompletionPort)
	{
		return false;
	}

	if(NULL == CreateIoCompletionPort((HANDLE)hSock, m_hCompletionPort, (ULONG_PTR)pstData, 0))
	{
		WARN(_SDT("CIocpCtrl::AssociateWithIocp, failed, errno %d"), WSAGetLastError());
		return false;
	}

	return true;
}

