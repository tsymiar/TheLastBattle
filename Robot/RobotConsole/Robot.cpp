#include "stdafx.h"
#include "Robot.h"
#include "RobotMgr.h"
#include <time.h> 

namespace ReBot
{
	CCClient::CCClient(CRobotMgr* pMgr,UINT32 robotId)
		: m_pRobotMgr(pMgr)
		, m_un64BattleID(0)
		, m_MapID(1002)
		, m_pPlayer(NULL)
		, m_bConnected(false)
		, m_bSendConnect(true)
	{
		mLoginState = RLS_None;
		m_eAskState = RS_AskNone;
		m_SLinkState = eServerLink_None;
		m_un32RobotID = robotId;
		m_n32NSID = 0;
		m_n32SSID = 0;
		m_eServerBattleState = eSSBS_Invalid;
		m_ServerStartTime = 0;
		m_bSendConnect = true;
		m_un32HeaderID = 0;
		ClearAllGO();
		//////////////////////////////////////////////////////////////////////////
		m_n8RobotSex = 1;
		//m_szUserName = GetRandStrFromNum(6);
		char temp[64];
		m_szUserName = itoa(m_un32RobotID,temp,10);
		m_szUserPwd = GetRandStrFromNum(6);
		m_szNickName =  GetRandStr(9);
		std::uniform_int_distribution<INT32> distribution(7, 32);
		m_un32HeaderID = distribution(m_pRobotMgr->m_Generator);
		m_bSendConnect = FALSE;
		m_BattleType = RBT_None;
		m_Role = RRIR_AsIdle;
		mHadOnce = false;
		m_AskRoomListTime = 0;
		m_AskAddRoomTime = 0;
		m_RoomMasterWaitTime = 0;
		m_DisConnectTime = 0;
		m_LastErrorCode = 0;
		mTeamID = false;
	}

	CCClient::~CCClient(void){
	}

