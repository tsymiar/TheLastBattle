#pragma once
#include "stdafx.h"
#include "CSUser.h"
#include <string>
#include <map>
#include "../ELogging/ELogging/ThreadSafeQueue.h"
#include "../ELogging/ELogging/SSActive.h"
#include "CSUserDbDataMgr.h"
//#include "CSGameLogMgr.h"
#include "CSMailMgr.h"
#include "CSDataSource.h"
#include "CSScript.h"
#include "DBActiveWrapper.h"
#include "Timer.h"
#include "tbb/concurrent_vector.h"
#include <set>

class Active;
class Buffer;
class CCSUserMgr;

namespace CentralServer{ 

class CCSUserMgr
{
private:
	typedef	map<UINT64, CCSUser*>	UserMap;
	UserMap						m_cUserGUIDMap;//真实表
	UserMap						m_cUserOnlineMap;

	typedef map<string, CCSUser*> UserNickNameMap;
	UserNickNameMap					m_cNickNameMap;//快表（按绰号名）
	map<SUserNetInfo, CCSUser*>		m_cUserNetMap;//快表（按网络连接）

	Concurrency::concurrent_queue<Buffer*>		m_DBCallbackQueue;
	CThreadSafeObejctPool<Buffer>	m_DBCallbackQueuePool;

	//用于玩家数据存储
	DBActiveWrapper			*	m_UserCacheDBActiveWrapper;
	// 用于全局数据操作
	DBActiveWrapper			*	m_CdkeyWrapper;
	//玩家请求时，操作数据库
	vector<DBActiveWrapper*>	m_pUserAskDBActiveWrapperVec;

	//当前服务器最大GUID，但游戏里的GUID是合成的，以保证所有CS Server唯一
	INT64						m_MaxGuid;

	//所有玩家昵称集合，所有关于昵称的操作可以通过此表来操作！
	set<string>					m_AllNickNameSet;
	//所有玩家用户名，guid表，如果玩家登陆 通过CS，SDK，Username查到对应guid
	struct UserCombineKey{
		string username;
		INT32 sdkid;
		UserCombineKey(){}
		UserCombineKey(const string& username, INT32 sdkid):username(username), sdkid(sdkid){}
		bool operator <(const UserCombineKey& sUserCombineKey2)const{
			int res = username.compare(sUserCombineKey2.username);
			if (res == 0){
				return sdkid < sUserCombineKey2.sdkid;
			}
			else{
				return res < 1;
			}
		}
	};
	map<UserCombineKey, UINT64>			m_AllUserName2GUIDMap;

	// 获取当前负载最小的DB线程
	DBActiveWrapper&			GetNowWorkActor();
	CCSMailMgr					m_MailMgr;
	boost::posix_time::ptime	m_TodayTime;//当前时间(年月日时分秒)
	boost::gregorian::date		m_Today;//当前时间(年月日)
	UINT32						m_monthDays;//当前月天数
	bool						m_IsDailyTaskRush;//今日日常任务是否刷新过
	INT64						m_GiftCDKID;//CDK任务唯一标识，用来表示运营每次做活动发放一批CDK的唯一ID，用Redis保存

	std::stringstream			m_SaveUserStream;
private:
	CCSUserMgr();
public:
	static CCSUserMgr&	GetInstance();
	void					Initialize();
	~CCSUserMgr();
	IDBConnector*			GetDBSource(int actorID);
	//注册消息处理回调
	void					RegisterMsgHandle(SSMsgHandlerMap& m_SSMsgHandlerMap, GSMsgHandlerMap& m_GSMsgHandlerMap, GCMsgHandlerMap& m_GCMsgHandlerMap, RCMsgHandlerMap& m_RCMsgHandlerMap);
	//玩家存储DB线程回调
	void					UserCacheDBAsynHandler(Buffer*& pBuffer);
	//玩家请求回调
	void					UserAskDBAsynHandler(Buffer*& pBuffer);

	void					GetProfileReport(wstringstream &report);

	INT32					OnHeartBeatImmediately();

	void					OnTimeUpdate();
	void					OnNewDay();
	void					OnNewMonth();
	void					OnNewYear();

