#include "stdafx.h"

namespace SceneServer{

const UINT16 c_FPEffectSize = 128;
const UINT16 c_MaxSaveFPEffectSize = 32;

struct SFPChangeEffect{
	EEffectCate eCate;
	INT32 n32ChangeNum;
	bool	bIfBase;
	INT32 n32UniqueID;
	void Clear(){
		eCate = eEffectCate_None;
		n32ChangeNum = 0;
		bIfBase = FALSE;
		n32UniqueID = 0;
	}
};

class CFPChangeData{
private:
	SFPChangeEffect m_Effects[c_FPEffectSize];
	INT32			m_TempIDArray[c_MaxSaveFPEffectSize];
	void			ResetTempIDArray(){memset(m_TempIDArray,0,c_MaxSaveFPEffectSize*sizeof(INT32));}
	bool			IfRepeatInTempIDArray(INT32 ID, bool bIfAdd){
		bool bIfRepeat = FALSE;
		for(int j=0;j<32;j++){
			if(m_TempIDArray[j] == ID){
				bIfRepeat = TRUE;
				break;
			}
		}
		if(bIfAdd && FALSE == bIfRepeat){
			for(int j=0;j<32;j++){
				if(m_TempIDArray[j] == 0){
					m_TempIDArray[j] = ID;
					break;
				}
			}
		}
		return bIfRepeat;
	}

	bool AddEffectChooser(EEffectCate eCate, INT32 n32ChangeNum, bool bIfBase, INT32 n32UniqueID);

	void	SetFPValue(SFightProperty& fp,EEffectCate eCate, INT32 n32Value);
	void	AddFPValue(SFightProperty& fp,EEffectCate eCate, INT32 n32Value);
public:
	CFPChangeData(){
		for (int i = 0; i < c_FPEffectSize; ++i){
			m_Effects[i].Clear();
		}
		for (INT32 i = 0; i < c_MaxSaveFPEffectSize; ++i){
			m_TempIDArray[i] = 0;
		}
	}
	void Clear(){
		memset(m_Effects,0,c_FPEffectSize * sizeof(SFPChangeEffect));
	}
	INT32	GetFPValue(SFightProperty& fp,EEffectCate eCate);
	bool AddEffect(EEffectCate eEffectCate, INT32 n32ChangeNum, INT32 n32Persent, INT32 n32UniqueID);

	SFightProperty getFPBase();
	SFightProperty getFPPercent();
};

}