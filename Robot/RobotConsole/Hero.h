#pragma once
#include "stdafx.h"
#include "GameUnit.h"

namespace ReBot{
	class CCClient;
	class CRebotHero : public CClientGameObject
	{
	public:
		CRebotHero(void);
		virtual ~CRebotHero(void);
		virtual INT32	Clear();

		virtual	RobotSkillData*		GetSkillData(UINT8 un8SkillSlotID){return &m_asSkilLRunTimeData[un8SkillSlotID];}
		void						UpdateLvel(UINT32 un32Level){m_un32Level = un32Level;}
		void						UpdateFury(INT32 n32Val){m_n32Fury = n32Val;}
		virtual	BOOLEAN				IsHero(){return TRUE;}
		void						UpdateGoods(UINT8 un8Pos, INT32 n32Num, UINT32 un32TypeID, INT32 n32State, UINT32 un32StateTime);
		virtual void				UpdateAbsorb(UINT32 un32ID1, UINT32 un32ID2);
		virtual	INT32				LoadHeroCfg(const SHeroCfg* pcCfg);
		void						UpdateExp(INT32 n32Val) {m_n32Exp = n32Val;}
		virtual	INT32				GetCollideRadius(){return m_cpsHeroCfg->n32CollideRadius;}
		virtual void				UpdateSkill(UINT32 n32SkillID, UINT32 un32StateTime);

	public:
		SGoodsRunTimeData			m_asGoodsRunTimeData[c_un32MaxGridNum];
		RobotSkillData				m_asSkilLRunTimeData[n32MaxRobotPlayerSkillNum];
		INT32						m_n32MonsterID1;
		INT32						m_n32MonsterID2;
		UINT32						m_un32Level;
		INT32						m_n32Fury;
		const SHeroCfg*				m_cpsHeroCfg;
		BOOLEAN						m_IsLocalPlayer;
		INT32						m_n32Exp;
		BOOLEAN						m_bIfAbsorb;
	};
}


