
#include "stdafx.h"
#include "CSMailMgr.h"
#include "CSUser.h" 
#include "CSCfgMgr.h"
#include "boost/spirit.hpp" 
#include "boost/algorithm/string.hpp"
#include "CSUserMgr.h"

namespace CentralServer 
{

	CCSMailMgr::CCSMailMgr()
	{ 
		m_curtMaxMailIdx = 0;

		m_mapGameMail.clear();

		m_mapPersonalMail.clear();  

		m_IfHasSendMail.clear();
	}
	CCSMailMgr::~CCSMailMgr()
	{

	}  

	bool CCSMailMgr::addGameMail(     const MailDBData  & mail)
	{
		if ( mail.objIdx > 0)
		{
			addPersonalMail( mail);   
		}

		auto itr =	m_mapGameMail.find(mail.mailId);
		if (itr == m_mapGameMail.end())
		{
			m_mapGameMail[mail.mailId] = mail; 
			return true;
		}
		ELOG(LOG_ERROR,"error: there exist game  mailId:%d!",mail.mailId);
		 
		return false;
	}


	void CCSMailMgr::addPersonalMail( const MailDBData  & mail )
	{
		auto itr = m_mapPersonalMail.find(mail.objIdx);
		if (itr == m_mapPersonalMail.end())
		{
			std::map<INT32,tagMailState> tMap;

			tagMailState t;
			t.curtState = (EMailCurtState)mail.curtState; 
			t.bFlag = false;

			tMap[mail.mailId] = t;  

			m_mapPersonalMail[mail.objIdx] = tMap;
		}
		else
		{
			std::map<INT32,tagMailState>& tMap = itr->second;
			auto itrChild = tMap.find(mail.mailId);
			if (itrChild == tMap.end())
			{ 
				tagMailState t;
				t.bFlag = false;
				t.curtState = (EMailCurtState)mail.curtState;
				tMap[mail.mailId] = t;
			}  
		}
	}

	INT32 CCSMailMgr::getMailInfoById(INT64 objIdx,INT32 mailId,   GSToGC::MailInfo & mailInfo, INT32 &nstate)
	{
		assert(NULL== name || mailId < 1);

		auto itr = m_mapGameMail.find(mailId);
		if (itr != m_mapGameMail.end())
		{ 
			mailInfo.set_mailcontent(itr->second.mailContent);
			mailInfo.set_mailgift(itr->second.mailGift);
			mailInfo.set_sender(itr->second.szSender);

			std::string createTime(itr->second.mCreateTime);
			INT32  npos = createTime.find_first_of(' ');
			if (-1 != npos)
			{
				createTime = createTime.substr(0,npos);
			} 
			mailInfo.set_createtime(createTime);

			//设置邮件查看状态
			if (!itr->second.mailGift.empty())
			{
				updatePerMailList(mailId,objIdx,eMailState_LookedButNotGotGift);

				nstate = eMailState_LookedButNotGotGift;
			}
			else
			{
				updatePerMailList(mailId,objIdx,eMailState_Look);
				nstate = eMailState_Look;
			}  

			return eNormal;
		} 
		return eEc_MailHasTimeOver;
	}

	void CCSMailMgr::updatePerMailList( INT32 mailId,    INT64  objIdx  ,EMailCurtState eState)
	{
		assert(objIdx < 1);
		auto itr = m_mapPersonalMail.find(objIdx);
		if (itr != m_mapPersonalMail.end()  )
		{
			std::map<INT32, tagMailState> &tMap = itr->second;
			auto itrChild = tMap.find(mailId);
			if (itrChild != tMap.end())
			{
				itrChild->second.curtState = eState;
				itrChild->second.bFlag = true;
			} 
			else
			{ 
				tagMailState t;
				t.bFlag = true;
				t.curtState = eState;
				tMap[mailId] = t;
			}
		}
		else
		{ 
			std::map<INT32, tagMailState > tMap;
			tagMailState t;
			t.bFlag = true;
			t.curtState = eState;
			tMap[mailId] = t;
			m_mapPersonalMail[objIdx] = tMap; 
		}
	}


