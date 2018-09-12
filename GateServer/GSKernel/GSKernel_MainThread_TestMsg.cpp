#include "stdafx.h"
#include "GSKernel.h"
#include <iostream>
#include <fstream>
#include "GSSSInfo.h"

using namespace std;

Begin_JT_FBAll_GateServer_GSKernel

extern CGSKernel g_cGSKernelInstance;

INT32	CGSKernel::CheckPostTestMsg()
{
	return eNormal;
	//return eNormal;
	if (0 == m_n32TestMsgStage){
		PostTestMsg_Registe(INetMgr::MakeNSID(1, 1), "Test1", "123456", "TestNick1", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(2, 1), "Test2", "123456", "TestNick2", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(3, 1), "Test3", "123456", "TestNick3", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(4, 1), "Test4", "123456", "TestNick4", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(5, 1), "Test5", "123456", "TestNick5", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(6, 1), "Test6", "123456", "TestNick6", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(7, 1), "Test7", "123456", "TestNick7", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(8, 1), "Test8", "123456", "TestNick8", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(9, 1), "Test9", "123456", "TestNick9", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(10, 1), "Test10", "123456", "TestNick10", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(11, 1), "Test11", "123456", "TestNick11", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(12, 1), "Test12", "123456", "TestNick12", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(13, 1), "Test13", "123456", "TestNick13", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(14, 1), "Test14", "123456", "TestNick14", 2000);
		PostTestMsg_Registe(INetMgr::MakeNSID(15, 1), "Test15", "123456", "TestNick15", 2000);
		WYYTools_LogMgr->Debug("CGSKernel::CheckPostTestMsg() Post Regsite msg down.");
		m_tLastPostTestMsgUTCMilsec = GetWYYToolsInstance()->GetUTCMilliSecond();
		m_n32TestMsgStage = 1;
	}
	else if (1 == m_n32TestMsgStage){
		//return eNormal;
		TIME_MILSEC tCurUTCMilsec = GetWYYToolsInstance()->GetUTCMilliSecond();
		TIME_TICK tTickSpan = tCurUTCMilsec - m_tLastPostTestMsgUTCMilsec;
		if (tTickSpan < 1000){
			return eNormal;
		}
		PostTestMsg_Login(INetMgr::MakeNSID(1, 1), eUP_None, "Test1", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(2, 1), eUP_None, "Test2", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(3, 1), eUP_None, "Test3", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(4, 1), eUP_None, "Test4", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(5, 1), eUP_None, "Test5", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(6, 1), eUP_None, "Test6", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(7, 1), eUP_None, "Test7", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(8, 1), eUP_None, "Test8", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(9, 1), eUP_None, "Test9", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(10, 1), eUP_None, "Test10", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(11, 1), eUP_None, "Test11", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(12, 1), eUP_None, "Test12", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(13, 1), eUP_None, "Test13", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(14, 1), eUP_None, "Test14", "123456");
		PostTestMsg_Login(INetMgr::MakeNSID(15, 1), eUP_None, "Test15", "123456");
		WYYTools_LogMgr->Debug("CGSKernel::CheckPostTestMsg() Post Login msg down.");
		m_tLastPostTestMsgUTCMilsec = tCurUTCMilsec;
		m_n32TestMsgStage = 2;
	}else if (2 == m_n32TestMsgStage){
		TIME_MILSEC tCurUTCMilsec = GetWYYToolsInstance()->GetUTCMilliSecond();
		TIME_TICK tTickSpan = tCurUTCMilsec - m_tLastPostTestMsgUTCMilsec;
		if (tTickSpan < 1000){
			return eNormal;
		}
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(1, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(2, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(3, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(4, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(5, 1));
		PostTestMsg_LeaveBattle(INetMgr::MakeNSID(3, 1));
		//PostTestLeaveRoomMsg(INetMgr::MakeNSID(4, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(6, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(7, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(8, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(9, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(10, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(11, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(12, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(13, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(14, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(15, 1));
		PostTestMsg_QuickBattle(INetMgr::MakeNSID(3, 1));
		PostTestMsg_ChangeBattleSeat(INetMgr::MakeNSID(3, 1), 5);
		WYYTools_LogMgr->Debug("CGSKernel::CheckPostTestMsg() Post QuckBattle msg down.");
		m_tLastPostTestMsgUTCMilsec = tCurUTCMilsec;
		m_n32TestMsgStage = 3;
	}else if (3 == m_n32TestMsgStage){
		TIME_MILSEC tCurUTCMilsec = GetWYYToolsInstance()->GetUTCMilliSecond();
		TIME_TICK tTickSpan = tCurUTCMilsec - m_tLastPostTestMsgUTCMilsec;
		if (tTickSpan < 1000){
			return eNormal;
		}
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(1, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(2, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(3, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(4, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(5, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(6, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(7, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(8, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(9, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(10, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(11, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(12, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(13, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(14, 1));
		PostTestMsg_ReadyBattle(INetMgr::MakeNSID(15, 1));
		WYYTools_LogMgr->Debug("CGSKernel::CheckPostTestMsg() Post ReadyBattle msg down.");
		m_tLastPostTestMsgUTCMilsec = tCurUTCMilsec;
		m_n32TestMsgStage = 4;
	}else if (4 == m_n32TestMsgStage){
		TIME_MILSEC tCurUTCMilsec = GetWYYToolsInstance()->GetUTCMilliSecond();
		TIME_TICK tTickSpan = tCurUTCMilsec - m_tLastPostTestMsgUTCMilsec;
		if (tTickSpan < 1000){
			return eNormal;
		}
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(1, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(2, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(3, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(4, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(5, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(6, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(7, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(8, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(9, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(10, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(11, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(12, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(13, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(14, 1));
		PostTestMsg_BeginBattle(INetMgr::MakeNSID(15, 1));
		WYYTools_LogMgr->Debug("CGSKernel::CheckPostTestMsg() Post BeginBattle msg down.");
		m_tLastPostTestMsgUTCMilsec = tCurUTCMilsec;
		m_n32TestMsgStage = 5;
	}
	else if (5 == m_n32TestMsgStage){
		TIME_MILSEC tCurUTCMilsec = GetWYYToolsInstance()->GetUTCMilliSecond();
		TIME_TICK tTickSpan = tCurUTCMilsec - m_tLastPostTestMsgUTCMilsec;
		if (tTickSpan < 3000){
			return eNormal;
		}
		PostTestMsg_Login(INetMgr::MakeNSID(35, 1), eUP_None, "Test5", "123456");
		PostTestMsg_SelectHero(INetMgr::MakeNSID(1, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(2, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(3, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(4, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(5, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(6, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(7, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(8, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(9, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(10, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(11, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(12, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(13, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(14, 1), 36601);
		PostTestMsg_SelectHero(INetMgr::MakeNSID(15, 1), 36601);
		WYYTools_LogMgr->Debug("CGSKernel::CheckPostTestMsg() Post SelectHero msg down.");
		m_tLastPostTestMsgUTCMilsec = tCurUTCMilsec;
		m_n32TestMsgStage = 6;
	}else if (6 == m_n32TestMsgStage){
		return eNormal;
		TIME_MILSEC tCurUTCMilsec = GetWYYToolsInstance()->GetUTCMilliSecond();
		TIME_TICK tTickSpan = tCurUTCMilsec - m_tLastPostTestMsgUTCMilsec;
		if (tTickSpan < 2000){
			return eNormal;
		}

		PostTestMsg_RandMoveDir(INetMgr::MakeNSID(1, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(2, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(3, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(4, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(5, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(6, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(7, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(8, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(9, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(10, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(11, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(12, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(13, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(14, 1));
		//PostTestMsg_RandMoveDir(INetMgr::MakeNSID(15, 1));

		WYYTools_LogMgr->Debug("CGSKernel::CheckPostTestMsg() Post Move msg down.");
		m_tLastPostTestMsgUTCMilsec = tCurUTCMilsec;
	}

	return eNormal;
}

INT32	CGSKernel::PostTestMsg_Registe(INT32 n32GCNSID, const CHAR *cpszUserName, const CHAR *cpszUserPwd, const CHAR *cpszNickName, INT32 n32HeaderID)
{
	TFastMsgBuilder<c_n32DefaultMiddleMsgBuffSize> cMsgBuilder;
	cMsgBuilder.AddData(cpszUserName, c_n32DefaultNameLen);
	cMsgBuilder.AddData(cpszUserPwd, c_n32DefaultUserPwdLen);
	cMsgBuilder.AddData(cpszNickName, c_n32DefaultNameLen);
	cMsgBuilder.AddINT32(n32HeaderID);
	cMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGSToCSFromGC_AskRegiste;
	return OnGCRecved(cMsgBuilder.GetMsgHeader(), n32GCNSID);
	//m_piTempMsg->Reset();
	//m_piTempMsg->Add(n32GCNSID);
	//m_piTempMsg->Add((BYTE*)cMsgBuilder.GetMsgHeader(), cMsgBuilder.GetMsgHeader()->n32Size);
	//m_piTempMsg->GetMsgHeader()->n32ProtocolID = eMsgToCSFromGS_ReportGCMsg;
	//PostToCentralServer(m_piTempMsg->GetMsgHeader());
	//return eNormal;
}

INT32	CGSKernel::PostTestMsg_Login(INT32 n32GCNSID, EUserPlatform eUP, const CHAR *cpszUserName, const CHAR *cpszUserPwd)
{
	TFastMsgBuilder<c_n32DefaultMiddleMsgBuffSize> cMsgBuilder;
	cMsgBuilder.AddINT32(eUP);
	cMsgBuilder.AddStr(cpszUserName, c_n32DefaultNameLen);
	cMsgBuilder.AddStr(cpszUserPwd, c_n32DefaultUserPwdLen);
	cMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGSToCSFromGC_AskLogin;
	return OnGCRecved(cMsgBuilder.GetMsgHeader(), n32GCNSID);
	//m_piTempMsg->Reset();
	//m_piTempMsg->Add(n32GCNSID);
	//m_piTempMsg->Add((BYTE*)cMsgBuilder.GetMsgHeader(), cMsgBuilder.GetMsgHeader()->n32Size);
	//m_piTempMsg->GetMsgHeader()->n32ProtocolID = eMsgToCSFromGS_ReportGCMsg;
	//PostToCentralServer(m_piTempMsg->GetMsgHeader());
	//return eNormal;
}

INT32	CGSKernel::PostTestMsg_QuickBattle(INT32 n32GCNSID)
{
	TFastMsgBuilder<c_n32DefaultMiddleMsgBuffSize> cMsgBuilder;
	cMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGSToCSFromGC_AskQuickBattle;

	return OnGCRecved(cMsgBuilder.GetMsgHeader(), n32GCNSID);
	//m_piTempMsg->Reset();
	//m_piTempMsg->Add(n32GCNSID);
	//m_piTempMsg->Add((BYTE*)cMsgBuilder.GetMsgHeader(), cMsgBuilder.GetMsgHeader()->n32Size);
	//m_piTempMsg->GetMsgHeader()->n32ProtocolID = eMsgToCSFromGS_ReportGCMsg;
	//PostToCentralServer(m_piTempMsg->GetMsgHeader());
	return eNormal;
}

INT32	CGSKernel::PostTestMsg_LeaveBattle(INT32 n32GCNSID)
{
	TFastMsgBuilder<c_n32DefaultMiddleMsgBuffSize> cMsgBuilder;
	cMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGSToCSFromGC_AskLeaveBattle;

	return OnGCRecved(cMsgBuilder.GetMsgHeader(), n32GCNSID);

	//m_piTempMsg->Reset();
	//m_piTempMsg->Add(n32GCNSID);
	//m_piTempMsg->Add((BYTE*)cMsgBuilder.GetMsgHeader(), cMsgBuilder.GetMsgHeader()->n32Size);
	//m_piTempMsg->GetMsgHeader()->n32ProtocolID = eMsgToCSFromGS_ReportGCMsg;
	//PostToCentralServer(m_piTempMsg->GetMsgHeader());
	return eNormal;
}

INT32	CGSKernel::PostTestMsg_ChangeBattleSeat(INT32 n32GCNSID, INT32 n32TargetPos)
{
	TFastMsgBuilder<c_n32DefaultMiddleMsgBuffSize> cMsgBuilder;
	cMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGSToCSFromGC_AskChangeBattleSeat;
	cMsgBuilder.AddINT32(n32TargetPos);

	return OnGCRecved(cMsgBuilder.GetMsgHeader(), n32GCNSID);

	//m_piTempMsg->Reset();
	//m_piTempMsg->Add(n32GCNSID);
	//m_piTempMsg->Add((BYTE*)cMsgBuilder.GetMsgHeader(), cMsgBuilder.GetMsgHeader()->n32Size);
	//m_piTempMsg->GetMsgHeader()->n32ProtocolID = eMsgToCSFromGS_ReportGCMsg;
	//PostToCentralServer(m_piTempMsg->GetMsgHeader());
	//return eNormal;
}

INT32	CGSKernel::PostTestMsg_ReadyBattle(INT32 n32GCNSID){
	TFastMsgBuilder<c_n32DefaultMiddleMsgBuffSize> cMsgBuilder;
	cMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGSToCSFromGC_AskReadyBattle;
	return OnGCRecved(cMsgBuilder.GetMsgHeader(), n32GCNSID);
}

INT32	CGSKernel::PostTestMsg_BeginBattle(INT32 n32GCNSID)
{
	TFastMsgBuilder<c_n32DefaultMiddleMsgBuffSize> cMsgBuilder;
	cMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGSToCSFromGC_AskBeginBattle;
	return OnGCRecved(cMsgBuilder.GetMsgHeader(), n32GCNSID);
}

INT32	CGSKernel::PostTestMsg_SelectHero(INT32 n32GCNSID, UINT32 un32HeroTypeID){

	//TFastMsgBuilder<c_n32DefaultMiddleMsgBuffSize> cMsgBuilder;
	//cMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGSToSSFromGC_AskSelectHero;
	//cMsgBuilder.AddINT32(20001);
	//cMsgBuilder.AddUINT32(un32HeroTypeID);

	//return OnGCRecved(cMsgBuilder.GetMsgHeader(), n32GCNSID);
	return eNormal;
}

INT32	CGSKernel::PostTestMsg_RandMoveDir(INT32 n32GCNSID){
	
	CVector3D cTempDir;
	cTempDir.m_fX = rand() % 1000;
	cTempDir.m_fY = 0;
	cTempDir.m_fZ = rand() % 1000;

	if (rand() % 2 == 1){
		cTempDir.m_fX = -cTempDir.m_fX;
	}
	if (rand() % 2 == 1){
		cTempDir.m_fZ = - cTempDir.m_fZ;
	}

	cTempDir.unit();

	TFastMsgBuilder<c_n32DefaultMiddleMsgBuffSize> cMsgBuilder;
	cMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGSToSSFromGC_AskMoveDir;
	cMsgBuilder.AddINT32(20001);
	cMsgBuilder.AddData(&cTempDir, sizeof(cTempDir));
	
	return OnGCRecved(cMsgBuilder.GetMsgHeader(), n32GCNSID);
}


End_JT_FBAll_GateServer_GSKernel
