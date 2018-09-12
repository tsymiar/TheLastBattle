// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUserMgr.h"
#include "CSUser.h"

#include "CSBattleMgr.h"
#include "CSKernel.h"
 

namespace CentralServer{

INT32 CCSUserMgr::OnMgrFromRC_AskServerUserInfo(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength){
	CSToRC::ServerUserInfo respMsg;
	respMsg.set_allusernum(m_cUserGUIDMap.size());
	GetCSKernelInstance()->PostMsgToRC(cpiRCInfo, respMsg, respMsg.msgid());
	return eNormal;
}

INT32 CCSUserMgr::OnMgrFromRC_AskNoticeInfo(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<RCToCS::AskNoticeInfo> sMsp = ParseProtoMsg<RCToCS::AskNoticeInfo>(pData, n32DataLength);
	if (!sMsp){
		return eNormal;
	}

	return PostAllNoticeInfo(cpiRCInfo);
	//return eNormal;
}

INT32 CCSUserMgr::PostAllNoticeInfo(const ICSRCInfo *cpiRCInfo){
// 	CSToRC::AllNoticeInfo sMsg;
// 	for(int type = eNotice_All; type <= eNotice_2; type++){
// 		SNotice notice = m_NoticeMgr.GetNoticeByType((ENoticeType)type);
// 		if(notice.notice.length() > 0){
// 			CSToRC::NoticeInfo* pNoticeMsg = sMsg.add_notices();
// 			pNoticeMsg->set_notice(notice.notice);
// 		}
// 	}
// 	if(sMsg.notices_size() > 0)
// 		return GetCSKernelInstance()->PostMsgToRC(cpiRCInfo, sMsg, sMsg.msgid());
// 	else
		return eNormal;
}

INT32 CCSUserMgr::OnMgrFromRC_AskAddNotice(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<RCToCS::AskAddNotice> sMsp = ParseProtoMsg<RCToCS::AskAddNotice>(pData, n32DataLength);
	if (!sMsp){
		return eNormal;
	}

	SNotice notice;
	notice.title = sMsp->title();
	notice.flag = (ENoticeFlag)sMsp->flag();
	notice.msg = sMsp->noticemsg();
	notice.state = (ENoticeState)sMsp->state();
	notice.star_time = sMsp->star_time();
	notice.end_time = sMsp->end_time();
	notice.platform = (EUserPlatform)sMsp->platform();
	notice.priority = sMsp->priority();

	bool reflag = AddNotice(notice);

	if(reflag){//当前添加公告一定成功，留给以后可能失败的逻辑

		string sql_str;
		InsertNoticeSQL(notice, sql_str);
		CSToDB::InsertNotice temp_notice;
		temp_notice.set_sql_str(sql_str);
		m_UserCacheDBActiveWrapper->EncodeAndSendToDBThread(temp_notice, CSToDB::eInsertNotice_DBCall);

		CSToRC::AddNoticeResp respMsg;
		respMsg.set_rst(true);
		GetCSKernelInstance()->PostMsgToRC(cpiRCInfo, respMsg, respMsg.msgid()); 

		//发送最新的公告列表
		return PostAllNoticeInfo(cpiRCInfo); 
	}
	else{
		CSToRC::AddNoticeResp respMsg;
		respMsg.set_rst(false);
		GetCSKernelInstance()->PostMsgToRC(cpiRCInfo, respMsg, respMsg.msgid());
	}

	return eNormal;
}

INT32 CCSUserMgr::OnMgrFromRC_AskInsertCDKey(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength)
{
	boost::shared_ptr<RCToCS::CreateCDKey> sMsp = ParseProtoMsg<RCToCS::CreateCDKey>(pData, n32DataLength);
	if (!sMsp){
		ELOG(LOG_ERROR, "");
		return eNormal;
	}

	auto pReids = GetLogicRedisHandler();
	if (!pReids){
		ELOG(LOG_ERROR, "Redis未连接！");
	}

	string title = sMsp->title();
	EUserPlatform plat = (EUserPlatform)sMsp->platform();
	UINT64 end_time = sMsp->end_time();
	string describe = sMsp->content();
	INT32 gift_type = sMsp->gift_type();
	INT32 code_len = sMsp->code_len();
	INT32 code_num = sMsp->cdkey_size();

	//计算出过期时间(单位：秒)
	INT64 expiredSeconds = end_time - time(NULL);
	if (expiredSeconds < 0){
		GetCDKeySurplusSec(expiredSeconds, end_time);
	}

	stringstream tempStream;
	for (int i = 0; i < sMsp->gift().size(); i ++)
	{
		auto itemID = sMsp->gift(i).item_id();
		auto itemNum = sMsp->gift(i).item_num();
		tempStream << itemID << "," << itemNum << ";";
	}

	++m_GiftCDKID;
	GetLogicRedisHandler()->redisAsyncCommand(nullptr, nullptr, "incr m_GiftCDKID");
	GetLogicRedisHandler()->redisAsyncCommand(nullptr, nullptr, "hmset cdkID:%lld describe %s gift_type %d items %s endtime %lld platfrom %d "
		"title %s", m_GiftCDKID, describe, gift_type, tempStream.str(), end_time, plat, title);
	ELOG(LOG_SpecialDebug, "增加CDKey ID:%lld 礼品：%s", m_GiftCDKID, tempStream.str());

	CSToDB::CDKeyEvents info;

	stringstream sql_ss;
	sql_ss<<"insert into cdkey_events(event_id, event_title, event_platform, event_end_time,\
			event_content, event_type, event_item, event_code_num, event_code_len)values(";
	sql_ss<<m_GiftCDKID;
	sql_ss<<",'";
	sql_ss<<title;
	sql_ss<<"',";
	sql_ss<<plat;
	sql_ss<<",";
	sql_ss<<end_time;
	sql_ss<<",'";
	sql_ss<<describe;
	sql_ss<<"',";
	sql_ss<<gift_type;
	sql_ss<<",'";
	sql_ss<<tempStream.str();
	sql_ss<<"',";
	sql_ss<<code_num;
	sql_ss<<",";
	sql_ss<<code_len;
	sql_ss<<");";

	info.set_sql_str(sql_ss.str());
	m_CdkeyWrapper->EncodeAndSendToDBThread(info, info.msgid());

	string sql_str = "";
	for (int i = 0; i < sMsp->cdkey().size(); ++i)
	{
		InsertCDKeySQL(sMsp->cdkey(i), sql_str);
		CSToDB::InsertCDKeyInfo cdkey_info;
		cdkey_info.set_sql_str(sql_str);
		m_CdkeyWrapper->EncodeAndSendToDBThread(cdkey_info, cdkey_info.msgid());

		const string& sCDK = sMsp->cdkey(i);
		GetLogicRedisHandler()->redisAsyncCommand(nullptr, nullptr, "hmset cdk:%s GiftCDKID %d", sCDK.c_str(), m_GiftCDKID);
		//设置cdk过期时间
		GetLogicRedisHandler()->redisAsyncCommand(nullptr, nullptr, "EXPIRE cdk:%s %lld", sCDK.c_str(), expiredSeconds);
	}

	return eNormal;
}

INT32 CCSUserMgr::OnMgrFromRC_AskDeleteNotice(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength){

	return eNormal;
}

INT32	CCSUserMgr::OnMgrFromRC_AskAddMail(const ICSRCInfo *cpiRCInfo, const char* pData, int n32DataLength){ 
	boost::shared_ptr<RCToCS::AskAddMailReq> sMsp = ParseProtoMsg<RCToCS::AskAddMailReq>(pData, n32DataLength);
	if (!sMsp){
		return eNormal;
	} 

	MailDBData mail;
	mail.mailId = m_MailMgr.getCurtMailIdx();
	mail.bIfNewMail = true; 
	mail.mailType = (EMailType)sMsp->type();

	mail.mailContent = sMsp->content();
	mail.mailTitle  = sMsp->title();
	mail.mailGift = sMsp->gift();
	mail.szSender = sMsp->sender();
	mail.mCreateTime = sMsp->createtime();
	 
	mail.curtState = eMailState_New;
	mail.n64CreateTime =  CFunction::FormatTime2TimeT(mail.mCreateTime.c_str());
	INT64 curSecondTime = time(NULL);
	if (mail.n64CreateTime < curSecondTime){
		mail.n64CreateTime = curSecondTime;
	}

	mail.n64EndTime = mail.n64CreateTime + sMsp->day() * 24 * 3600;
	mail.mEndTime = CFunction::GetCurtFormatTime(mail.n64EndTime); 

	INT64 waitTime = mail.n64CreateTime - curSecondTime;
	INT64 mailExpiredTime = mail.n64EndTime - curSecondTime;

	ELOG(LOG_SpecialDebug, "邮件(id:%d)过期事件设置在Redis WaitTime:%d, ExpireTime:%lld(秒)", mail.mailId, waitTime, mailExpiredTime);
	auto redisHandler = GetLogicRedisHandler();
	if (redisHandler && redisHandler->CanbeUsed()){
		if (waitTime > 0){
			redisHandler->redisAsyncCommand(NULL, NULL, "set mailWait:%d 1", mail.mailId);
			redisHandler->redisAsyncCommand(NULL, NULL, "expire mailWait:%d %lld", mail.mailId, waitTime);
		}

		if (mailExpiredTime > 0){
			redisHandler->redisAsyncCommand(NULL, NULL, "set mailExpired:%d 1", mail.mailId);
			redisHandler->redisAsyncCommand(NULL, NULL, "expire mailExpired:%d %lld", mail.mailId, mailExpiredTime);
		}
	}

	mail.channelId = sMsp->sdk();
	
	if (sMsp->useridlist_size() > 0)
	{
		for(int i=0; i< sMsp->useridlist_size(); i++)
		{
			mail.mailId = m_MailMgr.getCurtMailIdx(); 
			mail.objIdx = sMsp->useridlist(i);
			m_MailMgr.addGameMail(mail);

			if (waitTime < 1){
				PostTheNewMail(mail);
			}
		} 
	}
	else
	{
		m_MailMgr.addGameMail(mail);

		if (waitTime < 1){
			PostTheNewMail(mail);
		}
	}

	CSToRC::AddMailResp msp;
	msp.set_rst(true);
	GetCSKernelInstance()->PostMsgToRC(cpiRCInfo, msp, msp.msgid());  
 
	//新增的同时 同事更新到数据库
	Sync_SaveGameMail();

	return eNormal;
}

}