// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUserMgr.h"
#include "CSUser.h"
#include "CSGameLogMgr.h"
#include "../ELogging/ELogging/SSActive.h"
#include "CSCfgMgr.h"
#include "CSKernel.h"

using namespace std;

namespace CentralServer{
//不再查询数据库，直接从内存里验证！
INT32	CCSUserMgr::OnMgrFromGC_AskCompleteUserInfo(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CCSUser *pcUser = GetUserByNetInfo(cpiGSInfo, n32GCNSID);
	if (NULL == pcUser){
		ELOG(LOG_ERROR, "user net(%d) can't find", cpiGSInfo->GetNSID());
		return eEC_NullUser;
	}
	else{
		pcUser->ResetPingTimer();
	}
	INT32 n32Res = eNormal;
	
	do 
	{
		boost::shared_ptr<GCToCS::CompleteInfo> pCompleteInfo = ParseProtoMsg<GCToCS::CompleteInfo>(pData, n32DataLength);
		if (!pCompleteInfo){
			return 0;
		}

		if (!pCompleteInfo->has_nickname() || pCompleteInfo->nickname().empty()){
			n32Res = eEC_NickNameNotAllowed;
			break;
		}
		
		if (pCompleteInfo->nickname().size() > c_n32DefaultNickNameLen){
			n32Res = eEC_NickNameNotAllowed;
			break;
		}
		if (CCSCfgMgr::getInstance().CheckInvalidWorld(pCompleteInfo->nickname().c_str())){
			n32Res = eEC_NickNameNotAllowed;
			break;
		}

		map<string,bool>& aiNameMapForCheck = CCSCfgMgr::getInstance().GetAiRobotNameMapForCheck();
		if (aiNameMapForCheck.find(pCompleteInfo->nickname())!=aiNameMapForCheck.end()){
			n32Res = eEC_NickNameCollision;
			break;
		}

		auto iter = m_AllNickNameSet.find(pCompleteInfo->nickname());
		if (iter != m_AllNickNameSet.end()){
			n32Res = eEC_NickNameCollision;
			break;
		}

		int n32Res = 0;
		CCSUser* pcUser = GetUserByNetInfo(cpiGSInfo, n32GCNSID);
		if (NULL == pcUser){
			n32Res = eEC_NullUser;
			ELOG(LOG_ERROR, "pcUser is null");
			return 0;
		} 
		
		pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_Sex, pCompleteInfo->sex());
		pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_HeaderId, pCompleteInfo->headid());

		ChangeUserNickName(pcUser, pCompleteInfo->nickname());

		pcUser->SynUser_UserBaseInfo();
		pcUser->PostCSNotice();

		DBPoster_UpdateUser(pcUser);//存盘// 

		{
			stringstream mystream;
			mystream << pcUser->GetUserDBData().szUserName << LOG_SIGN;
			mystream << pCompleteInfo->headid() << LOG_SIGN;
			mystream << pCompleteInfo->nickname();

			CSSGameLogMgr::GetInstance().AddGameLog(eLog_HeadUse,mystream.str(),0);
		} 
	} while (FALSE);

	if (eNormal != n32Res){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskComleteUserInfo, n32Res);
	}
	
	return eNormal;
}