	CCSUser*				GetUserByNickName(const string& stUserName);
	CCSUser*				GetUserByGuid(const INT64& guid);
	CCSUser*				GetUserByNetInfo(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID);
	CCSUser*				GetUserByNetInfo(const SUserNetInfo &crsUNI);

	INT32					OnUserOnline(CCSUser *pcUser, const SUserNetInfo &crsUserNetInfo);
	void					OnUserOffline(CCSUser *pcUser);

	CCSUser*				GetIdlePlayer(map<string,bool>& igoreMap);

	INT32					DBPoster_UpdateUser(CCSUser *pcUse);
	void					PostSaveCmd();

	bool					PostUserAskMsgToDBThread(google::protobuf::Message& sMsg, int n32MsgID);
	bool					PostUserCacheMsgToDBThread(INT64 guidIndex, const string& sqlStr);

	boost::posix_time::ptime& GetTodayTime() { return m_TodayTime; }
	const boost::gregorian::date& GetToday() { return m_Today; }
	UINT32					GetMonthDays() { return m_monthDays; }

	CCSMailMgr		&		GetMailMgr()	{return			m_MailMgr;}
	bool					CheckIfCanRemoveUser(CCSUser*& pUser);

private:
	/************************************************************************/
	/*local functions.                                                      */
	/************************************************************************/
	void					ChangeUserNickName(CCSUser *pcUser, const string& nickName);

