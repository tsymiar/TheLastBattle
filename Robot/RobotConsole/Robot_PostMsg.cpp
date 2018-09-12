#include "stdafx.h"
#include "Robot.h"
#include "RobotMgr.h"
#include "Hero.h"
#include "RobotNPC.h"
namespace ReBot
{

//pb±ä¶¯
#define AskInsideMsgMarcos return m_pRobotMgr->PostMsg(m_n32NSID, pMsg, pMsg.msgnum());

INT32 CCClient::AskCompleteUserInfo(const CHAR *cpszNickName, UINT32 un32HeaderID, INT8 n8Sex){
	m_eAskState = RS_AskCompleteUserInfo;
	GCToCS::CompleteInfo pMsg;
	pMsg.set_nickname(cpszNickName);
	pMsg.set_headid(un32HeaderID);
	pMsg.set_sex(n8Sex);
	m_pRobotMgr->PostMsg(m_n32NSID, pMsg, pMsg.msgnum());
	return eNormal;
}

INT32 CCClient::AskAlwaysAutoAtk(INT8 n8Flag){
	GCToSS::AlwaysAutoAtk pMsg;
	pMsg.set_flag(n8Flag);
	AskInsideMsgMarcos
}

INT32 CCClient::AskAbsorb(){
	GCToSS::Absorb pMsg;
	AskInsideMsgMarcos
}


INT32 CCClient::AskReportAltarStrawSolder(int n32MonsterID){
	GCToSS::ReportAltarStrawSolder pMsg;
	pMsg.set_type(n32MonsterID);
	if (m_pPlayer->m_eGOCamp %  2){
		pMsg.set_index(54);
	}
	else{
		pMsg.set_index(53);
	}
	AskInsideMsgMarcos
}

INT32 CCClient::AskCreateMatchTeam(UINT32 mapid, EBattleMatchType matchType){
	m_eAskState = RS_AskCreateMatchTeam;
	GCToCS::AskCreateMatchTeam msg;
	msg.set_mapid(mapid);
	msg.set_matchtype(matchType);
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskStartMatch(){
	m_eAskState = RS_AskStartMatch;
	GCToCS::AskStartMatch msg;
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

//////////////////////////////////////////////////////////////////////////

INT32 CCClient::AskReEnterRoom(){
	m_eAskState = RS_AskReEnterRoom;
	GCToCS::AskReEnterRoom msg;
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskMatchTeamList(){
	m_eAskState = RS_AskMatchTeamList;
	GCToCS::RequestMatchTeamList msg;
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskRoomList(){
	m_eAskState = RS_AskRoomList;
	GCToCS::AskRoomList msg;
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}


INT32 CCClient::AskCreateRoom(){
	m_eAskState = RS_AskCreateRoom;
	GCToCS::AskCreateRoom msg;
	msg.set_mapid(1002);
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskAddRoom(UINT32 room){
	m_eAskState = RS_AskAddRoom;
	GCToCS::AskAddRoom msg;
	msg.set_battleid(room);
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskLeaveRoom(){
	m_eAskState = RS_AskLeaveRoom;
	GCToCS::AskLeaveRoom msg;
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskChangeRoomSeat(INT32 n32NewSeat){
	m_eAskState = RS_AskChangeRoomSeat;
	GCToCS::AskChangeRoomSeat msg;
	msg.set_newpos(n32NewSeat);
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskReadyRoom(){ 
	m_eAskState = RS_AskReadyRoom;
	GCToCS::AskReadyRoom msg;
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskCancelRoom(){
	m_eAskState = RS_AskCancelReadyRoom;
	GCToCS::AskCancelRoom msg;
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskStartRoom(){	
	m_eAskState = RS_AskStartRoom;
	GCToCS::AskStartRoom msg;
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

//////////////////////////////////////////////////////////////////////////

INT32	CCClient::AskEnterBattle(UINT64 un64BattleID){
	ELOG(LOG_SpecialDebug,"robot(%u) AskEnterBattle",m_un32RobotID);
	m_eAskState = RS_AskEnterBattle;
	Assert(un64BattleID>0);
	GCToSS::AskEnterBattle pMsg;
	pMsg.set_battleid(un64BattleID);
	m_pRobotMgr->PostMsg(m_n32NSID, pMsg, pMsg.msgnum());
	return eNormal;
}

INT32	CCClient::AskGMCmd(const char* gmCmd){
	m_eAskState = RS_AskGMCmd;
	GCToSS::GMCmd msg;
	msg.set_cmd(gmCmd);
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskTrySelectHero(){
	ELOG(LOG_SpecialDebug,"robot(%u) AskTrySelectHero",m_un32RobotID);
	m_eAskState = RS_AskTryToSelectHero;
	if (m_HeroVec.empty()) return eNormal;
	INT32 heroId = m_pRobotMgr->GetRandIndexOnce(m_HeroVec.size());
	GCToSS::TrySeleceHero msg;
	msg.set_heroid(10000+heroId);
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32 CCClient::AskSelectHero(){
	ELOG(LOG_SpecialDebug,"robot(%u) AskSelectHero",m_un32RobotID);
	m_eAskState = RS_AskSelectHero;
	if (m_HeroVec.empty()) return eNormal;
	INT32 heroId = m_pRobotMgr->GetRandIndexOnce(m_HeroVec.size());
	GCToSS::SelectHero msg;
	msg.set_heroid(10000+heroId);
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}

INT32	CCClient::AskLoadMapComplete(){
	m_eAskState = RS_AskLoadMapComplete;
	GCToSS::LoadComplete msg;
	m_pRobotMgr->PostMsg(m_n32NSID, msg, msg.msgnum());
	return eNormal;
}
/************************************************************************/
/* Msg To SS                                                            */
/************************************************************************/

INT32 CCClient::AskUseSkill(UINT32 un32SkillID){
	GCToSS::UseSkill pMsg;
	pMsg.set_skillid(un32SkillID);
	AskInsideMsgMarcos
}


INT32 CCClient::AskReward(){
	GCToCS::UserAskGetCLReward pMsg;
	AskInsideMsgMarcos
}


INT32 CCClient::AskAutoAtk(){
	GCToSS::AutoAtk pMsg;
	//AskInsideMsgMarcos
	return 0;
}

INT32	CCClient::AskMoveDir(const CVector3D& cDir){
	GCToSS::MoveDir pMsg;
	GCToSS::Dir* pDir=new GCToSS::Dir();
	pDir->set_x(cDir.m_fX);
	pDir->set_z(cDir.m_fZ);
	pMsg.set_allocated_dir(pDir);
	AskInsideMsgMarcos
}


INT32 CCClient::AskBuyGoods(INT32 n32Goods){
	GCToSS::BuyGoods pMsg;
	pMsg.set_typeid_(n32Goods);
	AskInsideMsgMarcos
}

INT32 CCClient::AskUseGoods(UINT8 un8Pos){
	GCToSS::UseGoods pMsg;
	pMsg.set_goodspos(un8Pos);
	AskInsideMsgMarcos
}

INT32 CCClient::AskSellGoods(UINT8 un8Pos){
	GCToSS::SellGoods pMsg;
	pMsg.set_goodspos(un8Pos);
	AskInsideMsgMarcos
}

INT32 CCClient::AskMoveGoods(UINT8 un8FromPos, UINT8 un8ToPos){
	GCToSS::MoveGoods pMsg;
	pMsg.set_frompos(un8FromPos);
	pMsg.set_topos(un8ToPos);
	AskInsideMsgMarcos
}

INT32 CCClient::AskLockTar(const SGUID_KEY& sGUID){
	//GCToSS::LockTar pMsg;
	//pMsg.set_guid(sGUID.un64GUID);
	//AskInsideMsgMarcos
	return eNormal;
}

INT32	CCClient::AskStopMove(){
	GCToSS::StopMove pMsg;
	AskInsideMsgMarcos
}

INT32 CCClient::AskMovePos(const CVector3D& cPos){
	GCToSS::MoveTar pMsg;
	GCToSS::Pos* pPos=new GCToSS::Pos();
	pPos->set_x(cPos.m_fX);
	pPos->set_z(cPos.m_fZ);
	pMsg.set_allocated_pos(pPos);
	AskInsideMsgMarcos
}

//////////////////////////////////////////////////////////////////////////

INT32 CCClient::EmsgToLs_AskLogin( UINT platform,string uin,string sessionid )
{
	ELOG(LOG_SpecialDebug,"user(%s)(%s)(%u) ask login ls",m_szUserName.c_str(),m_szUserPwd.c_str(),m_n32NSID);
	Assert(mLoginState==RLS_LsConnected);
	mLoginState = RLS_LsAskLogin;
	GCToLS::AskLogin pMsg ; 
	pMsg.set_platform(platform);
	pMsg.set_uin(uin);
	pMsg.set_sessionid(sessionid);
	return m_pRobotMgr->PostMsg(m_n32NSID, pMsg, pMsg.msgid(), eServerLink_LS);
}

INT32 CCClient::EmsgToBs_AskLogin( UINT platform,string uin,string sessionid )
{
	ELOG(LOG_SpecialDebug,"user(%s)(%s)(%u) ask login bs",m_szUserName.c_str(),m_szUserPwd.c_str(),m_n32NSID);
	Assert(mLoginState==RLS_BsConnected);
	mLoginState = RLS_BsAskLogin;
	GCToBS::OneClinetLogin pMsg;
	pMsg.set_plat(platform);
	pMsg.set_uin(uin);
	pMsg.set_sessionid(sessionid); 	
	pMsg.set_login_success(1);
	pMsg.set_nsid(0);
	return m_pRobotMgr->PostMsg(m_n32NSID, pMsg, pMsg.msgnum(), eServerLink_BS);
}

INT32 CCClient::EmsgToGs_AskLogin( UINT platform,string name,string pass,string equip )
{
	Assert(mLoginState==RLS_GsConnected);
	mLoginState = RLS_GsAskLogin;
	GCToCS::Login pMsg;
	pMsg.set_platform(platform);
	pMsg.set_name(name);
	pMsg.set_passwd(m_sToken);
	pMsg.set_equimentid(equip);
	return m_pRobotMgr->PostMsg(m_n32NSID, pMsg, pMsg.msgnum());
}



}