	void	CCClient::EnterBattle(DWORD curTime){
		if (m_n32SSID>0) return;//cs告知有战场
		if (m_un64BattleID>0) return;//已收到战场邀请
		if (m_eServerBattleState!=eSSBS_Invalid) return;//已确认过加入战场
		if (m_Role==RRIR_AsIdle){
			if (m_eAskState==RS_AskCompleteUserInfoOk) { m_RoomCounter.clear(); AskReEnterRoom(); }
			else if (m_eAskState==RS_AskReEnterRoomFailed) AskMatchTeamList();
			else if (m_eAskState==RS_AskMatchTeamListFailed) { 
				int rand = RandInt(1,100);// 100：20房主 50房友 30匹配  
				if ((rand=rand-20)<=0) m_Role = RRIR_AsRoomMaster;
				else if ((rand=rand-50)<=0) m_Role = RRIR_AsRoomMate;
				else if ((rand=rand-30)<=0) m_Role = RRIR_AsTeamMaster;
			}
			else if (m_RoomCounter.size()>0){
				for (auto it=m_RoomCounter.begin();it!=m_RoomCounter.end();++it){
					if (it->second.UserGuid==m_UserGUID){
						m_Role = it->second.IfMaster?RRIR_AsRoomMaster:RRIR_AsRoomMate;
						break;
					}
				}
			}
			else if (mTeamList.size()>0){
				m_Role =  RRIR_AsTeamMaster;
			}
		}
		if (m_Role==RRIR_AsIdle || m_Role>=RRIR_AsTotal) return;//未获取你要扮演的角色
		switch(m_Role)
		{
		case RRIR_AsRoomMaster:
			{
				if (m_RoomCounter.size()==0){
					if (m_eAskState!=RS_AskCreateRoom) { AskCreateRoom(); m_RoomMasterWaitTime = curTime; }
				}
				else if (m_RoomCounter.size()>=6 || curTime>m_RoomMasterWaitTime+60000){//满6人开
					if (m_eAskState!=RS_AskStartRoom){
						bool isAllReady = true;
						for (auto it=m_RoomCounter.begin();it!=m_RoomCounter.end();++it){
							if(it->second.UserGuid!= m_UserGUID && !it->second.IfReady){				
								isAllReady = false; 
								break;
							}
						}
						if(isAllReady == TRUE){
							AskStartRoom();
							m_RoomMasterWaitTime = 0;
						}
					}
					else{
						// 等待进入战场吧...
					}
				}
			}
			break;
		case RRIR_AsRoomMate:
			{
				if (m_RoomCounter.size()==0){
					if (mRoomlist.size()==0){//请求房间列表
						if (m_eAskState!=RS_AskRoomList) { AskRoomList(); m_AskRoomListTime = curTime; }
						else if (curTime>m_AskRoomListTime+5000) { AskRoomList(); m_AskRoomListTime = curTime; }
					}
					else{//请求加入房间列表
						int select = RandInt(0,mRoomlist.size()-1);
						auto selectIter = mRoomlist.begin(); while(select-->0) { ++selectIter; }
						if (m_eAskState!=RS_AskAddRoom) { AskAddRoom(selectIter->second.roomId); m_AskAddRoomTime = curTime; }
						else if (curTime>m_AskRoomListTime+1000) { AskAddRoom(selectIter->second.roomId); m_AskAddRoomTime = curTime; }
						mRoomlist.erase(selectIter);
					}
				}
				else if (m_eAskState!=RS_AskReadyRoom){
					for (auto it=m_RoomCounter.begin();it!=m_RoomCounter.end();++it){
						if(it->second.UserGuid==m_UserGUID && !it->second.IfReady){	
							AskReadyRoom();
						}
					}
				}
				else {
					// 等待进入战场吧...
				}
			}
			break;
		case RRIR_AsTeamMaster:
			{
				if (mTeamList.size()==0){
					if (m_eAskState!=RS_AskCreateMatchTeam) AskCreateMatchTeam(1002,eMatchType_Nomal);
					else if (m_eAskState==RS_AskCreateMatchTeamFailed) { /*创建匹配失败?*/ }
				}
				else{
					if (m_eAskState!=RS_AskStartMatch) AskStartMatch();
					else if (m_eAskState==RS_AskStartMatchFailed) { /*开始匹配失败*/ }
				}
			}
			break;
		}
	}

	void	CCClient::PlayBattle(DWORD curTime){
		if (m_eServerBattleState==eSSBS_Invalid){
			if (m_un64BattleID>0 && m_eAskState!=RS_AskEnterBattle){
				AskEnterBattle(m_un64BattleID);
			}
			else if (m_eAskState==RS_AskEnterBattleFailed){
				if (m_n32SSID==0){//SS、CS上都需已退出战斗
					FinishBattle(curTime);
				}
			}
		}
		else if (m_eServerBattleState==eSSBS_SelectHero){
			for (auto it=m_RoomCounter.begin();it!=m_RoomCounter.end();++it){
				if (m_UserGUID==it->second.UserGuid){
					if (it->second.TryHeroId==0){
						if (m_eAskState!=RS_AskTryToSelectHero) AskTrySelectHero();
					}else if (it->second.HeroId==0){
						if (m_eAskState!=RS_AskSelectHero) AskSelectHero();
					}
				}
			}
		}
		else if (m_eServerBattleState==eSSBS_SelectRune){
			// 让服务器自己走时间吧...
		}
		else if (m_eServerBattleState==eSSBS_Loading){
			if (m_eAskState!=RS_AskLoadMapComplete){
				AskLoadMapComplete();
			}
		}
		else if (m_eServerBattleState==eSSBS_Playing){
			for (GOMap::iterator iter = m_pClientGameObjectMap.begin(); iter != m_pClientGameObjectMap.end(); ++iter){
				iter->second->OnHeartBeat(curTime);
			}
			if (curTime>m_ServerStartTime+300000){
				if (m_eAskState!=RS_AskGMCmd) AskGMCmd("finish 1");
			}
		}
		else if (m_eServerBattleState==eSSBS_Finished){
			if (m_n32SSID==0){//SS、CS上都需已退出战斗
				FinishBattle(curTime);
			}
		}
	}

