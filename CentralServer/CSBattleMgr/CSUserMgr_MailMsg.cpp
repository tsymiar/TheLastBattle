// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。
#include "stdafx.h"
#include "CSUserMgr.h"
#include "CSCfgMgr.h"
#include "DBMgr.h"
#include "../ELogging/ELogging/SSActive.h"
#include "CSKernel.h"
#include "FBAll_Const.h"
 

using namespace std;
//using namespace MysqlDWrapper;
namespace CentralServer{  

	void	CCSUserMgr::DBAsyn_QueryGameMailList(/*MysqlDWrapper::*/IDBConnector *pConn,INT64 objIdx)
	{ 
		assert(NULL == pConn); 

		stringstream mystream;
		mystream.str("");
		mystream <<"select *  from game_mail where mail_del_state<> "<< eMailState_Del; 
		if( objIdx > 0)
		{ 
			mystream << " and mail_user_id = ";
			mystream << objIdx; 
		}
		else
		{
			mystream << " and (mail_user_id is NULL or mail_user_id < 1) "; 
		}
		mystream <<" AND unix_timestamp(mail_over_time ) > unix_timestamp(NOW()) order by mail_id DESC;";

		INT32 	n32QueryRet = pConn->ExecQuery(mystream.str());
		if (n32QueryRet != eNormal)
		{
			ELOG(LOG_WARNNING,  "--errorCode:%d,errorStr:%s", pConn->GetErrorCode(),pConn->GetErrorStr());
			return;
		}  

		UINT32 un32RowNum = pConn->GetQueryRowNum(); 
		for ( int i=0; i< un32RowNum; i++)
		{
			MailDBData mailDb;
			pConn->GetQueryFieldData("mail_id", mailDb.mailId);
			INT32 nValue =0;
			pConn->GetQueryFieldData("mail_type", nValue);
			mailDb.mailType = (EMailType)nValue;  
			 
			pConn->GetQueryFieldData("mail_sdk", mailDb.channelId); 

			pConn->GetQueryFieldData("mail_title", mailDb.mailTitle);
			pConn->GetQueryFieldData("mail_content", mailDb.mailContent);
			pConn->GetQueryFieldData("mail_gift", mailDb.mailGift); 
			pConn->GetQueryFieldData("mail_send", mailDb.szSender); 
			pConn->GetQueryFieldData("mail_create_time", mailDb.mCreateTime);  
			pConn->GetQueryFieldData("mail_over_time", mailDb.mEndTime);  

			if (objIdx > 0){
				mailDb.objIdx =  objIdx; 
			}

			DBToCS::MailCallBack pMsg;
			pMsg.set_mailid(mailDb.mailId);
			pMsg.set_mailtype(mailDb.mailType);
			pMsg.set_channel(mailDb.channelId);
			pMsg.set_title(mailDb.mailTitle);
			pMsg.set_content(mailDb.mailContent);
			pMsg.set_gift(mailDb.mailGift);
			pMsg.set_sender(mailDb.szSender);
			pMsg.set_createtime(mailDb.mCreateTime);
			pMsg.set_objid(mailDb.objIdx); 
			GetCSUserMgrInstance()->EncodeAndSendToLogicThread(pMsg, pMsg.mgsid() ); 

			pConn->QueryNext();
		} 

		pConn->CloseQuery(); 
	}

	INT32	CCSUserMgr::GetUserShortGiftMail(/*MysqlDWrapper::*/IDBConnector *pConn, SUserDBData & sUserData,  DBToCS::QueryUser& sQueryUser)
	{
		assert(pConn == NULL  ); 
		stringstream mystream; 
		mystream <<"select mail_id,mail_state from gameuser_mail where user_id = " << sUserData.sPODUsrDBData.un64ObjIdx <<" order by mail_id DESC;";  
		INT32 	n32QueryRet = pConn->ExecQuery(mystream.str());
		if (n32QueryRet != eNormal)
		{
			ELOG(LOG_WARNNING,  "--errorCode:%d,errorStr:%s", pConn->GetErrorCode(),pConn->GetErrorStr());
			return n32QueryRet;
		}  
		UINT32 un32RowNum = pConn->GetQueryRowNum(); 
		for ( int i=0; i< un32RowNum; i++)
		{
			INT32 mailId = 0;
			INT32 mailState = 0;
			pConn->GetQueryFieldData("mail_id",mailId);
			pConn->GetQueryFieldData("mail_state",mailState);

			auto mailInfo = sQueryUser.add_mail_info();
			mailInfo->set_mailid(mailId);
			mailInfo->set_state(mailState);

			pConn->QueryNext();
		} 

		pConn->CloseQuery();

		DBAsyn_QueryGameMailList(pConn,sUserData.sPODUsrDBData.un64ObjIdx );

		return eNormal;
	} 

