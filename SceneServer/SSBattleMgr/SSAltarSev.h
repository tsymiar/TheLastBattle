
#ifndef SSALTAR_H_
#define SSALTAR_H_
#include <algorithm>
#include <map>

namespace SceneServer{

class CSSAltarSev 
{   
	/*typedef std::map<UINT32, tagBornSolderCfg>			ALTAR_BORNNPCTIMECONFIG; 
	typedef std::map<UINT32, tagBornSolderCfg>::iterator  BORNNPCTIMECONFIG_ITR;

	typedef std::map<UINT32, UINT32>			ALTAR_BORNNPC_DELAYTIME_CFG; 
	typedef std::map<UINT32, UINT32>::iterator  BORNNPC_DELAYTIME_CFG_ITR;

	typedef std::map<UINT32, UINT64>			ALTAR_LASTBORNNPCTIME; 
	typedef std::map<UINT32, UINT64>::iterator LASTBORNNPCTIME_ITR; 

	typedef std::map<UINT32,UINT64>			ALTAR_IFADD_ANIMIAL;
	typedef std::map<UINT32,UINT64>::iterator IFADD_ANIMIAL_ITR;*/

	 
	 
	
	std::map<INT32/*altar id */, CurtAltarBornSolder>		m_AltarBornSolder;
 
public: 
	CSSAltarSev();
	~CSSAltarSev();  

	//void Release();

	//void InitAltarConfig(UINT32,UINT32,UINT32,UINT32);  
	std::map<INT32/*altar id */, CurtAltarBornSolder>	&	GetCurtAltarInfo()	{return	m_AltarBornSolder;}

	void													AddAltarSolderIdx( INT32 altarId ,INT32 monsterId ,INT32 camId,INT64 mapId );//INT64 btime,const SAltarSolderCfg& cfg );

	//更新祭坛当前出兵数量
	void													UpdateAltarBornSolder(INT32 altarId , INT64 btime );

	//祭坛死亡将删除祭坛信息
	void													RemoveAltarInfoWhenDead(INT32 altarId);

	//祭坛兵死亡
	void													MinusAltarSolderNumWhenDead(INT32 altarId );

	/*INT32 DoAltarBornSolder(TIME_MILSEC tUTCMilsec, std::vector<INT32> &vAltar); 

	void ChangeAltarParam(UINT32 altarIdex,bool bF);

	INT32 ChangeAltarBSolderParam(UINT32 ,bool); 

	bool IfMaxBSNum(UINT32  );

	bool IfBornNPC(UINT32 altarIdex);

	ALTAR_BORNNPCTIMECONFIG&	 GetAltarBornNPCCfg()										{return m_cAltarBornNpcTimeConfig;}
	void						SetAltarBornNPCCfg( ALTAR_BORNNPCTIMECONFIG & cfg)			{m_cAltarBornNpcTimeConfig.insert(cfg.begin(),cfg.end());}

	ALTAR_BORNNPC_DELAYTIME_CFG &GetAltarDelayBornNPCCfg()											{return m_cAltarFirstBornNpcDelayTimeCfg; }
	void						SetAltarDelayBornNPCCfg( ALTAR_BORNNPC_DELAYTIME_CFG & cfg)	        {    m_cAltarFirstBornNpcDelayTimeCfg.insert( cfg.begin(),cfg.end()) ; }*/
private:  

	/*INT32 GetAltarBornTimeSpaceConfig(UINT32);
	INT32 GetFirstBornDelayTimeCfg(UINT32); 

	bool DoAltarBornSolder(UINT32 altarIdex,TIME_MILSEC tUTCMilsec); 

	ALTAR_BORNNPCTIMECONFIG m_cAltarBornNpcTimeConfig;
	BORNNPCTIMECONFIG_ITR   m_cAlatrBornNpcTimeConfigItr;

	ALTAR_BORNNPC_DELAYTIME_CFG  m_cAltarFirstBornNpcDelayTimeCfg;
	BORNNPC_DELAYTIME_CFG_ITR	 m_cAltarFirstBornNpcDelayTimeCfgItr;

	ALTAR_LASTBORNNPCTIME m_cAltarLastBornNpcTime; 
	LASTBORNNPCTIME_ITR   m_cAltarLastBornNpcTimeItr;

	ALTAR_IFADD_ANIMIAL   m_cAltarIfAddAnimial;
	IFADD_ANIMIAL_ITR     m_cAltarIfAddAnimialItr;*/
};

}

#endif