	bool CCSMailMgr::getPerMailListStr(const SUserDBData & userDb, GSToGC::NotifyMailList&  mailList) 
	{ 
		INT32 total = 0;  
		INT64  cut = time(NULL);

		std::map<INT32, MailDBData>::reverse_iterator	 itrGameMail =	m_mapGameMail.rbegin();
		for (;itrGameMail != m_mapGameMail.rend(); ++itrGameMail )
		{
			if (itrGameMail->second.mailId <1)  continue; 
			if( cut < itrGameMail->second.n64CreateTime) continue; 
			if (itrGameMail->second.curtState == eMailState_Del)  continue; 
			if (  userDb.sPODUsrDBData.tRegisteUTCMillisec > itrGameMail->second.n64CreateTime )  continue; 
			if (checkIfMailHasRecv(userDb.sPODUsrDBData.un64ObjIdx, itrGameMail->first))  	continue; 
			//增加内存判断是否过期
			if ( cut > itrGameMail->second.n64EndTime   )
			{
				if (itrGameMail->second.objIdx > 0)  GetCSUserMgrInstance()->UpdateMailState2DB(itrGameMail->second.objIdx,itrGameMail->first,eMailState_Del);
				 
				itrGameMail->second.curtState = eMailState_Del;
				continue;
			} 
			if (( itrGameMail->second.channelId == ePlatform_All) || (itrGameMail->second.channelId == (INT32)userDb.sPODUsrDBData.eUserPlatform )) 
			{
				if ( itrGameMail->second.objIdx > 0    )
				{
					auto itrPer =	m_mapPersonalMail.find(userDb.sPODUsrDBData.un64ObjIdx);
					if ( itrPer != m_mapPersonalMail.end() )
					{ 
						std::map<INT32,tagMailState> & tMap = itrPer->second;
						auto itrChilde = tMap.find(itrGameMail->first);
						if (  (itrChilde != tMap.end()) &&  (itrChilde->second.curtState !=  GSToGC::EMailCurtState::eMailState_Del)  ) 
						{   
							getShortMailMasg(userDb.sPODUsrDBData.un64ObjIdx,itrGameMail->first,itrGameMail->second.mailType,itrGameMail->second.mailTitle,itrGameMail->second.curtState,mailList);  
							total++;  
						} 
					} 
				} 
				else
				{ 
					getShortMailMasg(userDb.sPODUsrDBData.un64ObjIdx,itrGameMail->first,itrGameMail->second.mailType,itrGameMail->second.mailTitle,itrGameMail->second.curtState,mailList); 
					total++; 
				}  
			} 
			if (total > CCSCfgMgr::getInstance().GetCSGlobalCfg().m_MaxUserMailN)
			{
				return total > 0 ? true : false;
			}
		}

		return total > 0 ? true : false;
	}
	//邮件消息
	void CCSMailMgr::getShortMailMasg(INT64 objId, INT32 mailid,INT32 mailtype, std::string& mailtitle, INT32 mailState, GSToGC::NotifyMailList&  mailList)
	{
		GSToGC::ShortMail* shortMail = mailList.add_maillist();
		shortMail->set_mailid(mailid);
		shortMail->set_mailtype( ( GSToGC::MailType)mailtype);
		shortMail->set_mailtitle(mailtitle);
		shortMail->set_mailstate(( GSToGC::EMailCurtState)mailState);  

		addHasSendMailId(objId, mailid);
	}
	bool  CCSMailMgr::closeOrGetMailGift(CCSUser *pUser, INT32 mailId  )
	{   
		bool bFlag = false;

		auto itr =	m_mapGameMail.find(mailId);
		if (itr != m_mapGameMail.end()  )
		{
			if(!itr->second.mailGift.empty() && ((EUserPlatform)itr->second.channelId ==  pUser->GetUserDBData().sPODUsrDBData.eUserPlatform || itr->second.channelId == ePlatform_All))
			{
				std::string giftStr = string(itr->second.mailGift);
				std::vector<std::string> tVec;
				boost::algorithm::split( tVec, giftStr, boost::algorithm::is_any_of(gSignSplitIc) ); 
				int total = tVec.size();
				for (int i=0; i <total; i++)
				{
					std::string itemStr = tVec[i]; // 类型:赠送编号:数量
					std::vector<std::string> tItemVec;
					boost::algorithm::split( tItemVec, itemStr, boost::algorithm::is_any_of(gSignPoint) ); 
					if (tItemVec.size() == 3)
					{ 
						EMerchType merType = (EMerchType)atoi( tItemVec[0].empty() ? "0": tItemVec[0].c_str());
						INT32  giftId = atoi( tItemVec[1].empty() ? "0": tItemVec[1].c_str());
						INT32  total = atoi( tItemVec[2].empty() ? "0": tItemVec[2].c_str());

						if (merType == eMerchType_Gold)
						{
							pUser->GetUserDBData().ChangeUserDbData(eUserDBType_Gold,total );
							pUser->SynCurGold();
						}
						else if (merType == eMerchType_Diamond)
						{
							pUser->GetUserDBData().ChangeUserDbData(eUserDBType_Diamond,total ); 
							pUser->SynCurDiamond();
						}
						else if (merType == eMerchType_Goods)
						{
							for (int i=0; i< total; i++)
							{
								if (IfHeroCfgID(giftId)) 
									pUser->AddHero(giftId, true); 
								else if (IfRuneCfgID(giftId)) 
									 pUser->AddRunes(giftId, true);
								else if (IfOtherCfgID(giftId)){
									 pUser->ChangeOtherItemMap(giftId, 1);
								}
								else { Assert(false && "can't get this wards."); }
							}
						} 
						else
						{
							ELOG(LOG_WARNNING,"waiting!!!! mail have not add  gift, type:%d, account:%d", merType,pUser->GetUserDBData().szNickName);
							return false;
						}
					} 
				} 
			}  
			updatePerMailList(mailId,pUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,eMailState_Del);
			if (itr->second.objIdx == pUser->GetUserDBData().sPODUsrDBData.un64ObjIdx)
			{ 
				itr->second.bIfPerDel = true;
				return true;
			} 
		}  
		return false;
	}