	void	CCSUserMgr::PostUserCurtMailList(CCSUser* pcUser)
	{
		assert(pcUser == NULL);
		GSToGC::NotifyMailList pMsg;
		bool bFlag = m_MailMgr.getPerMailListStr(pcUser->GetUserDBData() ,pMsg);
		if (bFlag)
		{
			pcUser->PostMsgToGC(pMsg,pMsg.msgid());
		}
	}

	void	CCSUserMgr::PostTheNewMail(const MailDBData & mail)
	{
		INT64 curt = time(NULL);
		if (  mail.n64CreateTime > curt  )
		{
			return;
		}
		//个人
		if (mail.objIdx > 0)
		{ 
			auto itr =	m_cUserOnlineMap.find( mail.objIdx );
			if (itr != m_cUserOnlineMap.end())
			{
				CCSUser *pcUser =  itr->second;
				if (NULL == pcUser)						return;  
				if( pcUser->GetUserDBData().sPODUsrDBData.tRegisteUTCMillisec > mail.n64CreateTime )						 return; 

				if( pcUser->GetUserDBData().sPODUsrDBData.eUserPlatform == mail.channelId || mail.channelId == ePlatform_All)
				{
					GSToGC::NotifyMailList msg;
					GSToGC::ShortMail *shortMail = msg.add_maillist();
					shortMail->set_mailtype((GSToGC::MailType)mail.mailType);
					shortMail->set_mailtitle(mail.mailTitle);
					shortMail->set_mailid(mail.mailId);
					shortMail->set_mailstate( GSToGC::eMailState_New);
					pcUser->PostMsgToGC(msg,msg.msgid());

					m_MailMgr.addHasSendMailId( mail.objIdx, mail.mailId);
				} 
			}  
		}
		else
		{//群众
			GSToGC::NotifyMailList msg;
			GSToGC::ShortMail *shortMail = msg.add_maillist();
			shortMail->set_mailtype((GSToGC::MailType)mail.mailType);
			shortMail->set_mailtitle(mail.mailTitle);
			shortMail->set_mailid(mail.mailId);
			shortMail->set_mailstate( GSToGC::eMailState_New);

			auto itr =	m_cUserOnlineMap.begin();
			for (;itr != m_cUserOnlineMap.end(); itr++)
			{
				CCSUser *pcUser =  itr->second;
				if  (NULL == pcUser) continue;  
				if	( pcUser->GetUserDBData().sPODUsrDBData.tRegisteUTCMillisec > mail.n64CreateTime)  continue; 
				 
				if (pcUser->GetUserDBData().sPODUsrDBData.eUserPlatform == mail.channelId || mail.channelId == ePlatform_All) 
				{
					itr->second->PostMsgToGC(msg,msg.msgid());
					m_MailMgr.addHasSendMailId(pcUser->GetUserDBData().sPODUsrDBData.un64ObjIdx, mail.mailId); 
				} 
			}
		} 
	}

	void		CCSUserMgr::Sync_SaveGameMail()
	{
		 CSToDB::UpdateGameMail msg;
		 bool bFlag =  m_MailMgr.UpdateGameMail(msg );
		 if (bFlag)
		 {
			  GetNowWorkActor().EncodeAndSendToDBThread(msg, msg.mgsid());
		 } 
	}

	void		CCSUserMgr::DBAsynUpdateGameMail(Buffer*& pBuffer, /*MysqlDWrapper::*/IDBConnector* pConn)
	{
		assert(NULL == pConn);

		boost::shared_ptr<CSToDB::UpdateGameMail> pMsg = ParseProtoMsgWithNewMemory<CSToDB::UpdateGameMail>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());
		if (!pMsg.get()){
			ELOG(LOG_ERROR, "");
			return;
		}  
		