INT32	CCSUserMgr::OnMgrFromGC_AskLogin(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	GCToCS::Login pLogin;
	if (!ParseProtoMsg(pData, n32DataLength, pLogin)){
		ELOG(LOG_ERROR, "Login Fail With Msg Analysis Error.");
		return 0;
	} 
	ELOG(LOG_SpecialDebug,"--new login msg!%s--",pLogin.name());
	INT32 n32RetFlag = UserAskLogin(cpiGSInfo, n32GCNSID, pLogin);
	if (eNormal != n32RetFlag){
		PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskLogin, n32RetFlag);
	}
	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskBuyGoods(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	GCToCS::AskBuyGoods pMsg;
	if (!ParseProtoMsg(pData, n32DataLength, pMsg)){
		ELOG(LOG_ERROR, "Login Fail With Msg Analysis Error.");
		return 0;
	} 

	INT32 n32Res = pcUser->AskBuyGoods(pMsg);
	if (eNormal != n32Res){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskBuyGoods, n32Res);
	}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskEqipRunes(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	GCToCS::EuipRunes pMsg;
	if (!ParseProtoMsg(pData, n32DataLength, pMsg)){
		ELOG(LOG_ERROR, "Login Fail With Msg Analysis Error.");
		return 0;
	}

	INT32 n32Res = pcUser->GCAskEqipRunes(pMsg.runeid(), pMsg.topos());
	if (eNormal != n32Res)
	{
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_EuipRunes, n32Res);
	}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskUnloadRunes(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	GCToCS::UnEuipRunes pMsg;
	if (!ParseProtoMsg(pData, n32DataLength, pMsg)){
		ELOG(LOG_ERROR, "Login Fail With Msg Analysis Error.");
		return 0;
	} 

	INT32 n32Res = pcUser->GCAskUnloadRunes(pMsg.page(), pMsg.pos());
	if (eNormal != n32Res)
	{
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_UnloadRunes, n32Res);
	}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskComposeRunes(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::ComposeRunes> pMsg = ParseProtoMsg<GCToCS::ComposeRunes>(pData, n32DataLength);
	if (!pMsg){
		ELOG(LOG_ERROR, "Login Fail With Msg Analysis Error.");
		return 0;
	} 

	INT32 n32Res = pcUser->GCAskComposeRunes(*pMsg);
	if (eNormal != n32Res){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_ComposeRunes, n32Res);
	}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskUserGameInfo(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::UserGameInfo> pUserIno = ParseProtoMsg<GCToCS::UserGameInfo>(pData, n32DataLength);
	if (!pUserIno){
		return 0;
	}

	INT32 n32RetFlag = pcUser->GCAskUserGameInfo();
	if( eNormal != n32RetFlag){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskUserGameInfo, n32RetFlag);
	}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskReconnectGame(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	boost::shared_ptr<GCToCS::ReconnectToGame> pReconnectToGame = ParseProtoMsg<GCToCS::ReconnectToGame>(pData, n32DataLength);
	if (!pReconnectToGame){
		return 0;
	}

	INT32 n32RetFlag = UserAskReconnectGame(cpiGSInfo, n32GCNSID, pReconnectToGame->name(), pReconnectToGame->passwd());
	if( eNormal != n32RetFlag){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskReconnectGame, n32RetFlag);
	}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskAddToSNSList(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskAddToSNSList> pMsg = ParseProtoMsg<GCToCS::AskAddToSNSList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}

	if (pMsg->nickname().empty()){
		return eNormal;
	}

	INT32 n32RetFlag = pcUser->AskAddToSNSlistByNickName(pMsg->nickname(), (ERelationShip)pMsg->type());
	if( eNormal != n32RetFlag){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskAddToSNSList, n32RetFlag);
	}
	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskAddToSNSListByUserID(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskAddToSNSListByID> pMsg = ParseProtoMsg<GCToCS::AskAddToSNSListByID>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}

	INT32 n32RetFlag = pcUser->AskAddToSNSlistByGuid(pMsg->userid(), (ERelationShip)pMsg->type());	
	if( eNormal != n32RetFlag){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskAddToSNSList, n32RetFlag);
	}
	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskRemoveFromSNSList(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	
	boost::shared_ptr<GCToCS::AskRemoveFromSNSList> pMsg = ParseProtoMsg<GCToCS::AskRemoveFromSNSList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}

	INT32 n32RetFlag = pcUser->AskRemoveFromSNSList(pMsg->guididx(), pMsg->type());
	if( eNormal != n32RetFlag)
	{
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskRemoveFromSNSList, n32RetFlag);
	}
	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskSendMsgToUser(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	
	boost::shared_ptr<GCToCS::AskSendMsgToUser> pMsg = ParseProtoMsg<GCToCS::AskSendMsgToUser>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}

	INT32 n32RetFlag = pcUser->AskSendMsgToUser(pMsg->guididx(), pMsg->contents().size(), pMsg->contents().c_str());
	if( eNormal != n32RetFlag){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskSendMsgToUser, n32RetFlag);
	}
	return eNormal;
}