	void	CCClient::FinishBattle(DWORD curTime)
	{
		m_un64BattleID = 0;
		m_eServerBattleState = eSSBS_Invalid;
		m_eAskState = RS_AskCompleteUserInfoOk;
		m_Role = RRIR_AsIdle;
		m_RoomCounter.clear();
		mRoomlist.clear();
		mTeamList.clear();
		mHadOnce = true;
	}

	void	CCClient::OnHeartBeat(DWORD curTime)
	{
		if (!mHadOnce)
		{
			int k = 1;
			int j = k;
		}
		Connect();
		if (!m_bConnected) return;
		if (mLoginState!=RLS_GsHadGetBaseInfo) return;
		if (m_eAskState==RS_AskCompleteUserInfoFailed){
			m_szNickName = GetRandStr(15);
			AskCompleteUserInfo(m_szNickName.c_str(),m_un32HeaderID,m_n8RobotSex);
			return;
		}
		if (m_eAskState==RS_AskCompleteUserInfo || m_eAskState==RS_AskCompleteUserInfoFailed) return;
		EnterBattle(curTime);
		PlayBattle(curTime);
	}

	INT32	CCClient::OnNetConnected(INT32 n32NetSessionID)
	{
		ELOG(LOG_SpecialDebug,"robot(%u)(%u)(%u) OnNetConnected",m_un32RobotID,m_SLinkState,n32NetSessionID);
		m_n32NSID = n32NetSessionID;
		switch(m_SLinkState)
		{
		case eServerLink_LS:
			Assert(mLoginState==RLS_LsConnecting);
			mLoginState = RLS_LsConnected;
			EmsgToLs_AskLogin(m_pRobotMgr->m_PlatForm,m_szUserName,m_szUserPwd);
			break;
		case eServerLink_BS:
			Assert(mLoginState==RLS_BsConnecting);
			mLoginState = RLS_BsConnected;
			EmsgToBs_AskLogin(0,m_szUserName,m_szUserPwd);
			break;
		case eServerLink_GS:
			mLoginState = RLS_GsConnected;
			EmsgToGs_AskLogin(0,m_szUserNameRet,m_szUserPwd,"equipmentid");
			break;
		}
		return 0;
	}

	INT32 CCClient::OnNetRealConnected(INT32 n32NetSessionID)
	{
		ELOG(LOG_SpecialDebug,"robot(%u)(%u)(%u) OnNetRealConnected",m_un32RobotID,m_SLinkState,m_n32NSID);
		m_bConnected = true;
		return 0;
	}

	INT32	CCClient::OnNetDisconnected(INT32 n32NetSessionID)
	{
		ELOG(LOG_SpecialDebug,"robot(%u)(%u)(%u) OnNetDisconnected ",m_un32RobotID,m_SLinkState,m_n32NSID);
		m_bConnected = false;
		m_bSendConnect = false;
		m_eAskState = RS_AskNone;
		m_eServerBattleState = eSSBS_Invalid;
		m_DisConnectTime = m_pRobotMgr->m_TickCount;
		return 0;
	}

	CClientGameObject* CCClient::GetGOByGUID(const SGUID_KEY& guid){
		GOMap::iterator iter = m_pClientGameObjectMap.find(guid);
		if (iter != m_pClientGameObjectMap.end()){
			return iter->second;
		}

		return NULL;
	}

	std::string CCClient::GetRandStr(INT32 n32Len){
		static string str = "0123456789abcdefghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ"; 
		std::uniform_int_distribution<INT32> distribution(0, str.size() - 1);

		string res;
		for(INT32 i = 0; i < n32Len; ++i){
			INT32 n32Index = distribution(m_pRobotMgr->m_Generator);
			res.push_back(str[n32Index]);
		}
		return res;
	}