		INT32 n32Update = pConn->ExecQuery("begin; set autocommit=0;");
		if (n32Update != eNormal)
		{
			ELOG(LOG_WARNNING,  "error: crrorcode:%d, msg:%s!", pConn->GetErrorCode(), pConn->GetErrorStr());
			return ;
		} 

		stringstream mystream;
		INT32 total = pMsg->maillist().size();
		for (int i=0; i< total; i++)
		{  
			CSToDB::GameMailInfo  mail = pMsg->maillist().Get(i);
			if (mail.curtstate() == CSToDB::EMailCurtState::eMailState_Del)
			{
				mystream << "update game_mail set mail_del_state =";
				mystream << mail.curtstate();
				mystream <<"  where  mail_id=";
				mystream << mail.mailid();
				mystream << ";";
			} 
			else if (mail.curtstate() == CSToDB::EMailCurtState::eMailState_New)
			{
				mystream << "insert into game_mail(mail_id,mail_sdk,mail_type,mail_user_id,mail_title,mail_content,mail_gift,mail_send,mail_create_time,mail_over_time,mail_del_state) values(";
				mystream << mail.mailid() <<",";
				mystream << mail.sdkidx() <<",";
				mystream << mail.type() <<",";
				mystream << mail.userid()<<",'";
				mystream << mail.title() <<"','";
				mystream << mail.content() <<"','";
				mystream << mail.giftstr() <<"','";
				mystream << mail.sender() <<"','";  
				mystream << mail.createtime() <<"','";
				mystream <<  mail.overtime() <<"',"; 
				mystream <<  mail.curtstate();
				mystream <<  ");";
			} 
		}
		