INT32	CCSUserMgr::OnMgrFromGC_AskInviteFriendsToBattle(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskInviteFriendsToBattle> pMsg = ParseProtoMsg<GCToCS::AskInviteFriendsToBattle>(pData, n32DataLength);

	INT32 n32RetFlag = pcUser->AskInviteFriendsToBattle(pMsg->battleid(), pMsg->guididx());

	if( eNormal != n32RetFlag){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, pMsg->msgnum(), n32RetFlag);
	}

	return eNormal;
}

INT32	CCSUserMgr::OnMgrFromGC_AskCanInviteFriends(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);

	pcUser->NotifyCanInviteFriends();

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskQueryUaserByNickName(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
// 	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
// 
// 	boost::shared_ptr<GCToCS::AskQueryUserByNickName> pMsg = ParseProtoMsg<GCToCS::AskQueryUserByNickName>(pData, n32DataLength);
// 	if (!pMsg){
// 		return 0;
// 	}
// 
// 	GSToGC::NotifyQueryNickNameRet sNotifyQueryNickNameRet;
// 	for (map<UINT64, CCSUser*>::iterator iter = m_cUserOnlineMap.begin(); iter != m_cUserOnlineMap.end(); ++iter){
// 		CCSUser* pUser = iter->second;
// 		if(NULL !=pUser && pUser->GetNickName().find(pMsg->nickname())!=-1)
// 		{
// 			GSToGC::NotifyQueryNickNameRet::QueryInfo* pInfo = sNotifyQueryNickNameRet.add_info();
// 			pInfo->set_headid(pUser->GetHeadID());
// 			pInfo->set_nickname(pUser->GetNickName());
// 			ELOG(LOG_INFO, "Get User:%s", pUser->GetNickName());
// 		}
// 	}
// 
// 	GetCSKernelInstance()->PostMsgToGC(cpiGSInfo, n32GCNSID, sNotifyQueryNickNameRet, sNotifyQueryNickNameRet.msgid());

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_GCReplyAddFriendRequst(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	
	boost::shared_ptr<GCToCS::GCReplyAddFriendRequst> pMsg = ParseProtoMsg<GCToCS::GCReplyAddFriendRequst>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	INT32 n32RetFlag = pcUser->ReplyAddFriendRequst(pMsg->reply(), pMsg->guididx(), pcUser->GetGUID());

	if( eNormal != n32RetFlag){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, pMsg->msgnum(), n32RetFlag);
	}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_GCReplyInviteToBattle(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	//CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);

	//const BYTE *pbDataPos = (const BYTE*)(piMsgHeader + 1);
	//INT32 n32Length = *(INT32*)pbDataPos;
	//pbDataPos += sizeof(INT32);

	//CHAR szNickname[c_n32DefaultNickNameLen + 1];
	//memset(szNickname, 0, sizeof(szNickname));
	//memcpy(szNickname, pbDataPos, n32Length);
	//pbDataPos += n32Length;

	//INT32 n32RetFlag = pcUser->ReplyInviteToBattle(szNickname);
	//if( eNormal != n32RetFlag){
	//	return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, eMsgToGSToCSFromGC_GCReplyInviteToBattle, n32RetFlag);
	//}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_Notice(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
// 	boost::shared_ptr<GCToCS::Notice> pMsg = ParseProtoMsg<GCToCS::Notice>(pData, n32DataLength);
// 	if (!pMsg){
// 		return 0;
// 	}
// 
// 	ENoticeType type = (ENoticeType)pMsg->type();
// 	std::string strNotice( pMsg->notice() );  
// 
// 	if (strNotice.length() > 0)
// 	{
// 		//am_NoticeMgr.AddNotice(type,strNotice);
// 	}
// 	else
// 	{
// 		pcUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_Notice, eEC_NothingContent);
// 	}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_UserAskGetCLReward(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);

	INT32 RetFlag = pcUser->AskGetLoginReward();
	if (RetFlag != eNormal){
		pcUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_Notice, eEC_NothingContent);
	}

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskOneTaskRewards(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	INT32 n32Ret = eNormal;
	boost::shared_ptr<GCToCS::AskOneTaskRewards> pMsg = ParseProtoMsg<GCToCS::AskOneTaskRewards>(pData, n32DataLength);
	if (pMsg==NULL) n32Ret = eEC_ParseProtoError;
	else n32Ret = pcUser->GetTaskMgr()->FinishTask(pMsg->task_guid());
	if (n32Ret != eNormal) pcUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskOneTaskRewards, n32Ret);
	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskCurtNotice(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
// 	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID); 
// 	boost::shared_ptr<GCToCS::AskCurtNotice> pMsg = ParseProtoMsg<GCToCS::AskCurtNotice>(pData, n32DataLength);
// 	if (!pMsg){
// 		return 0;
// 	}
// 
// 	pcUser->PostCSNotice(); 
 	return eNormal;
}

INT32		CCSUserMgr::OnMsgFromGC_AskCloseMailOrGetMailGift(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);

	boost::shared_ptr<GCToCS::AskGetMailGift> pMsg = ParseProtoMsg<GCToCS::AskGetMailGift>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	} 
	GSToGC::NotifyMailRet mailRet;
	mailRet.set_mailid(pMsg->mailid());
	if (pMsg->mailid() < 1)
	{
		ELOG(LOG_ERROR,"error mailId:%d",pMsg->mailid());
		mailRet.set_errcode(eEc_InvalidMailId); 
		return pcUser->PostMsgToGC(mailRet,mailRet.msgid());
	}
	
	//1 邮件是否邮件领取
	if (m_MailMgr.checkIfMailHasRecv(pcUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,pMsg->mailid()))
	{
		ELOG(LOG_ERROR,"mailId:%d has recv.",pMsg->mailid());
		mailRet.set_errcode(eEc_MailHasRecv);
		return pcUser->PostMsgToGC(mailRet,mailRet.msgid());
	}
	//2 邮件是否过期
	if (m_MailMgr.checkIfMailHasTimeOver(pMsg->mailid()))
	{ 
		ELOG(LOG_ERROR,"mailId:%d has time out.",pMsg->mailid());
		mailRet.set_errcode(eEc_MailHasTimeOver);
		return pcUser->PostMsgToGC(mailRet,mailRet.msgid()); 
	} 
	 
	bool bFlag  =  m_MailMgr.closeOrGetMailGift(pcUser, pMsg->mailid());
	if (bFlag)
	{  
		Sync_SaveGameMail(); 
	} 
	UpdateMailState2DB(pcUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,pMsg->mailid(),eMailState_Del); 

	GSToGC::DelAndSortMail msg;
	msg.set_mailid(pMsg->mailid());
	msg.set_mailiddel(CCSCfgMgr::getInstance().GetCSGlobalCfg().m_IfDelAtLooked);
	msg.set_sort(CCSCfgMgr::getInstance().GetCSGlobalCfg().m_ifSortAtLooked);

	pcUser->PostMsgToGC(msg,msg.msgid());
	return eNormal;
}