	INT32					AddUser(CCSUser *pcUser);
	INT32					RemoveUser(CCSUser*& pcUser);
	//////////////////////////////////////////////////////////////////////////
	INT32					OnMsgFromGS_UserOffline(ICSGSInfo *piGSInfo, const char* pData, int n32DataLength);
	//////////////////////////////////////////////////////////////////////////
	INT32					OnMgrFromGC_AskLogin(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMgrFromGC_AskCompleteUserInfo(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskExchangeGold(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskBuyGoods(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskEqipRunes(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskUnloadRunes(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskComposeRunes(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskUserGameInfo(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskReconnectGame(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskQueryUaserByNickName(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskAddToSNSList(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskAddToSNSListByUserID(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskRemoveFromSNSList(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_GCReplyAddFriendRequst(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMgrFromGC_AskInviteFriendsToBattle(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMgrFromGC_AskCanInviteFriends(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskSendMsgToUser(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_GCReplyInviteToBattle(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_Notice(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_UserAskGetCLReward(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskOneTaskRewards(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskCompCSGuideStep(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskCurtNotice(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskCloseMailOrGetMailGift(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskMailInfo(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskGoodsCfg(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskChangeNickname(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskChangeHeaderid(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskRecoinRune(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskCDKGift(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	//cs gm cmd
	INT32					OnMsgFromGC_AskNewGMCmd(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	//log
	INT32					OnMsgFromGC_ReportCLLog(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);

	void					PostTheNewMail(const MailDBData & mail);
	void					Sync_SaveGameMail();
	//////////////////////////////////////////////////////////////////////////
	INT32					OnMsgFromSS_AskUpdateData(const ICSSSInfo *cpiSSInfo, const char* pData, int n32DataLength);
	INT32					OnMsgFromSS_AskAddGold(const ICSSSInfo *piSSInfo, const char* pData, int n32DataLength);
	INT32					OnMsgFromSS_AskChangeFirstWinTime(const ICSSSInfo *cpiSSInfo, const char* pData, int n32DataLength);
	INT32					OnMsgFromSS_AskUpdateGuideStep(const ICSSSInfo *cpiSSInfo, const char* pData, int n32DataLength);
	INT32					OnMsgFromSS_AskUserGoHall(const ICSSSInfo *cpiSSInfo, const char* pData, int n32DataLength);	
	INT32					OnMsgFromSS_AskUpdateBattleTaskSetps(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength);
	INT32					OnMsgFromSS_AskGameLogSwitch(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength);
	//////////////////////////////////////////////////////////////////////////
	INT32					OnMgrFromRC_AskServerUserInfo(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength);
	INT32					OnMgrFromRC_AskNoticeInfo(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength);
	INT32					OnMgrFromRC_AskAddNotice(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength);
	INT32					OnMgrFromRC_AskDeleteNotice(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength);
	INT32					OnMgrFromRC_AskInsertCDKey(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength);

	INT32					PostAllNoticeInfo(const ICSRCInfo *cpiRCInfo);
	INT32					OnMgrFromRC_AskAddMail(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength); 
	 
	/************************************************************************/
	/* Ask handler                                                          */
	/************************************************************************/
	INT32					UserAskLogin(const ICSGSInfo *cpiGSInfo, INT32 n32UserGCNSID, GCToCS::Login& pLogin);
	INT32					UserAskReconnectGame(const ICSGSInfo *cpiGSInfo, INT32 n32UserGCNSID, const string& cpszUserName, const string& cpszUserPwd);
	INT32					GuideReAskLogin();
	bool					EncodeAndSendToLogicThread(google::protobuf::Message& sMsg, int n32MsgID);

	/************************************************************************/
	/* UserAskDB 玩家即时请求Operation   [单独玩家DB线程，DBConnect也是单独的]                                              */
	/************************************************************************/
	//INT32					GetGuideSteps(IDBConnector *pConn, UINT64	un64ObjIdx,  SUserGuideSteps &guideSteps);
	INT32					GetUserHeros(IDBConnector *pConn, UINT64	un64ObjIdx, DBToCS::QueryUser& sQueryUser);
	INT32					GetUserRune(IDBConnector *pConn, UINT64	un64ObjIdx, DBToCS::QueryUser& sQueryUser);
	INT32					DBAsyn_QueryUserSNSList(IDBConnector *pConn, const UINT64 un64ObjIdx, map<UINT64, UINT32>& t_map);
	INT32					DBAsyn_QueryUserHeaderAndNickname(IDBConnector *pConn, const UINT64 un64ObjIdx, DBToCS::RSinfo& rs_info);
 
	/************************************************************************/
	/* ckey                                              */
	/************************************************************************/	 
	INT32					DBAsyn_QueryUser(SUserDBData & userdb, DBToCS::QueryUser& sQueryUser, IDBConnector* pDB); 
	INT32					DBAsyn_ExeSQL(Buffer*& pBuffer, IDBConnector* pDB); 
	INT32					AddNewGameUserInfo( SUserDBData & sUserData, IDBConnector* pDB); 

	//mail
	INT32					GetUserShortGiftMail(IDBConnector *pConn, SUserDBData & sUserData,  DBToCS::QueryUser& sQueryUser);
public:
	void					PostUserCurtMailList(CCSUser* pcUser); 
	void					CheckPostDelayNewMail(CCSUser* pUser);
private:
	void					DBAsynUpdateGameMail(Buffer*& pBuffer, IDBConnector* pDB);
	UINT64					CombineGUID();
	void					DBAsynUpdateUserGameMail(Buffer*& pBuffer, IDBConnector* pDB);
	/************************************************************************/
	/* UserAskDB 线程异步回调                                                 */
	/************************************************************************/ 
	void					DBAsynQueryUserCallBack(Buffer*& pBuffer, IDBConnector* pDB);
	void					DBAsynTryToChangeMoneyReleated_CallBack(Buffer*& pBuffer, IDBConnector* pDB);
	 
	void					DBAsyn_QueryWhenThreadBegin(); 

	void					DBAsyn_QueryGameMailList(IDBConnector *pConn,INT64 objIdx );
	/************************************************************************/
	/* DB 线程异步回调                                                 */
	/************************************************************************/
	void					DBAsynInsertUserCallback(Buffer*& pBuffer, IDBConnector* pDB);
	void					DBAsynUpdateUserCallback(Buffer*& pBuffer, IDBConnector* pDB);
	/************************************************************************/
	/* DB 同步CallBack                                                 */
	/************************************************************************/
	INT32					SynUserAskDBCallBack();
	INT32					SynHandleQueryUserCallback(Buffer* pBuffer);

public:
	INT32					PostMsgToGC_AskReturn(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, INT32 n32AskProtocalID, INT32 n32RetFlag);
	INT32					PostMsgToGC_AskReturn(const SUserNetInfo &crsUserNetInfo, INT32 n32AskProtocalID, INT32 n32RetFlag);
	INT32					PostMsgToGC_NotifyUserPlayState(CCSUser *pcUser, CCSUser *piUser);

	void					UpdateSNSList(const UINT64 guid, SUserRelationshipInfo info, EDBOperation type);
	void					UpdateUserInfo(boost::shared_ptr<SSToCS::AskUpdateData>& puserData); 
private:
	void					DBAsynAlterSNSList(Buffer*& pBuffer, IDBConnector* pDB);
	INT32					AlterUserSNSList(IDBConnector *pConn, const UINT64 asker_guid, const UINT64 r_guid, ERelationShip rs_type, EDBOperation sOpType);

	void					SynHandleAllAccountCallback(Buffer* buffer);

	void					SynHandleMailCallback(Buffer*pBuffer);

	public:
		std::vector<SNotice>& GetNotice() {return gNoticeVec;};
		UINT64					GetCurDateTime();
		UINT64					GetCDKeySurplusSec(INT64 &dateTime, UINT64 expire_time);
	private:
		std::vector<SNotice>   gNoticeVec;
		void					CDKThreadBeginCallback(); 
		void					InsertNoticeSQL(SNotice notice, string sql_str);
		INT32					DBAsynInsertNoticeCall(Buffer*& pBuffer, IDBConnector* pDB);
		INT32					DBAsynQueryNoticeCallBack(IDBConnector* pDB);
		INT32					DBCallBack_QueryNotice(Buffer*& pBuffer);
		bool					AddNotice(const SNotice &notice);

		INT32					DBAsynChangeNickNameCallBack(Buffer*& pBuffer, IDBConnector* pDB);
	//////////////////////////////////////////////////////////////////////////
	//	Item 相关函数
	public:
		void					UserAskUdateItem(const SUserItemInfo& info, EDBOperation optype, UINT64 user_id);
		//查询CDKID
		void					redisQueryCDKIDCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData);
		void					InitCDKIDCFromRedis();
		void					InitRedisSucscribe();
	private:
		INT32					DBAsyn_QueryUserItems(IDBConnector *pConn, const UINT64 user_id, DBToCS::QueryUser& sQueryUser);
		INT32					DBAsyAlterItemCallBack(Buffer*& pBuffer, IDBConnector* pDB);
		INT32					DBAsyn_AlterUserItem(IDBConnector *pConn, const string sql_str);

		void					UpdateUserNickNameToDB(CCSUser* pUser);

		bool					RemoveUserFromRedisLRU(CCSUser& pUser);

		void					redisLoginCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData);
		void					redisCheckGiftCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData);
		void					redisGetGiftInfoCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData);
		void					redisSubsribeCallBack(redisAsyncContext* predisAsyncContext, redisReply* predisReply, void* pData);

		void					InsertNewUserToMysql(GCToCS::Login &pLogin, CCSUser* pcUser);
		//新手引导补允许顶号
	public:
		bool					CheckIfInGuideBattle(CCSUser* pUser);

	//CDKey Events DB Function
	public:
		void					InsertCDKeyEvent(Buffer*& pBuffer, IDBConnector* pDB);
		void					QueryCDKeyEventsInfo(Buffer*& pBuffer, IDBConnector* pDB);
		void					UpdateCDKeyEvent(Buffer*& pBuffer, IDBConnector* pDB);

		void					InsertCDKeySQL(string cdkey, string &sql_str);
		void					InsertCDKey(Buffer*& pBuffer, IDBConnector* pDB);
		void					UpdateCDKeySQL(const string user_nam, const string cdkey, string &sql_str);
		void					UpdateCDKey(Buffer*& pBuffer, IDBConnector* pDB);

	public:
		void					UpdateMailState2DB(INT64 objId, INT64 mailId, INT32  cstate);

		INT32					GetCurtTotalServerUser()					{ return m_cUserGUIDMap.size(); }
};

}