		if(total > 0)
		{  
			n32Update = pConn->ExecQuery(mystream.str());
			if (eNormal != n32Update){ 
				ELOG(LOG_WARNNING,  "error code %d , msg %s!",  pConn->GetErrorCode(), pConn->GetErrorStr());
				return ;
			} 
			n32Update = pConn->ExecQuery("commit;");
			if (eNormal != n32Update){ 
				ELOG(LOG_WARNNING,  "error code %d , msg %s!",  pConn->GetErrorCode(), pConn->GetErrorStr());
				return ;
			}  
			pConn->CloseQuery();
		}  
	}

	void	CCSUserMgr::DBAsynUpdateUserGameMail(Buffer*& pBuffer, IDBConnector* pConn)
	{
		boost::shared_ptr<CSToDB::UpdateUserMail> pMsg = ParseProtoMsgWithNewMemory<CSToDB::UpdateUserMail>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());
		if (!pMsg.get()){ 
			return;
		}  
		stringstream mystream;
		mystream << "select id from gameuser_mail where mail_id=";
		mystream << pMsg->mailid();
		mystream << " and user_id=";
		mystream << pMsg->objid();
		mystream << ";";
		INT32 n32Update = pConn->ExecQuery(mystream.str());
		if (n32Update != eNormal)
		{
			ELOG(LOG_WARNNING,  "error: crrorcode:%d, msg:%s!", pConn->GetErrorCode(), pConn->GetErrorStr());
			return ;
		} 
		mystream.clear(); mystream.str("");
		INT32 ntotal = 0;
		pConn->GetQueryFieldData("id",ntotal);
		pConn->CloseQuery();
		if (ntotal > 0 )
		{
			mystream << "update   gameuser_mail set mail_state=";
			mystream << pMsg->cstate();
			mystream << " where mail_id=";
			mystream << pMsg->mailid();
			mystream << " and user_id=";
			mystream << pMsg->objid();
			mystream << ";";
		}
		else
		{
			mystream << "insert into  gameuser_mail (mail_id,user_id, mail_state) values(";
			mystream << pMsg->mailid();
			mystream<<",";
			mystream << pMsg->objid();
			mystream<<",";
			mystream << pMsg->cstate();
			mystream << ");";
		}
		
		n32Update = pConn->ExecQuery(mystream.str());
		if (n32Update != eNormal)
		{
			ELOG(LOG_WARNNING,  "error: crrorcode:%d, msg:%s!", pConn->GetErrorCode(), pConn->GetErrorStr());
			return ;
		} 
	}

	//有点费，每次都要跑，后边优化
	 void		CCSUserMgr::CheckPostDelayNewMail(CCSUser* pUser)
	 {
		 GSToGC::NotifyMailList pMsg;
		 bool bFlag = false;

		 INT64 ctime = time(NULL);
		 
		 for (auto itr = m_MailMgr.getCurtGameMail().begin(); itr != m_MailMgr.getCurtGameMail().end(); ++itr)
		 { 
			 if ( itr->second.curtState == eMailState_Del )
			 {
				 continue;
			 }
			 if (ctime < itr->second.n64CreateTime)
			 {
				 continue;
			 }
			  //增加内存判断是否过期
			 if (  ctime > itr->second.n64EndTime  ) 
			 {
				 if(itr->second.objIdx > 0)  UpdateMailState2DB(itr->second.objIdx,itr->first,eMailState_Del); 
				 itr->second.curtState = eMailState_Del;
				 continue;  
			 }
			 
			 if (itr->second.objIdx > 0 && itr->second.objIdx != pUser->GetUserDBData().sPODUsrDBData.un64ObjIdx)
			 {
				  continue;
			 }

			 bool bRet = m_MailMgr.checkIfMailHasSend(pUser->GetUserDBData().sPODUsrDBData.un64ObjIdx ,itr->first);
			 if (bRet)
			 {
				 continue;
			 }
			
			 if (itr->second.n64CreateTime < pUser->GetUserDBData().sPODUsrDBData.tRegisteUTCMillisec) 
			 {
				  continue;
			 }

			 bRet = m_MailMgr.checkIfMailHasRecv(pUser->GetUserDBData().sPODUsrDBData.un64ObjIdx ,itr->first);
			 if (bRet)
			 {
				 continue;
			 }
			 if ((itr->second.channelId == ePlatform_All ) || (itr->second.channelId == (INT32)pUser->GetUserDBData().sPODUsrDBData.eUserPlatform )   )
			 {
				 GSToGC::ShortMail* shortMail = pMsg.add_maillist();
				 shortMail->set_mailid(itr->first);
				 shortMail->set_mailtype( ( GSToGC::MailType)itr->second.mailType);
				 shortMail->set_mailtitle(itr->second.mailTitle);
				 shortMail->set_mailstate((GSToGC::EMailCurtState)itr->second.curtState); 

				 m_MailMgr.addHasSendMailId(pUser->GetUserDBData().sPODUsrDBData.un64ObjIdx ,itr->first); 
				 bFlag = true;
			 }  
		 }
		 if (bFlag)
		 {
			 pUser->PostMsgToGC(pMsg, pMsg.msgid());
		 } 
	 }

	 void		CCSUserMgr::SynHandleMailCallback(Buffer*buffer)
	 {
		 boost::shared_ptr<DBToCS::MailCallBack> pMsg = ParseProtoMsgInThread<DBToCS::MailCallBack>(buffer->GetDataHeader(), buffer->GetDataLength());
		 if (NULL == pMsg){
			 ELOG(LOG_ERROR, "SynHandleMailCallback");
			 return;
		 }

		 MailDBData mailDb;
		 mailDb.mailId = pMsg->mailid(); 
		 mailDb.mailType = (EMailType)pMsg->mailtype();   
		 mailDb.channelId = pMsg->channel();  
		 mailDb.mailContent = pMsg->content();
		 mailDb.mailTitle  = pMsg->title();
		 mailDb.mailGift = pMsg->gift();
		 mailDb.szSender = pMsg->sender();
		 mailDb.mCreateTime = pMsg->createtime();
		 mailDb.n64CreateTime =   CFunction::FormatTime2TimeT(mailDb.mCreateTime.c_str());
		 mailDb.n64EndTime = CFunction::FormatTime2TimeT(mailDb.mEndTime.c_str());
		 
		 mailDb.objIdx =  pMsg->objid(); 
		   
		 m_MailMgr.addGameMail(mailDb);
	 }

	 void		CCSUserMgr::UpdateMailState2DB(INT64 objId, INT64 mailId, INT32  cstate)
	 {
		 CSToDB::UpdateUserMail sUpdateUser;
		 sUpdateUser.set_objid(objId);   
		 sUpdateUser.set_mailid(mailId);
		 sUpdateUser.set_cstate(cstate);

		 GetNowWorkActor().EncodeAndSendToDBThread(sUpdateUser, sUpdateUser.msgid() );
	 }
}