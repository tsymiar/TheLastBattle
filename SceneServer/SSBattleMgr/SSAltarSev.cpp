
#include "stdafx.h"
#include "SSAltarSev.h"
#include "SSCfgMgr.h"

 namespace SceneServer{

CSSAltarSev::CSSAltarSev()
{
	//Release(); 
	m_AltarBornSolder.clear();
}
CSSAltarSev::~CSSAltarSev()
{
	 
}
//
//void CSSAltarSev::Release()
//{
//	m_cAltarBornNpcTimeConfig.clear();
//	m_cAltarLastBornNpcTime.clear();
//
//	m_cAltarIfAddAnimial.clear();
//}
//
//INT32 CSSAltarSev::GetAltarBornTimeSpaceConfig(UINT32 altarIdex)
//{
//	INT32 nRet = 0;
//	m_cAlatrBornNpcTimeConfigItr = 	m_cAltarBornNpcTimeConfig.find(altarIdex);
//	if (m_cAlatrBornNpcTimeConfigItr != m_cAltarBornNpcTimeConfig.end())
//	{
//		nRet = m_cAlatrBornNpcTimeConfigItr->second.n32BSpaceTimeCfg;
//	}
//	return nRet;
//}
//
// 
//
//INT32 CSSAltarSev::GetFirstBornDelayTimeCfg(UINT32 altarIdex)
//{
//	INT32 nRet = 0;
//	m_cAltarFirstBornNpcDelayTimeCfgItr = m_cAltarFirstBornNpcDelayTimeCfg.find(altarIdex);
//	if(m_cAltarFirstBornNpcDelayTimeCfgItr != m_cAltarFirstBornNpcDelayTimeCfg.end())
//	{
//		nRet = m_cAltarFirstBornNpcDelayTimeCfgItr->second;
//	}
//	return nRet;
//}
//
// 
//void CSSAltarSev::InitAltarConfig(UINT32 altarIdex ,UINT32 bornTimeSpace , UINT32 firstDelayTime,  UINT32 AltarMaxBornSolder)
//{  
//	tagBornSolderCfg tb(bornTimeSpace,AltarMaxBornSolder);
//	m_cAltarBornNpcTimeConfig[altarIdex] = tb;
//	//first delay time
//	m_cAltarFirstBornNpcDelayTimeCfg[altarIdex] = firstDelayTime;  
//}
//
//bool CSSAltarSev::DoAltarBornSolder(UINT32 altarIdex,TIME_MILSEC tUTCMilsec)
//{ 
//	m_cAltarLastBornNpcTimeItr = m_cAltarLastBornNpcTime.find(altarIdex);
//	if(  m_cAltarLastBornNpcTimeItr != m_cAltarLastBornNpcTime.end()  )
//	{   
//		if ( tUTCMilsec >  m_cAltarLastBornNpcTimeItr->second   )
//		{
//			return IfMaxBSNum(altarIdex  );  
//		}
//		else
//		{
//			return false;
//		}
//	} 
//	return true;
//}
//
//INT32 CSSAltarSev::DoAltarBornSolder(TIME_MILSEC tUTCMilsec, std::vector<INT32> &vAltar )
//{  
//	INT32 nRet = 0;
// 
//	m_cAltarIfAddAnimialItr = m_cAltarIfAddAnimial.begin();
//	for (; m_cAltarIfAddAnimialItr != m_cAltarIfAddAnimial.end(); m_cAltarIfAddAnimialItr++ )
//	{
//		if (tUTCMilsec < m_cAltarIfAddAnimialItr->second )
//		{
//			continue;
//		} 
//		INT32 altarIdex = m_cAltarIfAddAnimialItr->first;
//
//		bool bRet = DoAltarBornSolder(altarIdex,tUTCMilsec);
//		if (bRet)
//		{
//			vAltar.push_back(altarIdex);
//
//			INT32 timeSpaceConfig = GetAltarBornTimeSpaceConfig(altarIdex); 
//
//			m_cAltarLastBornNpcTime[altarIdex] = GetUTCMiliSecond() + timeSpaceConfig; 
//
//			nRet++;
//		} 
//	}
//	return nRet;  
//}
//
//void CSSAltarSev::ChangeAltarParam(UINT32 altarIdex,bool bF)
//{
//	if (bF )
//	{
//		ChangeAltarBSolderParam(altarIdex,false);
//		return;
//	}
//
//	m_cAltarLastBornNpcTimeItr = m_cAltarLastBornNpcTime.find(altarIdex);
//	if (m_cAltarLastBornNpcTimeItr != m_cAltarLastBornNpcTime.end())
//	{
//		m_cAltarLastBornNpcTime.erase(m_cAltarLastBornNpcTimeItr);
//	}
//	
//	m_cAltarIfAddAnimialItr = m_cAltarIfAddAnimial.find(altarIdex);
//	if (m_cAltarIfAddAnimialItr != m_cAltarIfAddAnimial.end())
//	{
//		m_cAltarIfAddAnimial.erase(m_cAltarIfAddAnimialItr);
//	}
//
//	//cfg
//	m_cAlatrBornNpcTimeConfigItr = m_cAltarBornNpcTimeConfig.find(altarIdex);
//	if (m_cAlatrBornNpcTimeConfigItr != m_cAltarBornNpcTimeConfig.end())
//	{
//		m_cAltarBornNpcTimeConfig.erase(m_cAlatrBornNpcTimeConfigItr);
//	}
//}
//
////¼ÀÌ³Ìí¼Ó±ø
//bool CSSAltarSev::DoAltarAddAnimial(UINT32 altarIdex, TIME_MILSEC tUTCMilsec)
//{ 
//	m_cAlatrBornNpcTimeConfigItr = m_cAltarBornNpcTimeConfig.find(altarIdex);
//	if(m_cAlatrBornNpcTimeConfigItr == m_cAltarBornNpcTimeConfig.end())
//	{
//		return false;
//	}
//
//	m_cAltarIfAddAnimialItr = m_cAltarIfAddAnimial.find(altarIdex);
//	if (m_cAltarIfAddAnimialItr == m_cAltarIfAddAnimial.end())
//	{
//		INT32 firstBornDelayT = GetFirstBornDelayTimeCfg(altarIdex);
//		m_cAltarIfAddAnimial[altarIdex] = tUTCMilsec + firstBornDelayT;
//	} 
//	return true;
//}
//
////bf: true -- as add
////bf: false --as minus
// INT32 CSSAltarSev::ChangeAltarBSolderParam(UINT32 altarIdex, bool bF)
// {
//	 m_cAlatrBornNpcTimeConfigItr = m_cAltarBornNpcTimeConfig.find(altarIdex);
//	 if (m_cAlatrBornNpcTimeConfigItr != m_cAltarBornNpcTimeConfig.end())
//	 {
//		 m_cAlatrBornNpcTimeConfigItr->second.ChangeParam(bF);
//	 } 
//	 return eNormal;
// }
//
// bool CSSAltarSev::IfMaxBSNum(UINT32 altarIdex )
// { 
//	 m_cAlatrBornNpcTimeConfigItr = m_cAltarBornNpcTimeConfig.find(altarIdex);
//	 if (  m_cAlatrBornNpcTimeConfigItr != m_cAltarBornNpcTimeConfig.end() )
//	 {  
//		 if ( (m_cAlatrBornNpcTimeConfigItr->second.n32CurtExistNum - m_cAlatrBornNpcTimeConfigItr->second.n32CurtDeadNum < m_cAlatrBornNpcTimeConfigItr->second.n32MaxExistNumCfg))
//		 {
//			 return  true; // 5 > 4 = false;
//		 }
//		 else
//		 {
//			return false;
//		 }
//	 } 
//	 return false;
// }
// 
// bool CSSAltarSev::IfBornNPC(UINT32 altarIdex)
// {
//	 m_cAltarLastBornNpcTimeItr = m_cAltarLastBornNpcTime.find(altarIdex);
//	 if (m_cAltarLastBornNpcTimeItr != m_cAltarLastBornNpcTime.end())
//	 {
//		 return true;
//	 }
//	 return false;
// }

	//Ìí¼Ó¼ÀÌ³ ¹ÖÎïid
	void  CSSAltarSev::AddAltarSolderIdx(INT32 altarId ,INT32 monsterId ,INT32 camId,INT64 mapId /*INT64 btime,const SAltarSolderCfg& cfg*/)
	{  
		auto itr = m_AltarBornSolder.find(altarId);
		if (itr != m_AltarBornSolder.end())
		{
			itr->second.monsterId = monsterId;
			/*itr->second.nextBornTime = btime;
			itr->second.campId = camId;
			itr->second.mCfg = cfg;*/
		} 
		else
		{
			SAltarSolderCfg cfg;
			CSSCfgMgr::GetSSCfgInstance().GetSSAltarSolderPosInfo(mapId,altarId,cfg); 

			CurtAltarBornSolder bs;
			bs.monsterId = monsterId;
			bs.nextBornTime = GetUTCMiliSecond() + CSSCfgMgr::GetSSCfgInstance().GetSSMapFuncDelayTimeCfg(eDelay_AltarSolder  , mapId );//btime;
			bs.campId = camId;
			bs.mCfg = cfg;

			m_AltarBornSolder[altarId] = bs;
		}
	}
	 

	 void	 CSSAltarSev::UpdateAltarBornSolder(INT32 altarId , INT64 btime )
	 {
		 auto itr = m_AltarBornSolder.find(altarId);
		 if ( itr != m_AltarBornSolder.end() )
		 {
			 itr->second.nextBornTime = btime;
			 itr->second.curtBornNum += 1;
		 }
	 }


	 void		 CSSAltarSev::RemoveAltarInfoWhenDead(INT32 altarId)
	 {
		 auto itr =	m_AltarBornSolder.find(altarId);
		 if (itr != m_AltarBornSolder.end())
		 {
			 m_AltarBornSolder.erase(itr);
		 }
	 }


	 void		 CSSAltarSev::MinusAltarSolderNumWhenDead(INT32 altarId )
	 {
		 auto itr = m_AltarBornSolder.find(altarId);
		 if ( itr != m_AltarBornSolder.end() )
		 {
			 itr->second.curtBornNum--;
		 }
	 }
}