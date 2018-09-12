#include "stdafx.h"
#include "RobotMgr.h"
#include "Robot.h"

namespace ReBot
{
	//call back event while net listener heart beat.
	INT32	CRobotMgr::OnConnectorHeartBeart(){
		TIME_MILSEC  tCurMilsec = GetWYYToolsInstance()->GetUTCMilliSecond();
		if (tCurMilsec - tLastHeartBeatMilsec > 100){
			if (NULL != m_pcRobotList){
				for (INT32 i = 0; i < m_n32RobotNum; i++){
					m_pcRobotList[i].OnHeartBeat(tCurMilsec);
				}
			}

			tLastHeartBeatMilsec = tCurMilsec;
		}

		return 0;
	}

	//call back event while received new message.
	UINT32	CRobotMgr::OnReceieved(INT32 n32NetSessionID, BYTE *pbData, UINT32 un32DataLen)
	{
		char* p = NULL; 
		*p = '\0';
		return eNormal;
	}

	//call back event while disconnect from server.
	VOID	CRobotMgr::OnDisconnect(INT32 n32NetSessionID) {
		CCClient *pcRobot = GetRobotByNetLogicID(n32NetSessionID);
		if (NULL != pcRobot)
		{
			pcRobot->OnNetDisconnected(n32NetSessionID);
		}
		return;
	}

	//call backer event while connected to server.
	INT32	CRobotMgr::OnConnected(INT32 n32NetSessionID){
		CCClient *pcRobot = GetRobotByNetLogicID(n32NetSessionID);
		if (NULL != pcRobot)
		{
			pcRobot->OnNetConnected(n32NetSessionID);
		};
		return 0;
	}

	//call backer event while connect fail.
	INT32	CRobotMgr::OnConnectFail(INT32 n32NetSessionID, INT32 n32ErrorCode){
		CCClient *pcRobot = GetRobotByNetLogicID(n32NetSessionID);
		if (NULL != pcRobot){
			pcRobot->OnNetConnectFail(n32NetSessionID, n32ErrorCode);
		}
		return 0;
	}
}