INT32		CCSUserMgr::OnMsgFromGC_AskMailInfo(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);

	boost::shared_ptr<GCToCS::AskMailInfo> pMsg = ParseProtoMsg<GCToCS::AskMailInfo>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}

	GSToGC::MailInfo mailInfo;
	mailInfo.set_mailid(pMsg->mailid());
	INT32  curtState = 0;
	INT32 nRet  = m_MailMgr.getMailInfoById(pcUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,pMsg->mailid(),mailInfo,curtState);
	if (nRet != eNormal)
	{
		GSToGC::NotifyMailRet mailRet;
		mailRet.set_mailid(pMsg->mailid());
		mailRet.set_errcode(nRet);
		return pcUser->PostMsgToGC(mailRet,mailRet.msgid()); 
	}
	pcUser->PostMsgToGC(mailInfo, mailInfo.msgid()); 
	//save 
	UpdateMailState2DB(pcUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,pMsg->mailid(),curtState); 
	 
	return eNormal;
}


INT32 CCSUserMgr::OnMsgFromGC_AskGoodsCfg(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);

	return eNormal;
}

INT32 CCSUserMgr::OnMsgFromGC_AskChangeHeaderid(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskChangeheaderId> pMsg = ParseProtoMsg<GCToCS::AskChangeheaderId>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	pcUser->AskChangeheaderId(pMsg->newheaderid());
	return eNormal;
}

