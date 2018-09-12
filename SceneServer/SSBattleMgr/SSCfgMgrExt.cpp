#include "stdafx.h"
#include "SSCfgMgr.h"

#include "tinyxml.h" 
#include "boost/spirit.hpp"

#include "boost/algorithm/string.hpp"


namespace SceneServer{

	INT32	CSSCfgMgr::LoadSSHeroBornCfg()
	{
		stringstream mystream;
		std::string cfg = m_strExeDir + "CSBattleMgr/MapConfig/SS_HeroBirthConfig.xml";
		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("SoldierWaypoints").FirstChild("info").ToElement();
			while(father) 
			{ 
				SHeroBornPos  poscfg; 

				char dump;

				INT32 mapId = atoi(father->Attribute("MapID"));

				TiXmlElement *cmp  = father->FirstChildElement("Camp")->ToElement(); 
				INT32 campId =  atoi(cmp->GetText());

				TiXmlElement *pos = father->FirstChildElement("HeroBirthCoordinates")->ToElement();

				mystream = stringstream(pos->GetText() );
				mystream>>poscfg.bPos.m_fX>>dump>>poscfg.bPos.m_fY>>dump>>poscfg.bPos.m_fZ;
				mystream.clear();
				mystream.str("");


				TiXmlElement *dir = father->FirstChildElement("Orientation")->ToElement(); 
				mystream = stringstream(dir->GetText());
				mystream>>poscfg.bPosDir.m_fX>>dump>>poscfg.bPosDir.m_fY>>dump>>poscfg.bPosDir.m_fZ;
				mystream.clear();
				mystream.str("");

				TiXmlElement *repos = father->FirstChildElement("RelivePoint")->ToElement();

				mystream = stringstream(repos->GetText() );
				mystream>>poscfg.bRelivePos.m_fX>>dump>>poscfg.bRelivePos.m_fY>>dump>>poscfg.bRelivePos.m_fZ;
				mystream.clear();
				mystream.str(""); 
				

				auto itr = mapHeroBornPosCfg.find(mapId);
				if (itr == mapHeroBornPosCfg.end())
				{  
					std::map<INT32,SHeroBornPos > tMap;
					tMap[campId] = poscfg;
					mapHeroBornPosCfg[mapId] = tMap;
				}
				else
				{
					std::map<INT32,SHeroBornPos > &tMap = itr->second;
					auto itrChild = tMap.find(campId);
					if (itrChild == tMap.end())
					{
						tMap[campId] = poscfg;
					}
				}

				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}

	CVector3D	CSSCfgMgr::getHeroBornPosByCamp(UINT64 mapId, INT32 campId){
		CVector3D retPos;
		auto itr = mapHeroBornPosCfg.find(mapId);
		if (itr != mapHeroBornPosCfg.end())
		{
			std::map<INT32,SHeroBornPos > &tMap = itr->second;
			auto itrChild = tMap.find(campId);

			if (itrChild != tMap.end() && itrChild->first == campId)
			{
				retPos = itrChild->second.bPos; 
			} 
		}
		return retPos;
	}
	CVector3D	CSSCfgMgr::getHeroBornDirByCamp(UINT64 mapId, INT32 campId)
	{
		CVector3D retDir;
		auto itr = mapHeroBornPosCfg.find(mapId);
		if (itr != mapHeroBornPosCfg.end())
		{
			std::map<INT32,SHeroBornPos > &tMap = itr->second;
			auto itrChild = tMap.find(campId);

			if (itrChild != tMap.end() && itrChild->first == campId)
			{
				retDir = itrChild->second.bPosDir;
			} 
		}
		return retDir;
	}


	INT32	CSSCfgMgr::LoadSSMapPathPointCfg()
	{
		m_MapPathPointCfg.clear();

		std::string cfg = m_strExeDir + "CSBattleMgr/MapConfig/SS_SoldierWaypoints.xml";  

		std::vector<std::string>  tVec; 

		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("SoldierWaypoints").FirstChild("info").ToElement();
			while(father) 
			{ 
				tVec.clear();
				SSolderGoWayPointCfg cfg;   


				INT32 mapId = atoi(father->Attribute("MapID"));

				TiXmlElement *way  = father->FirstChildElement("WayID")->ToElement(); 
				INT32 pathId =  atoi(way->GetText());

				TiXmlElement *camp  = father->FirstChildElement("Camp")->ToElement(); 
				cfg.camp =  atoi(camp->GetText()); 

				TiXmlElement *point = father->FirstChildElement("Coordinate")->ToElement();

				std::string pointStr( point->GetText() );

				boost::algorithm::split( tVec, pointStr, boost::algorithm::is_any_of(";") ); 

				for (int i=0; i < tVec.size(); i++)
				{
					CVector3D v3d;
					char dump;
					stringstream mystream(tVec[i]);
					mystream>>v3d.m_fX>>dump>>v3d.m_fY>>dump>>v3d.m_fZ; 
					cfg.vecPoint.push_back(v3d); 
				} 

				auto itr = m_MapPathPointCfg.find(mapId);
				if (itr == m_MapPathPointCfg.end())
				{
					std::map<INT32,SSolderGoWayPointCfg >  tmap; 
					tmap[pathId] = cfg;
					m_MapPathPointCfg[mapId] = tmap;

				}
				else
				{
					std::map<INT32,SSolderGoWayPointCfg > &tMap = itr->second;
					auto itrChild = tMap.find(pathId);
					if(itrChild == tMap.end())
					{
						tMap[pathId] = cfg;
					}
					else
					{
						ELOG(LOG_ERROR,"shit, i find the same pathid:%d, mapId:%d",pathId,mapId);
					}
				} 

				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal; 
	}
	void		CSSCfgMgr::GetSSSolderSearPathPointCfg(INT32 mapId, std::map<INT32/*pathid*/,SSolderGoWayPointCfg >  & pathCfg)
	{ 
		pathCfg.clear();

		auto itr = m_MapPathPointCfg.find(mapId);
		if (itr != m_MapPathPointCfg.end())
		{
			pathCfg = itr->second; 
		}
		else
		{
			ELOG(LOG_ERROR,"shit, i can not find solder search path point, mapId:%d", mapId);
		}
	}


	INT32		CSSCfgMgr::LoadSSMapSolderInfoCfg()
	{
		m_MapSolderBaseInfoCfg.clear();

		std::string cfg = m_strExeDir + "CSBattleMgr/MapConfig/SS_MapBaseConfig.xml";   


		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("MapBaseConfig").FirstChild("info").ToElement();
			while(father) 
			{   
				SMapSolderInfoCfg solderInfo;

				INT32 mapId = atoi(father->Attribute("MapID"));

				TiXmlElement *way  = father->FirstChildElement("WayID")->ToElement(); 
				INT32 pathId =  atoi(way->GetText());

				TiXmlElement *baseBuildIdx = father->FirstChildElement("BaseID")->ToElement();
				solderInfo.baseBuildIdxCfg = atoi(baseBuildIdx->GetText());  

				TiXmlElement *totalBorn = father->FirstChildElement("BatmanTotal")->ToElement();
				solderInfo.totalSolderCfg = atoi(totalBorn->GetText());  

				TiXmlElement *everybornTimeSpace = father->FirstChildElement("BatchBatmanInterval")->ToElement();
				solderInfo.solderTimeSpaceCfg = atoi(everybornTimeSpace->GetText());  

				TiXmlElement *bornTimeSpace = father->FirstChildElement("SingleBatmanInterval")->ToElement();
				solderInfo.everyTimeSpace = atoi(bornTimeSpace->GetText());  

				TiXmlElement *chariotCfg = father->FirstChildElement("SiegeVehicles")->ToElement();
				solderInfo.mChariotSolderCfg.charSolderBornTimes  = atoi(chariotCfg->GetText());  

				TiXmlElement *solderCfg = father->FirstChildElement("BatmanID")->ToElement(); 
				std::string solderStr( solderCfg->GetText() );

				std::vector<std::string> tVec;
				boost::algorithm::split( tVec, solderStr, boost::algorithm::is_any_of(",") ); 

				for (int i=0; i < tVec.size(); i++)
				{
					INT32 solderIdx = atoi(tVec[i].c_str());
					solderInfo.solderIdxCfg.push_back(solderIdx);
				}
				tVec.clear();

				TiXmlElement *chariotIdxCfg = father->FirstChildElement("SiegeVehiclesID")->ToElement();
				solderInfo.mChariotSolderCfg.chariotSolderIdx  = atoi(chariotIdxCfg->GetText()); 

				TiXmlElement *superCfg = father->FirstChildElement("SuperBatmanID")->ToElement();
				std::string superStr( superCfg->GetText() );
				boost::algorithm::split( tVec, superStr, boost::algorithm::is_any_of(",") ); 
				for (int i=0; i < tVec.size(); i++)
				{
					INT32 solderIdx = atoi(tVec[i].c_str());
					solderInfo.mSuperSolderCfg.superSolderIdx.push_back(solderIdx); 
				}
				tVec.clear();

				TiXmlElement *bornPosCfg = father->FirstChildElement("BatmanOrientation")->ToElement();
				std::string bornStr( bornPosCfg->GetText() );
				boost::algorithm::split( tVec, bornStr, boost::algorithm::is_any_of(",") );   
				solderInfo.bornDirCfg.m_fX = atof(tVec[0].c_str());
				solderInfo.bornDirCfg.m_fY = atof(tVec[1].c_str());
				solderInfo.bornDirCfg.m_fZ = atof(tVec[2].c_str());

				TiXmlElement *altarCfg = father->FirstChildElement("SuperBatmanAltar")->ToElement();
				std::string altarStr( altarCfg->GetText() );
				boost::algorithm::split( tVec, altarStr, boost::algorithm::is_any_of(",") ); 
				for (int i=0; i< tVec.size(); i++)
				{
					solderInfo.mSuperSolderCfg.altarIdx.push_back(atoi(tVec[i].c_str()));
				}
				tVec.clear();

				TiXmlElement *superSolderWay = father->FirstChildElement("SuperBatmanWay")->ToElement();
				solderInfo.mSuperSolderCfg.pathId = atoi(superSolderWay->GetText());

				TiXmlElement *dirCfg = father->FirstChildElement("SoliderBornPoint")->ToElement();
				std::string dirStr( dirCfg->GetText() );
				boost::algorithm::split( tVec, dirStr, boost::algorithm::is_any_of(",") ); 
				solderInfo.bornPosCfg.m_fX = atof(tVec[0].c_str());
				solderInfo.bornPosCfg.m_fY = atof(tVec[1].c_str());
				solderInfo.bornPosCfg.m_fZ = atof(tVec[2].c_str());

				//std::map<INT32/*mapid*/,std::map<INT32/*pathid*/,SMapSolderInfoCfg > >				m_MapSolderBaseInfoCfg;
				auto itr = m_MapSolderBaseInfoCfg.find(mapId);
				if (itr != m_MapSolderBaseInfoCfg.end())
				{
					std::map<INT32/*pathid*/,SMapSolderInfoCfg > & tPathCfg = itr->second;
					auto itrChild = tPathCfg.find(pathId);
					if (itrChild == tPathCfg.end())
					{
						tPathCfg[pathId] = solderInfo;
					}
					else
					{
						ELOG(LOG_ERROR,"the same mapid:%d, path:%d",mapId,pathId);
					}
				}
				else
				{
					std::map<INT32/*pathid*/,SMapSolderInfoCfg >   tPathCfg;
					tPathCfg[pathId] = solderInfo;
					m_MapSolderBaseInfoCfg[mapId] = tPathCfg;
				}

				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}

	void		CSSCfgMgr::GetSSSolderBornInfoCfg(INT32 mapId, std::map<INT32,SMapSolderInfoCfg>& pathCfg)
	{
		auto itr = m_MapSolderBaseInfoCfg.find(mapId);
		if(itr != m_MapSolderBaseInfoCfg.end())
		{
			pathCfg = itr->second;
		}
		else
		{
			ELOG(LOG_ERROR,"can not find solder born cfg, mapId:%d",mapId);
		}
	}
	//返回出超级兵对应的路径(祭坛被打爆，该出超级兵的路径)
	INT32		CSSCfgMgr::GetSSAltarBelongPathCfg(INT32 mapId,INT32 altarId,std::vector<INT32>& altarCfg)
	{ 
#if  1
		auto itr = m_MapSolderBaseInfoCfg.find(mapId);
		if(itr != m_MapSolderBaseInfoCfg.end())
		{
			std::map<INT32,SMapSolderInfoCfg> & tmap = itr->second;
			auto itrChilde = tmap.begin();
			for (; itrChilde != tmap.end(); itrChilde++)
			{
				SMapSolderInfoCfg & tSolderCfg = itrChilde->second;

				altarCfg = tSolderCfg.mSuperSolderCfg.altarIdx;

				for(int i=0; i <  altarCfg.size(); i++) 
				{
					if (altarCfg[i] == altarId)
					{
						return tSolderCfg.mSuperSolderCfg.pathId;
					}
				}
			}
		}
		ELOG(LOG_ERROR,"shit, i can not find altar belong path, mapid:%d, alatrid:%d",mapId, altarId);
#endif
		return 0;
	}

	INT32		CSSCfgMgr::LoadSSMapAltarSolderInfoCfg()
	{
		m_MapAltarSolderInfoCfg.clear();

		std::string cfg = m_strExeDir + "CSBattleMgr/MapConfig/SS_AltarConfig.xml";    

		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("AltarConfig").FirstChild("info").ToElement();
			while(father) 
			{   
				SAltarSolderCfg solderInfo;

				INT32 mapId = atoi(father->Attribute("MapID"));

				TiXmlElement *id  = father->FirstChildElement("AltarID")->ToElement(); 
				INT32  altarIdx =  atoi(id->GetText());

				TiXmlElement *way  = father->FirstChildElement("WayID")->ToElement(); 
				solderInfo.pathId =  atoi(way->GetText());

				TiXmlElement *timeSpace  = father->FirstChildElement("BatmanInterval")->ToElement(); 
				solderInfo.bornSolderSpaceTime  =  atoi(timeSpace->GetText());

				TiXmlElement *num  = father->FirstChildElement("BatmanNumber")->ToElement(); 
				solderInfo.bornNum  =  atoi(num->GetText());

				TiXmlElement *maxNum  = father->FirstChildElement("batmanMaximum")->ToElement(); 
				solderInfo.maxSolderNum  =  atoi(maxNum->GetText());

				TiXmlElement *posStr  = father->FirstChildElement("BatmanBirthCoordinates")->ToElement(); 
				std::string  bornPos(posStr->GetText());  

				std::vector<std::string> tVec;
				boost::algorithm::split( tVec, bornPos, boost::algorithm::is_any_of(",") ); 
				solderInfo.bornPos.m_fX = atof(tVec[0].c_str());
				solderInfo.bornPos.m_fY = atof(tVec[1].c_str());
				solderInfo.bornPos.m_fZ = atof(tVec[2].c_str()); 
				tVec.clear();

				TiXmlElement *dirStr = father->FirstChildElement("BatmanOrientation")->ToElement();
				std::string borDir(dirStr->GetText());  
				boost::algorithm::split( tVec, borDir, boost::algorithm::is_any_of(",") ); 
				solderInfo.bornDir.m_fX = atof(tVec[0].c_str());
				solderInfo.bornDir.m_fY = atof(tVec[1].c_str());
				solderInfo.bornDir.m_fZ = atof(tVec[2].c_str());  

				// std::map<INT32/*mapid*/,std::map<INT32/*altarIdx*/,SAltarSolderCfg > >				m_MapAltarSolderInfoCfg;
				auto itr = m_MapAltarSolderInfoCfg.find(mapId);
				if (itr != m_MapAltarSolderInfoCfg.end())
				{
					std::map<INT32/*altarIdx*/,SAltarSolderCfg > &tMap = itr->second;
					auto itrChilde = tMap.find(altarIdx);
					if (itrChilde == tMap.end())
					{
						tMap[altarIdx] = solderInfo;
					}
				}
				else
				{
					std::map<INT32/*altarIdx*/,SAltarSolderCfg >  tMap;
					tMap[altarIdx] = solderInfo;
					m_MapAltarSolderInfoCfg[mapId] = tMap;
				}
				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}

	INT32		CSSCfgMgr::LoadSSMapFunDelayCfg()
	{
		m_GameDelayTimeCfg.clear();

		std::string cfg = m_strExeDir + "CSBattleMgr/MapConfig/SS_DelayConfig.xml";    

		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("DelayConfig").FirstChild("info").ToElement();
			while(father) 
			{   
				SGameDelayTimeCfg delayCfg;

				INT32 mapId = atoi(father->Attribute("MapID"));

				TiXmlElement *solderDelay  = father->FirstChildElement("BatmanDelay")->ToElement(); 
				delayCfg.mSoldeDelay =  atoi(solderDelay->GetText());

				TiXmlElement *vmDelay  = father->FirstChildElement("MonsterDelay")->ToElement(); 
				delayCfg.mPrimaryWMMonsterDelay =  atoi(vmDelay->GetText());

				TiXmlElement *advmDelay  = father->FirstChildElement("AdvancedmonsterDelay")->ToElement(); 
				delayCfg.mHighWMMonsterDelay =  atoi(advmDelay->GetText());

				TiXmlElement *altarDelay  = father->FirstChildElement("AltarDelay")->ToElement(); 
				delayCfg.mAltarSolderDelay =  atoi(altarDelay->GetText());
				//std::map<INT32/*mapid*/,SGameDelayTimeCfg>											m_GameDelayTimeCfg;	
				auto itr = m_GameDelayTimeCfg.find(mapId);
				if (itr == m_GameDelayTimeCfg.end())
				{
					m_GameDelayTimeCfg[mapId] = delayCfg;
				} 
				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}
	INT32		CSSCfgMgr::LoadSSMapWildMonsterCfg()
	{
		m_GameWildMonsterCfg.clear();
		std::string cfg = m_strExeDir + "CSBattleMgr/MapConfig/SS_CreepsConfig.xml";    

		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("CreepsConfig").FirstChild("info").ToElement();
			while(father) 
			{   
				SGameWildMonsterCfg wcfg;

				INT32 mapId = atoi(father->Attribute("MapID"));

				TiXmlElement *pointId  = father->FirstChildElement("ID")->ToElement(); 
				INT32  pid =  atoi(pointId->GetText());

				TiXmlElement *mlv  = father->FirstChildElement("MonsterLv")->ToElement(); 
				wcfg.lv  =  atoi(mlv->GetText());

				TiXmlElement *wnum  = father->FirstChildElement("MonsterNumber")->ToElement(); 
				wcfg.num =  atoi(wnum->GetText());

				TiXmlElement *idstr  = father->FirstChildElement("MonsterID")->ToElement(); 
				std::string   idStr(idstr->GetText());
				std::vector<std::string> tvec;
				boost::algorithm::split( tvec, idStr, boost::algorithm::is_any_of(",") ); 
				for (INT32  itr = 0; itr < tvec.size();  itr++)
				{
					INT32  id = atoi(tvec[itr].c_str());
					wcfg.vecIdx.push_back(id);
				}
				tvec.clear();

				TiXmlElement *bornstr  = father->FirstChildElement("BornCoordinates")->ToElement(); 
				std::string bornPosStr(bornstr->GetText());
				boost::algorithm::split( tvec, bornPosStr, boost::algorithm::is_any_of(",") ); 
				wcfg.bornPos.m_fX = atof(tvec[0].c_str());
				wcfg.bornPos.m_fY = atof(tvec[1].c_str());
				wcfg.bornPos.m_fZ = atof(tvec[2].c_str());

				TiXmlElement *refresh  = father->FirstChildElement("MonsterRefreshInterval")->ToElement(); 
				wcfg.refreshTime = atoi(refresh->GetText());

				TiXmlElement *myrand  = father->FirstChildElement("Random")->ToElement(); 
				wcfg.bIfRandom = atoi(myrand->GetText()) > 0 ? true : false;

				tvec.clear(); 
				TiXmlElement *dirstr  = father->FirstChildElement("Orientation")->ToElement(); 
				std::string bornDirStr(dirstr->GetText());
				boost::algorithm::split( tvec, bornDirStr, boost::algorithm::is_any_of(",") ); 
				wcfg.bornDir.m_fX = atof(tvec[0].c_str());
				wcfg.bornDir.m_fY = atof(tvec[1].c_str());
				wcfg.bornDir.m_fZ = atof(tvec[2].c_str()); 

				//std::map<INT32/*mapId*/,std::map<INT32/*wm point*/,SGameWildMonsterCfg> >			m_GameWildMonsterCfg;
				auto itr = m_GameWildMonsterCfg.find(mapId);
				if (itr != m_GameWildMonsterCfg.end())
				{
					std::map<INT32/*wm point*/,SGameWildMonsterCfg> & tMap = itr->second;
					auto itrChild = tMap.find(pid);
					if (itrChild == tMap.end())
					{
						tMap[pid] = wcfg;
					} 
				} else
				{
					std::map<INT32/*wm point*/,SGameWildMonsterCfg> tmap;
					tmap[pid] = wcfg;

					m_GameWildMonsterCfg[mapId] = tmap;
				}
				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}

	INT64	CSSCfgMgr::GetSSMapFuncDelayTimeCfg(EGameFunDelay  etype, INT32 mapId )
	{  
		auto  itr = m_GameDelayTimeCfg.find(mapId);
		if (itr != m_GameDelayTimeCfg.end())
		{
			if(etype == eDelay_Solder )
			{
				return itr->second.mSoldeDelay;
			}
			else if(etype == eDelay_PrimaryMonster )
			{
				return itr->second.mPrimaryWMMonsterDelay;
			}
			else if( etype == eDelay_HightMonster)
			{
				return itr->second.mHighWMMonsterDelay;
			}
			else if(etype == eDelay_AltarSolder )
			{
				return itr->second.mAltarSolderDelay;
			} 
		} 
		return 0;
	}

	void		CSSCfgMgr::GetSSAltarSolderPosInfo(INT32 mapId, INT32 altarid, SAltarSolderCfg & ascfg)
	{  
		auto itr = m_MapAltarSolderInfoCfg.find(mapId);
		if (itr != m_MapAltarSolderInfoCfg.end())
		{
			std::map<INT32/*altarIdx*/,SAltarSolderCfg > & tMap = itr->second;
			auto itrChilde = tMap.find(altarid);
			if (itrChilde != tMap.end())
			{
				ascfg = itrChilde->second;
			}
		}
	}

	INT32		CSSCfgMgr::GetSSPathBelongCamp(INT32 mapId, INT32 pathId)
	{
		//std::map<INT32/*mapid*/,std::map<INT32/*pathid*/,SSolderGoWayPointCfg > >			m_MapPathPointCfg;	
		auto itr = m_MapPathPointCfg.find(mapId);
		if (itr != m_MapPathPointCfg.end())
		{
			std::map<INT32/*pathid*/,SSolderGoWayPointCfg > & tmap = itr->second;
			auto itrChilde = tmap.find(pathId);
			if (itrChilde != tmap.end())
			{
				return itrChilde->second.camp;
			}
		}
		return 0;
	}

	void	CSSCfgMgr::GetWildMonsterCfg(INT32 mapid,std::map<INT32/*wm point*/,SGameWildMonsterCfg>& tmap )
	{
		tmap.clear();

		auto itr =	m_GameWildMonsterCfg.find(mapid);
		if (itr != m_GameWildMonsterCfg.end())
		{
			tmap = itr->second;
		}
	}

	bool	CSSCfgMgr::CheckObjIdIsBaseBuild(INT32 mapid, INT64 objidx, INT32 &pathId)
	{ 
		auto itr = m_MapSolderBaseInfoCfg.find(mapid);
		if (itr != m_MapSolderBaseInfoCfg.end())
		{
			std::map<INT32/*pathid*/,SMapSolderInfoCfg > &tmap = itr->second;
			auto itrChild = tmap.begin();
			for(; itrChild != tmap.end(); itrChild++)
			{
				if (itrChild->second.baseBuildIdxCfg == objidx)
				{
					pathId = itrChild->first;
					return true;
				}
			}
		}
		return false;
	}

	////////////////////////////////////新手引导//////////////////////////////// 
#if 0
	INT32		CSSCfgMgr::GetGuideDelayCfg(EGameFunDelay  etype, INT32 mapId )
	{
		auto itr = m_GuideDelayCfg.find(mapId);
		if (itr != m_GuideDelayCfg.end() )
		{
			if(etype == eDelay_Solder )
			{
				return itr->second.mSoldeDelay;
			}
			else if(etype == eDelay_PrimaryMonster )
			{
				return itr->second.mPrimaryWMMonsterDelay;
			}
			else if( etype == eDelay_HightMonster)
			{
				return itr->second.mHighWMMonsterDelay;
			}
			else if(etype == eDelay_AltarSolder )
			{
				return itr->second.mAltarSolderDelay;
			}  
		} 
		return 0;
	}

	INT32		CSSCfgMgr::LoadSSGuideMonsterCfg()
	{ 
		return eNormal;

		m_GuideWildMonsterCfg.clear();
		const char* cfg = "CSBattleMgr/SSGuide/Creepspoint.xml";    

		TiXmlDocument doc(cfg); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("Creepspoint").FirstChild("info").ToElement();
			while(father) 
			{   
				SGameWildMonsterCfg cfg;

				INT32 mapId = atoi(father->Attribute("MapID"));

				TiXmlElement *point   = father->FirstChildElement("ID")->ToElement(); 
				INT32  pointId = atoi(point->GetText());

				TiXmlElement *clv   = father->FirstChildElement("MonsterLv")->ToElement(); 
				cfg.lv = atoi(clv->GetText());

				TiXmlElement *num   = father->FirstChildElement("MonsterNumber")->ToElement(); 
				cfg.num = atoi(num->GetText());

				TiXmlElement *idlist   = father->FirstChildElement("MonsterID")->ToElement(); 
				std::string idStr(idlist->GetText());
				std::vector<std::string> tvec;
				boost::algorithm::split( tvec, idStr, boost::algorithm::is_any_of(",") ); 
				for (int i=0; i< tvec.size(); i++)
				{
					cfg.vecIdx.push_back(atoi(tvec[i].c_str()));
				}
				tvec.clear();

				TiXmlElement *pos   = father->FirstChildElement("BornCoordinates")->ToElement(); 
				std::string posStr(pos->GetText());
				boost::algorithm::split( tvec, posStr, boost::algorithm::is_any_of(",") ); 
				cfg.bornPos.m_fX = atof(tvec[0].c_str());
				cfg.bornPos.m_fY = atof(tvec[1].c_str());
				cfg.bornPos.m_fZ = atof(tvec[2].c_str());
				tvec.clear();

				TiXmlElement *reftime   = father->FirstChildElement("MonsterRefreshInterval")->ToElement(); 
				cfg.refreshTime = atoi(reftime->GetText());

				TiXmlElement *brand   = father->FirstChildElement("Random")->ToElement(); 
				cfg.bIfRandom = atoi(brand->GetText()) > 0 ? true : false;

				tvec.clear();
				TiXmlElement *bdir   = father->FirstChildElement("Orientation")->ToElement(); 
				std::string strDir(bdir->GetText());
				boost::algorithm::split( tvec, strDir, boost::algorithm::is_any_of(",") );  
				cfg.bornDir.m_fX = atof(tvec[0].c_str());
				cfg.bornDir.m_fY = atof(tvec[1].c_str());
				cfg.bornDir.m_fZ = atof(tvec[2].c_str()); 

				auto itr = m_GuideWildMonsterCfg.find(mapId);
				if (itr != m_GuideWildMonsterCfg.end())
				{
					std::map<INT32/*wm point*/,SGameWildMonsterCfg> & tmap = itr->second;
					auto itrChild = tmap.find(pointId);
					if (itrChild == tmap.end())
					{
						tmap[pointId] = cfg;
					}
				}
				else
				{
					std::map<INT32/*wm point*/,SGameWildMonsterCfg>   tmap;
					tmap[pointId] = cfg;
					m_GuideWildMonsterCfg[mapId] = tmap;
				}

				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}

	INT32		CSSCfgMgr::LoadSSGuideDelayCfg()
	{
		m_GuideDelayCfg.clear();
		const char* cfg = "CSBattleMgr/SSGuide/Delay.xml";    

		TiXmlDocument doc(cfg); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("Delay").FirstChild("info").ToElement();
			while(father) 
			{   
				SGameDelayTimeCfg cfg;

				INT32 mapId = atoi(father->Attribute("MapID"));

				TiXmlElement *soldelay   = father->FirstChildElement("BatmanDelay")->ToElement(); 
				cfg.mSoldeDelay  = atoi(soldelay->GetText());

				TiXmlElement *mdelay   = father->FirstChildElement("MonsterDelay")->ToElement(); 
				cfg.mPrimaryWMMonsterDelay = atoi(mdelay->GetText());

				TiXmlElement *addelay   = father->FirstChildElement("AdvancedmonsterDelay")->ToElement(); 
				cfg.mHighWMMonsterDelay = atoi(addelay->GetText());

				TiXmlElement *altardelay   = father->FirstChildElement("AltarDelay")->ToElement(); 
				cfg.mAltarSolderDelay = atoi(altardelay->GetText());  

				m_GuideDelayCfg[mapId] = cfg; 

				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}
#endif

	INT32		CSSCfgMgr::LoadSSGuideHeroCfg()
	{
		m_GuideHeroCfg.clear();
		std::string cfg = m_strExeDir + "CSBattleMgr/SSGuideCfg/SS_bornhero.xml";    

		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("bornhero").FirstChild("info").ToElement();
			while(father) 
			{   
				SGuideHeroCfg cfg;

				INT32 mapId = atoi(father->Attribute("mapid"));

				TiXmlElement *task   = father->FirstChildElement("taskid")->ToElement(); 
				INT32  taskId  = atoi(task->GetText());

				std::vector<std::string > tvec; 

				TiXmlElement *borntype   = father->FirstChildElement("pointform")->ToElement(); 
				cfg.bornTypeCfg = (EGuideHeroBornType)atoi(borntype->GetText());


				TiXmlElement *idx   = father->FirstChildElement("ID")->ToElement(); 
				cfg.objIdxCfg = atoi(idx->GetText());

				tvec.clear(); 

				TiXmlElement *camp   = father->FirstChildElement("camp")->ToElement(); 
				cfg.campIdCfg = atoi(camp->GetText());


				TiXmlElement *rage   = father->FirstChildElement("Increasinganger")->ToElement(); 
				cfg.ifRage = atoi(rage->GetText()) > 0 ? true : false; 
				tvec.clear(); 

				TiXmlElement *time   = father->FirstChildElement("time")->ToElement(); 
				cfg.reliveTimeCfg = atoi(time->GetText());

				auto itr = m_GuideHeroCfg.find(mapId);
				if (itr != m_GuideHeroCfg.end() )
				{
					std::map<INT32/*taskid*/,SGuideHeroCfg> &  tmap = itr->second;
					tmap[taskId] = cfg;
				}
				else
				{
					std::map<INT32/*taskid*/,SGuideHeroCfg> tmap;
					tmap[taskId] = cfg; 
					m_GuideHeroCfg[mapId] = tmap;
				} 

				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}

	INT32	CSSCfgMgr::LoadSSGuideSolderCfg()
	{ 
		m_GuideSolderCfg.clear();
		std::string cfg = m_strExeDir + "CSBattleMgr/SSGuideCfg/SS_bornnpc.xml";    

		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("bornnpc").FirstChild("info").ToElement();
			while(father) 
			{   
				SGuideSolderCfg cfg;

				INT32 mapId = atoi(father->Attribute("mapid"));

				TiXmlElement *task   = father->FirstChildElement("taskid")->ToElement(); 
				INT32  taskId  = atoi(task->GetText());

				TiXmlElement *pos   = father->FirstChildElement("post")->ToElement(); 
				std::vector<std::string>  tvec;
				string strPos(pos->GetText());

				boost::algorithm::split( tvec, strPos, boost::algorithm::is_any_of(",") );  
				cfg.bornPosCfg.m_fX = atof(tvec[0].c_str());
				cfg.bornPosCfg.m_fY = atof(tvec[1].c_str());
				cfg.bornPosCfg.m_fZ = atof(tvec[2].c_str());
				tvec.clear();

				TiXmlElement *patrol   = father->FirstChildElement("way")->ToElement();  
				string strPatrol(patrol->GetText());

				boost::algorithm::split( tvec, strPatrol, boost::algorithm::is_any_of(";") );  
				for (int i=0; i< tvec.size(); i++)
				{
					if (tvec[i].length() < 1)
					{
						continue;
					}
					std::vector<std::string>  myvec;
					boost::algorithm::split( myvec, tvec[i], boost::algorithm::is_any_of(",") );  
					CVector3D  tv;  
					tv.m_fX  = atof(myvec[0].c_str());
					tv.m_fY  = atof(myvec[1].c_str());
					tv.m_fZ  = atof(myvec[2].c_str());

					cfg.mGoPathPointCfg.push_back(tv);
				} 
				tvec.clear();

				TiXmlElement *reft   = father->FirstChildElement("timespace")->ToElement(); 
				cfg.bornSpaceTimeCfg = atoi(reft->GetText());

				TiXmlElement *bornn   = father->FirstChildElement("bornnumberspace")->ToElement(); 
				cfg.bornAheadTimeCfg  = atoi(bornn->GetText());

				tvec.clear();
				TiXmlElement *idex   = father->FirstChildElement("ID")->ToElement(); 
				string strIdx(idex->GetText()); 
				boost::algorithm::split( tvec, strIdx, boost::algorithm::is_any_of(",") );  
				for (int i=0; i< tvec.size(); i++)
				{
					cfg.vecObjIdCfg.push_back( atoi(tvec[i].c_str() ) );
				} 

				tvec.clear();

				TiXmlElement *dir   = father->FirstChildElement("dir")->ToElement(); 
				std::string strDir(dir->GetText());
				boost::algorithm::split( tvec, strDir, boost::algorithm::is_any_of(",") );  
				cfg.bornDirCfg.m_fX = atof(tvec[0].c_str());
				cfg.bornDirCfg.m_fY = atof(tvec[1].c_str());
				cfg.bornDirCfg.m_fZ = atof(tvec[2].c_str());

				if(NULL != father->FirstChildElement("closeborn"))
				{
					TiXmlElement *closeId   = father->FirstChildElement("closeborn")->ToElement(); 
					cfg.closeTaskIdCfg = atoi(closeId->GetText());
				} 

				TiXmlElement *camp   = father->FirstChildElement("camp")->ToElement(); 
				cfg.campidCfg = atoi(camp->GetText());

				if ( father->FirstChildElement("altarid") != NULL)
				{
					TiXmlElement *altar   = father->FirstChildElement("altarid")->ToElement(); 
					 
					memcpy(cfg.altaridVecCfg,altar->GetText(),sizeof(cfg.altaridVecCfg)); 
				} 

				auto itr = m_GuideSolderCfg.find(mapId);
				if (itr != m_GuideSolderCfg.end() )
				{
					std::map<INT32/*taskid*/,SGuideSolderCfg> &tmap=itr->second;
					tmap[taskId] = cfg;
				}
				else
				{
					std::map<INT32/*taskid*/,SGuideSolderCfg> tmap;
					tmap[taskId ] = cfg;
					m_GuideSolderCfg[mapId] = tmap;
				}

				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal; 
	}

	void		CSSCfgMgr::GetGuideHeroCfg(INT32 mapId, INT32 taskId,SGuideHeroCfg&  cfg)
	{
		auto itr =				m_GuideHeroCfg.find(mapId); 
		if (itr != m_GuideHeroCfg.end())
		{
			std::map<INT32/*taskid*/,SGuideHeroCfg> &tmap = itr->second;
			auto itrChild = tmap.find(taskId);
			if (itrChild != tmap.end())
			{
				cfg = itrChild->second;
			}
		}
	}
	void	CSSCfgMgr::GetGuideSolderCfg(INT32 mapId, INT32 taskId, SGuideSolderCfg & cfg)
	{
		//std::map<INT32/*mapId*/,std::map<INT32/*taskid*/,SGuideSolderCfg> >					m_GuideSolderCfg;  
		auto itr = m_GuideSolderCfg.find(mapId);
		if (itr != m_GuideSolderCfg.end())
		{
			std::map<INT32/*taskid*/,SGuideSolderCfg> &tmap = itr->second;
			auto itrChilde = tmap.find(taskId);
			if (itrChilde != tmap.end())
			{
				cfg = itrChilde->second;
			}
		}

	}

	INT32	CSSCfgMgr::LoadSSGuideUserHeroCfg()
	{
		m_GuideUserHeroCfg.clear();
		std::string cfg = m_strExeDir + "CSBattleMgr/SSGuideCfg/SS_userHero.xml";    

		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("userhero").FirstChild("info").ToElement();
			while(father) 
			{   
				INT32 mapId = atoi(father->Attribute("mapid"));

				std::vector<INT32>  vec;
				TiXmlElement *id   = father->FirstChildElement("id")->ToElement(); 
				std::string  heroStr(id->GetText());

				if (-1 != heroStr.find(","))
				{ 
					std::vector<std::string>  tvec;
					boost::algorithm::split( tvec, heroStr, boost::algorithm::is_any_of(",") );  
					for (int i=0; i< tvec.size(); i++)
					{
						vec.push_back(atoi(tvec[i].c_str()));
					}
				}
				else
				{
					vec.push_back(atoi(heroStr.c_str()));
				}

				auto itr = m_GuideUserHeroCfg.find(mapId);
				if (itr != m_GuideUserHeroCfg.end())
				{
					for (int i=0; i< vec.size(); i++)
					{
						itr->second.push_back(vec[i]);
					}
				}
				else
				{
					m_GuideUserHeroCfg[mapId] = vec;
				}

				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}
	INT32	CSSCfgMgr::GetUserHeroIdByMapId(INT32 mapId)
	{
		auto itr = m_GuideUserHeroCfg.find(mapId);
		if (itr != m_GuideUserHeroCfg.end())
		{
			std::vector<INT32> &tvec = itr->second;
			return tvec[0];
		}
		return 0;
	}
	INT32		CSSCfgMgr::LoadSSGuideHurtObjCfg()
	{
		m_GuideHurtBuildCfg.clear();
		std::string cfg = m_strExeDir + "CSBattleMgr/SSGuideCfg/SS_BuildBeHurt.xml";    

		TiXmlDocument doc(cfg.c_str()); 
		if(doc.LoadFile())
		{
			TiXmlHandle dochandle(&doc);
			TiXmlElement *father =  dochandle.FirstChild("SS_BuildBeHurt").FirstChild("info").ToElement();
			while(father) 
			{   
				SGuideHurtBuildCfg cfg;

				INT32 mapId = atoi(father->Attribute("mapid")); 

				TiXmlElement *pid   = father->FirstChildElement("id")->ToElement(); 
				INT32    taskId = atoi(pid->GetText());

				TiXmlElement *obtype   = father->FirstChildElement("object")->ToElement(); 
				cfg.objTypeCfg = (EGuideHurtObjType)atoi(obtype->GetText());

				TiXmlElement *obId  = father->FirstChildElement("objectid")->ToElement(); 
				cfg.objIdxCfg = atoi(obId->GetText());

				TiXmlElement *atktype  = father->FirstChildElement("typeid")->ToElement(); 
				cfg.atkTypeCfg = (EGuideAtkType) atoi(atktype->GetText());

				auto itr = m_GuideHurtBuildCfg.find(mapId);
				if (itr != m_GuideHurtBuildCfg.end())
				{
					std::map<INT32/*TASKID*/,SGuideHurtBuildCfg>  &tmap = itr->second;
					auto itrChilde = tmap.find(taskId);
					if (itrChilde == tmap.end())
					{
						tmap[taskId] = cfg;
					}
				}
				else
				{
					std::map<INT32/*TASKID*/,SGuideHurtBuildCfg>   tmap;
					tmap[taskId] = cfg;
					m_GuideHurtBuildCfg[mapId] = tmap;
				}

				father = father->NextSiblingElement(); 
			}
		}
		else
		{
			ELOG(LOG_ERROR,"can not find file: %s",cfg);
		}
		return eNormal;
	}

	void		CSSCfgMgr::GetGuideHurtBuildCfg(INT32 mapId,INT32 taskId, SGuideHurtBuildCfg &cfg)
	{
		auto itr =		m_GuideHurtBuildCfg.find(mapId);
		if (itr != m_GuideHurtBuildCfg.end())
		{
			std::map<INT32/*TASKID*/,SGuideHurtBuildCfg> &tamp = itr->second;
			auto itrChild = tamp.find(taskId);
			if (itrChild != tamp.end())
			{
				cfg = itrChild->second;
			}
		}
	}
	void	CSSCfgMgr::GetGuideSolderPathPointCfg(INT32 mapId,INT32 altarId, std::vector<CVector3D> &vec)
	{
		char szAltar[32]={};
		sprintf(szAltar,"%d",altarId);
		auto itr =	m_GuideSolderCfg.find(mapId);  
		if (itr != m_GuideSolderCfg.end())
		{
			std::map<INT32/*taskid*/,SGuideSolderCfg>  & tMap = itr->second;
			auto itrChild = tMap.begin();
			for (; itrChild != tMap.end(); itrChild++)
			{
				std::string altarStr(itrChild->second.altaridVecCfg);
				if (altarStr.length() < 1)
				{
					continue;
				}
				int nPos = altarStr.find(szAltar);
				if(-1 != nPos)
				{
					vec = itrChild->second.mGoPathPointCfg;
					return;
				}  
			}
		 }
	}

bool	CSSCfgMgr::getHeroBornCfg(INT32 mapId, INT32 campId, SHeroBornPos & hb)
{
	auto itr = mapHeroBornPosCfg.find(mapId);
	if (itr != mapHeroBornPosCfg.end())
	{
		std::map<INT32,SHeroBornPos > &tMap = itr->second;
		auto itrChild = tMap.find(campId);

		if (itrChild != tMap.end() && itrChild->first == campId)
		{
			hb = itrChild->second;

			return true;
		} 
	}
	return false;
}
}