	void CCSMailMgr::changeMailState(INT64 objId, INT32 mailId ,std::string &str, INT32 curtState)
	{ 
		stringstream mystream; 
		mystream <<"replace into gameuser_mail (mail_id,user_id,mail_state) values(";
		mystream <<mailId<<",";		
		mystream <<objId<<",";
		mystream << curtState ;
		mystream<<");"; 
		str = mystream.str();
	}


	bool	CCSMailMgr::UpdateGameMail(CSToDB::UpdateGameMail &msg)
	{
		INT32 total = 0;
		auto itr = m_mapGameMail.begin();
		for (; itr != m_mapGameMail.end(); itr++)
		{ 
			CSToDB::GameMailInfo *mail = msg.add_maillist();  
			mail->set_mailid(itr->first);
			if (itr->second.bIfNewMail)
			{ 
				mail->set_type((CSToDB::MailType)itr->second.mailType); 
				mail->set_userid(itr->second.objIdx);
				mail->set_title(itr->second.mailTitle);
				mail->set_content(itr->second.mailContent);
				mail->set_giftstr(itr->second.mailGift);
				mail->set_createtime(itr->second.mCreateTime);
				mail->set_overtime(itr->second.mEndTime);
				mail->set_curtstate((CSToDB::EMailCurtState)itr->second.curtState);//(CSToDB::EMailCurtState::eMailState_New);
				mail->set_sdkidx(itr->second.channelId);
				mail->set_sender(itr->second.szSender);

				itr->second.bIfNewMail = false; 
			}
			else if (itr->second.bIfPerDel)
			{
				mail->set_curtstate(CSToDB::EMailCurtState::eMailState_Del);
			}
			total++;
		}
		return total > 0 ? true : false;
	}

	void	CCSMailMgr::RemoveObjId(INT64 objIdx)
	{
		auto itr = m_mapPersonalMail.find(objIdx);
		if (itr != m_mapPersonalMail.end())
		{ 
			m_mapPersonalMail.erase(itr);
		}

		auto itrHasSendMail = m_IfHasSendMail.find(objIdx);
		if (itrHasSendMail != m_IfHasSendMail.end())
		{
			itrHasSendMail->second.clear();
			m_IfHasSendMail.erase(itrHasSendMail);
		}
	}

 
	void	CCSMailMgr::addHasSendMailId(INT64 objIdx, INT32 mailId)
	{
		auto itrHasSendMail = m_IfHasSendMail.find(objIdx);
		if (itrHasSendMail != m_IfHasSendMail.end())
		{
			std::map<INT32,bool> &tmap = itrHasSendMail->second;
			auto itrChild = tmap.find(mailId);
			if (itrChild == tmap.end())
			{
				tmap[mailId] = true;
			}
		}
		else
		{
			std::map<INT32,bool> tmap ;
			tmap[mailId] = true;
			m_IfHasSendMail[objIdx] = tmap;
		}
	}

	bool	CCSMailMgr::checkIfMailHasSend(INT64 objIdx, INT32 mailId)
	{
		auto itrHasSendMail = m_IfHasSendMail.find(objIdx);
		if (itrHasSendMail != m_IfHasSendMail.end())
		{
			std::map<INT32,bool> &tmap = itrHasSendMail->second;
			auto itrChild = tmap.find(mailId);
			if (itrChild != tmap.end())
			{
				return true;
			}
		}
		return false;
	}
	 
	void   CCSMailMgr::addPersonalMail(INT64 objIdx, vector<UserMailDBData> &pmail	)
	{
		INT32  total = pmail.size();
		for (int i=0; i< total; i++)
		{  
			 updatePerMailList( pmail[i].mailId,   objIdx  ,pmail[i].mState);
		}
	} 
	 

	std::map<INT32,tagMailState>* CCSMailMgr::GetUserMailState(UINT64 guid){
		auto iter = m_mapPersonalMail.find(guid);
		if (iter != m_mapPersonalMail.end()){
			return &iter->second;
		}

		return NULL;
	}

	bool   CCSMailMgr::checkIfMailHasTimeOver(INT32 mailId)
	{
		auto itr = m_mapGameMail.find(mailId);
		if (itr != m_mapGameMail.end() && itr->second.curtState == eMailState_Del)
		{
			return true;
		}
		return false;
	}

	bool   CCSMailMgr::checkIfMailHasRecv(INT64 objIdx, INT32 mailId)
	{
		auto itr = m_mapPersonalMail.find(objIdx);  
		if (itr != m_mapPersonalMail.end())
		{
			 std::map<INT32,tagMailState> &tmap = itr->second;
			 auto itrChild = tmap.find(mailId);
			 if (itrChild != tmap.end() && itrChild->second.curtState == eMailState_Del)
			 {
				 return true;
			 }
		}
		return false;
	}
}