INT32 CCSUserMgr::OnMsgFromGC_AskChangeNickname(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::ChangeNickName> pMsg = ParseProtoMsg<GCToCS::ChangeNickName>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	INT32 res = eNormal;
	do {
		if (pMsg->newnickname().size() < 3)
		{
			res = eEC_NickNameTooShort;
		}

		if (m_AllNickNameSet.end() != m_AllNickNameSet.find(pMsg->newnickname())){
			res = eEC_NickNameCollision;
			break;
		}
		if (!pcUser->CheckIfEnoughPay(ePayType_Diamond, 20) ){
			res = eEC_DiamondNotEnough;
			break;
		}
	} while (false);
	
	if (res != eNormal){
		return  pcUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskChangeNickName, res);
	}

	pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_Diamond, -20);

	pcUser->PostMsgToGC_NotifyNewNickname(pcUser->GetGUID(), pMsg->newnickname());

	ChangeUserNickName(pcUser, pMsg->newnickname());

	//notify new nickname to online friend
	auto iter = pcUser->GetUserDBData().friendListMap.begin();
	for(; iter != pcUser->GetUserDBData().friendListMap.end(); iter ++){
		CCSUser *piUser = GetUserByGuid(iter->second.guididx);
		if (NULL != piUser && piUser->GetUserPlayingStatus() == eUserPlayingStatus_Playing){
			piUser->SynUserSNSList(pcUser->GetGUID(), eRSType_Friends);
		}
	}

	pcUser->SynCurDiamond();
	//日志
	CSSGameLogMgr::GetInstance().AddGameLog(eLog_ChangeUseName,pcUser->GetGUID(),pcUser->GetUserDBData().sPODUsrDBData.un8UserLv,pcUser->GetUserDBData().sPODUsrDBData.un16VipLv);

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromGC_AskRecoinRune(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskRecoinRune> pMsg = ParseProtoMsg<GCToCS::AskRecoinRune>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}

	int res = pcUser->AskRecoinRune(pMsg->rune_id(), pMsg->cost());
	if (res != eNormal){
		return PostMsgToGC_AskReturn(cpiGSInfo, n32GCNSID, GCToCS::eMsgToGSToCSFromGC_AskRecoinRune, res);
	}

	return eNormal;
}


INT32 CCSUserMgr::OnMsgFromGC_AskCDKGift(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	GCToCS::CDKReq sMsg;
	ParseProtoMsg(pData, n32DataLength, sMsg);
	AskCDKGiftData* pAskCDKGiftData = new AskCDKGiftData;
	pAskCDKGiftData->pGuid = pcUser->GetGUID();
	pAskCDKGiftData->cdk = sMsg.cdkstr();

	bool bFlag = false;
	auto pReids = GetLogicRedisHandler();
	if (pReids){
		pReids->redisAsyncCommand(RedisSCallBack(&CCSUserMgr::redisCheckGiftCallBack, this), pAskCDKGiftData, "hmget cdk:%s GiftCDKID", sMsg.cdkstr());
		bFlag = true;
	}
	else{
		ELOG(LOG_ERROR, "Redis未连接!");
	}

	if (bFlag)
	{ 
		CSSGameLogMgr::GetInstance().AddGameLog(eLog_Exchange,pcUser->GetGUID(),sMsg.cdkstr());
	}
	return eNormal;
}

INT32		CCSUserMgr::OnMsgFromGC_ReportCLLog(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength)
{
	CheckAndGetUserByNetInfo(cpiGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::CurtUIEvent> pMsg = ParseProtoMsg<GCToCS::CurtUIEvent>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	stringstream  mystream; 
	INT32 total =  pMsg->eventlist_size();
	for (int i=0; i<total; i++)
	{
		mystream << pMsg->eventlist(i).uiidx() << LOG_SIGN_FIRST;
		mystream << pMsg->eventlist(i).eventnum();
		if (i+1 < total )
		{
			mystream << LOG_SIGN_SECOND;
		}
	}
	CSSGameLogMgr::GetInstance().AddGameLog(eLog_UIEvent, pcUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,   mystream.str() );
	  
	return eNormal;
}
}