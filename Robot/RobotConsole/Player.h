#pragma once
#include <set>
#include "Hero.h"

namespace ReBot
{
	class CCClient;
	class CPlayerAtkAI;
	class CPlayerMoveAI;
	class CPlayerOccupyAI;
	class CRobotNPC;
	class AbsorbAI;

	class CRobotPlayer : public CRebotHero
	{
	public:
		CRobotPlayer();
		virtual ~CRobotPlayer();

		virtual INT32				OnHeartBeat(TIME_MILSEC tUTCMilsec);
		virtual	INT32				Clear();
		void						SetRobot(CCClient* pClient);
		void						ClearLockedTar();
		CClientGameObject*			FindRandEnemyAndLock();
		CClientGameObject*			FindNearestEnemy(BOOLEAN bIfHpEvent);
		CRobotNPC*					FindNearestAbosrbNPC();
		vector<RobotSkillData*>		GetCanUseSkillVec();
		RobotSkillData*				GetRandSkill();
		BOOLEAN						IfInReleaseSkillRange(const SSNewSkillCfg& sCfg, CClientGameObject& pGO);
		CCClient*					GetClient();
		virtual void				UpdateState(EGOActionState state, const CVector3D& curPos, const CVector3D& dir);
		virtual	void				UpdateHP(INT32 n32Val);
		virtual void				UpdateAbsorb(UINT32 un32ID1, UINT32 un32ID2);
		void						UpdateGoodsState(GSToGC::NotifyGoodsInfo_GoodsInfo& pInfo);
	public:
		INT32						m_n32CurRoadIndex;
		CCClient*					m_pCClient;
		TIME_MILSEC					m_SendMoveTarTime;
		TIME_MILSEC					m_StateChangeTime;
		EPlayerState				m_EPlayerState;
		CClientGameObject*			m_LockEnemy;
		CPlayerOccupyAI*			m_pPlayerOccupyAI;
		TIME_MILSEC					m_LockTime;
		AbsorbAI*					m_pAbsorbAI;
		INT32						m_AbsorbTimes;
		bool						m_IfAbsorbed1;
		bool						m_IfAbsorbed2;
		bool						m_IfRepostSolider;
		std::set<UINT32>			m_BuyGoods;
		std::set<UINT32>			m_TryToBuGoods;
	};
}