	INT32 CCClient::AskBackToHome(){
		return eNormal;
	} 

	std::string CCClient::GetRandStrFromNum( INT32 n32Len )
	{ 
		static string strNum = "0123456789";
		std::uniform_int_distribution<INT32> distribution(0, strNum.size() - 1);

		string res;
		for(INT32 i = 0; i < n32Len; ++i){
			INT32 n32Index = distribution(m_pRobotMgr->m_Generator);
			res.push_back(strNum[n32Index]);
		}
		return res;
	}

	void CCClient::NetworkClose( EServerLinkType type )
	{
		INetSession* iCli = INetSessionMgr::GetInstance()->GetSession(m_n32NSID);
		Assert(iCli!=NULL);
		iCli->DelayClose(111);
	}

	void CCClient::ClearAllGO(){
		for (auto iter = m_pClientGameObjectMap.begin(); iter != m_pClientGameObjectMap.end(); ++iter){
			delete iter->second;
		}
		m_pClientGameObjectMap.clear();
		m_pPlayer = nullptr;
	}

	void CCClient::EraseObj(const SGUID_KEY& guid){
		auto iter = m_pClientGameObjectMap.find(guid);
		if (iter != m_pClientGameObjectMap.end()){
			delete iter->second;
			m_pClientGameObjectMap.erase(iter);
		}
	}

	void CCClient::Connect()
	{
		if (m_bConnected) return;//已链接
		if (m_bSendConnect) return;//已发送链接
		if (CRobotMgr::m_n32ConnectingRobotNum>=3) return;//当前登录玩家太多
		if (m_pRobotMgr->m_TickCount<m_DisConnectTime+1000) return;
		m_pRobotMgr->PostConnect(this);
	}

	INT32 CCClient::OnMsg_NotifyRoomListRet( const char* pMsg,int n32MsgLength,int n32MsgID )
	{
		std::auto_ptr<GSToGC::AskRoomListRet> msg(new GSToGC::AskRoomListRet);
		msg->ParseFromArray(pMsg,n32MsgLength);
		mRoomlist.clear();
		for (int i=0;i<msg->roomlist_size();++i)
		{
			const GSToGC::RoomInfo& msgRoom = msg->roomlist(i);
			SRoomInfo& sRoom = mRoomlist[msgRoom.roomid()];
			sRoom.roomId = msgRoom.roomid();
			sRoom.ifPWD = msgRoom.ifpwd();
			sRoom.maxCount = msgRoom.maxusercount();
			sRoom.curCount = msgRoom.curusercount();
		}
		return eNormal;
	}

	INT32 CCClient::OnMsg_NotifyTeamBaseInfo( const char* pMsg,int n32MsgLength,int n32MsgID )
	{
		std::auto_ptr<GSToGC::NotifyMatchTeamBaseInfo> msg(new GSToGC::NotifyMatchTeamBaseInfo);
		msg->ParseFromArray(pMsg,n32MsgLength);
		mTeamID = msg->teamid();
		return eNormal;
	}

	INT32 CCClient::OnMsg_NotifyTeamPlayerInfo( const char* pMsg,int n32MsgLength,int n32MsgID )
	{
		std::auto_ptr<GSToGC::NotifyMatchTeamPlayerInfo> msg(new GSToGC::NotifyMatchTeamPlayerInfo);
		msg->ParseFromArray(pMsg,n32MsgLength);
		int pos = msg->postion();
		int headid = msg->headid();
		if (headid>0){//增加
			auto &info = mTeamList[pos];
			info.Seat = pos;
			info.NickName = msg->nickname();
			info.headId = headid;
			info.level = 1;
		}
		else{
			mTeamList.erase(pos);
		}
		return eNormal;
	}

}