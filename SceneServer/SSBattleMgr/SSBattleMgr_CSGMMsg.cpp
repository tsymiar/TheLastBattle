#include "stdafx.h"
 
#include "SSBattle.h"
#include "SSBattleMgr.h"
#include "SSHero.h"
#include <boost/spirit.hpp>
#include "boost/algorithm/string.hpp"
#include "SSUser.h"
#include <vector>
using namespace std;
//战斗场景
//gm配置
/*
	"lv",	--eg:lv 10
	 "cp",	--eg:cp 100
	 "speed",--eg:speed 5
	 "hp",	--eg:hp  1000
	 "mp",	--eg:mp  1000
	 "rage",	--eg:rage 1000
	 "finish",	--eg:finish 1
	 "guidefinish",	--eg:guidefinish 1
	 "cd", ---cd 1, -- 刷新所有CD，包括物品
	 "npc", ---npc 1, --开关小兵1开，0关
	 "monster",----monster 1,--关闭野怪功能 1开，0关
	 "altar",----altar 1, --关闭祭坛出怪能能 1 开，0关
	 "sx", ----sx,--改变属性。 格式  sx 属性序列 数值。比如 sx 5 100 加100物理攻击力
	 常用属性: 5物攻 6魔攻 7物防 8魔防 10攻速 11最大血 12最大蓝 13生命回复 14魔法回复 15复活时间
	 ""，--
*/
using namespace boost;
namespace SceneServer{ 

	INT32	CSSBattleMgr::OnMsgFromGC_GMCmd(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){	

		if (!CSSCfgMgr::GetSSCfgInstance().GetSSFuncSwitch().gmSwitch)
		{
			ELOG(LOG_ERROR,"curt gm fun has closed!",);
			return eNormal;
		}

		CSSHero* TempHero = m_pMsgBattle->GetUserBattleInfo(m_pMsgUser)->pcHero;
		if (NULL == TempHero) return eEC_NullPointer;

		boost::shared_ptr<GCToSS::GMCmd> pCmd = ParseProtoMsgWithNewMemory<GCToSS::GMCmd>(pData, n32DataLength);
		if (!pCmd) return eEC_ParseProtoError; 
		

		INT32	value1 = 0;
		INT32   value2 = 0;

		vector<string> sCmdVec;
		boost::algorithm::split(sCmdVec,  pCmd->cmd() , boost::algorithm::is_any_of(" "));

		const char * cmd = strlwr( (char*)sCmdVec[0].c_str());
		if (sCmdVec.size() >= 2)
		{
			value1 = atoi(sCmdVec[1].c_str());
		}
		if (sCmdVec.size() >= 3)
		{
			value2 = atoi(sCmdVec[2].c_str());
		}

		if( strcmp("lv", cmd) == 0)
		{
			while(TempHero->GetLevel() < value1){
				TempHero->DoUpgrade();
			}
		}
		else if (strcmp("altar",cmd) ==0)
		{
			value2 = atoi(sCmdVec[1].c_str());
			CSSCfgMgr::GetSSCfgInstance().GetSSFuncSwitch().altarSwitch = value2;
		}
		else if (strcmp("monster",cmd) ==0)
		{
			value2 = atoi(sCmdVec[1].c_str());
			CSSCfgMgr::GetSSCfgInstance().GetSSFuncSwitch().monsterSwitch = value2;
		}
		else if (strcmp("npc",cmd) == 0)
		{
			value2 = atoi(sCmdVec[1].c_str());
			CSSCfgMgr::GetSSCfgInstance().GetSSFuncSwitch().solderSwitch = value2;
		}
		else if( strcmp("cp", cmd ) == 0)
		{
			value1 *= 1000;
			TempHero->ChangeCP(eCPCP_Personnal, value1, false); 
		}
		else if( strcmp("speed", cmd ) == 0)
		{
			TempHero->ChangeFPData(eEffectCate_MoveSpeed, value1 - TempHero->GetFPData(eEffectCate_MoveSpeed));
		}
		else if( strcmp("hp",cmd ) == 0)
		{
			TempHero->ChangeCurHP(TempHero,GSToGC::HPMPChangeReason::RecoverHurt,value1);
		}
		else if( strcmp("mp", cmd ) == 0)
		{
			TempHero->ChangeCurMP(TempHero,GSToGC::HPMPChangeReason::RecoverHurt,value1);
		}
		else if( strcmp("rage", cmd ) == 0)
		{
			TempHero->ChangeFuryValue(eFuryAdd, value1); 
		}
		else if( strcmp("finish", cmd ) == 0)
		{
			m_pMsgBattle->Finish(eBattleEnd_Normal, value1);
		}
		else if( strcmp("guidefinish", cmd ) == 0)
		{
		//	m_pMsgBattle->DoGuideBattleEnd();

			m_pMsgBattle->Finish(eBattleEnd_Guide,value1);
		}
		else if( strcmp("sx", cmd ) == 0)
		{
			TempHero->GetParaMgr()->AddBaseValue(value1,value2);
		}
		else if( strcmp("cd", cmd ) == 0)
		{
			TempHero->RefreshCD();
		}
		else if( strcmp("notice", cmd ) == 0)
		{
		}
		else{
			ELOG(LOG_ERROR,"-ss has not find the cmd:%s!-",pCmd->cmd().c_str());
		} 
		return eNormal;
	}

}