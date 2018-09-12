#include "SSFPChangeData.h"
#include <set>

namespace SceneServer{

bool CFPChangeData::AddEffect(EEffectCate eEffectCate, INT32 n32ChangeNum, INT32 n32Persent, INT32 n32UniqueID){
	if(n32ChangeNum != 0){
		AddEffectChooser(eEffectCate, n32ChangeNum, TRUE, n32UniqueID);
	}
	if(n32Persent != 0){
		AddEffectChooser(eEffectCate, n32Persent, FALSE, n32UniqueID);
	}
	return TRUE;
}

bool CFPChangeData::AddEffectChooser(EEffectCate eCate, INT32 n32ChangeNum, bool bIfBase, INT32 n32UniqueID){
	INT16 n16EmptyIndex = -1;
	bool bRemoved = FALSE;
	for(int i=0; i<c_FPEffectSize; ++i){
		SFPChangeEffect* pEffect = &m_Effects[i];
		if(pEffect->eCate == eEffectCate_None){
			if(n16EmptyIndex == -1){
				n16EmptyIndex = i;
			}
			continue;
		}
		//如果有属性相同数值相反的项，则用移除代替添加
		if(pEffect->eCate == eCate && pEffect->n32ChangeNum == -n32ChangeNum && pEffect->bIfBase == bIfBase){
			pEffect->eCate = eEffectCate_None;
			bRemoved = TRUE;
			break;
		}
	}

	//如果没有找到可抵消的效果
	if(FALSE == bRemoved && n16EmptyIndex >= 0){
		SFPChangeEffect* pEffect = &m_Effects[n16EmptyIndex];
		pEffect->eCate = eCate;
		pEffect->n32ChangeNum = n32ChangeNum;
		pEffect->bIfBase = bIfBase;
		pEffect->n32UniqueID = n32UniqueID;
		return TRUE;
	}
	return FALSE;

	
}

SFightProperty CFPChangeData::getFPBase(){
	SFightProperty fpOther;
	ResetTempIDArray();
	for(int i=0; i<c_FPEffectSize; ++i){
		SFPChangeEffect* pEffect = &m_Effects[i];
		if(FALSE == pEffect->bIfBase) continue;
		if(pEffect->n32UniqueID != 0 && pEffect->eCate == eEffectCate_ChangeMoveSpeed){
			if(IfRepeatInTempIDArray(pEffect->n32UniqueID,TRUE)){
				continue;
			}
		}
		AddFPValue(fpOther, pEffect->eCate, pEffect->n32ChangeNum);
	}
	return fpOther;
}

SFightProperty CFPChangeData::getFPPercent(){
	SFightProperty fpAdd;
	SFightProperty fpMinus;
	SFightProperty fpOther;
	ResetTempIDArray();
	for(int i=0; i<c_FPEffectSize; ++i){
		SFPChangeEffect* pEffect = &m_Effects[i];
		if(TRUE == pEffect->bIfBase) continue;
		if(pEffect->n32UniqueID != 0 && pEffect->eCate == eEffectCate_ChangeMoveSpeed){
			if(IfRepeatInTempIDArray(pEffect->n32UniqueID,TRUE)){
				continue;
			}
		}
		if(pEffect->eCate == eEffectCate_ChangeAttackSpeed || pEffect->eCate == eEffectCate_ChangeMoveSpeed){
			//如果是移动速度和攻击速度，需要分开计算

			if(pEffect->n32ChangeNum < 0){
				//如果是减益效果，只有最大值生效
				INT32 oldValue = GetFPValue(fpMinus,pEffect->eCate);
				if(oldValue > pEffect->n32ChangeNum){
					SetFPValue(fpMinus,pEffect->eCate,pEffect->n32ChangeNum);
				}
			}
			else{
				AddFPValue(fpAdd,pEffect->eCate,pEffect->n32ChangeNum);
			}
		}
		else{
			AddFPValue(fpOther,pEffect->eCate,pEffect->n32ChangeNum);
		}
		
	}
	return fpOther + fpAdd + fpMinus;
}

INT32	CFPChangeData::GetFPValue(SFightProperty& fp,EEffectCate eCate){
	switch(eCate) {
	case eEffectCate_ChangePhyAttack:
		return fp.n32PhyAttPower;
	case eEffectCate_ChangeMagicAttack:
		return fp.n32MagicAttPower;
	case eEffectCate_ChangePhyDefense:
		return fp.n32PhyDefAbility;
	case eEffectCate_ChangeMagicDefense:
		return fp.n32MagicDefAbility;
	case eEffectCate_ChangeMoveSpeed:
		return fp.n32MoveSpeed;
	case eEffectCate_ChangeAttackSpeed:
		return fp.n32AttackSpeed;
	case eEffectCate_ChangeMaxHP:
		return fp.n32MaxHP;
	case eEffectCate_ChangeMaxMP:
		return fp.n32MaxMP;
	case eEffectCate_ChangeHPRecoverRate:
		return fp.n32HPRecover;
	case eEffectCate_ChangeMPRecoverRate:
		return fp.n32MPRecover;
	case eEffectCate_ChangeReliveTime:
		return fp.n32ReliveSecond;
	case eEffectCate_CriPersent:
		return fp.n32CriPersent;
	case eEffectCate_CriHarm:
		return fp.n32CriHarm;
	case eEffectCate_CPRecover:
		return fp.n32CPRecover;
	case eEffectCate_Dizziness:
		return fp.n32DizzinessCounter;
	case eEffectCate_Silence:
		return fp.n32SilenceCounter;
	case eEffectCate_Disarm:
		return fp.n32DisarmCounter;
	case eEffectCate_Restrain:
		return fp.n32RestrainCounter;
	case eEffectCate_Invisible:
		return fp.n32InvisibleCounter;
	default:
		return 0;
	}
	
}

void	CFPChangeData::SetFPValue(SFightProperty& fp,EEffectCate eCate, INT32 n32Value){
	switch(eCate) {
	case eEffectCate_ChangePhyAttack:
		fp.n32PhyAttPower = n32Value;
		break;
	case eEffectCate_ChangeMagicAttack:
		fp.n32MagicAttPower = n32Value;
		break;
	case eEffectCate_ChangePhyDefense:
		fp.n32PhyDefAbility = n32Value;
		break;
	case eEffectCate_ChangeMagicDefense:
		fp.n32MagicDefAbility = n32Value;
		break;
	case eEffectCate_ChangeMoveSpeed:
		fp.n32MoveSpeed = n32Value;
		break;
	case eEffectCate_ChangeAttackSpeed:
		fp.n32AttackSpeed = n32Value;
		break;
	case eEffectCate_ChangeMaxHP:
		fp.n32MaxHP = n32Value;
		break;
	case eEffectCate_ChangeMaxMP:
		fp.n32MaxMP = n32Value;
		break;
	case eEffectCate_ChangeHPRecoverRate:
		fp.n32HPRecover = n32Value;
		break;
	case eEffectCate_ChangeMPRecoverRate:
		fp.n32MPRecover = n32Value;
		break;
	case eEffectCate_ChangeReliveTime:
		fp.n32ReliveSecond = n32Value;
		break;
	case eEffectCate_CriPersent:
		fp.n32CriPersent = n32Value;
		break;
	case eEffectCate_CriHarm:
		fp.n32CriHarm = n32Value;
		break;
	case eEffectCate_CPRecover:
		fp.n32CPRecover = n32Value;
		break;
	case eEffectCate_Dizziness:
		fp.n32DizzinessCounter = n32Value;
		break;
	case eEffectCate_Silence:
		fp.n32SilenceCounter = n32Value;
		break;
	case eEffectCate_Disarm:
		fp.n32DisarmCounter = n32Value;
		break;
	case eEffectCate_Restrain:
		fp.n32RestrainCounter = n32Value;
	case eEffectCate_Invisible:
		fp.n32InvisibleCounter = n32Value;
		break;
	}
}

void	CFPChangeData::AddFPValue(SFightProperty& fp,EEffectCate eCate, INT32 n32Value){
	switch(eCate) {
	case eEffectCate_ChangePhyAttack:
		fp.n32PhyAttPower += n32Value;
		break;
	case eEffectCate_ChangeMagicAttack:
		fp.n32MagicAttPower += n32Value;
		break;
	case eEffectCate_ChangePhyDefense:
		fp.n32PhyDefAbility += n32Value;
		break;
	case eEffectCate_ChangeMagicDefense:
		fp.n32MagicDefAbility += n32Value;
		break;
	case eEffectCate_ChangeMoveSpeed:
		fp.n32MoveSpeed += n32Value;
		break;
	case eEffectCate_ChangeAttackSpeed:
		fp.n32AttackSpeed += n32Value;
		break;
	case eEffectCate_ChangeMaxHP:
		fp.n32MaxHP += n32Value;
		break;
	case eEffectCate_ChangeMaxMP:
		fp.n32MaxMP += n32Value;
		break;
	case eEffectCate_ChangeHPRecoverRate:
		fp.n32HPRecover += n32Value;
		break;
	case eEffectCate_ChangeMPRecoverRate:
		fp.n32MPRecover += n32Value;
		break;
	case eEffectCate_ChangeReliveTime:
		fp.n32ReliveSecond += n32Value;
		break;
	case eEffectCate_CriPersent:
		fp.n32CriPersent += n32Value;
		break;
	case eEffectCate_CriHarm:
		fp.n32CriHarm += n32Value;
		break;
	case eEffectCate_CPRecover:
		fp.n32CPRecover += n32Value;
		break;
	case eEffectCate_Dizziness:
		fp.n32DizzinessCounter += n32Value;
		break;
	case eEffectCate_Silence:
		fp.n32SilenceCounter += n32Value;
		break;
	case eEffectCate_Disarm:
		fp.n32DisarmCounter += n32Value;
		break;
	case eEffectCate_Restrain:
		fp.n32RestrainCounter += n32Value;
		break;
	case eEffectCate_Invisible:
		fp.n32InvisibleCounter += n32Value;
	}
}

}