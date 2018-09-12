#include "SSGUParameter.h"

namespace SceneServer{

	CSSGUParameter::CSSGUParameter():m_type(eParaType_Normal)
		, m_bIfOnlyOneMinusePercent(FALSE)
		, m_value(0)
		, m_baseValue(0)
		, m_percent(0)
		, m_percentAdd(0)
		, m_bIfChanged(TRUE)
		, m_minValue(0)
		, m_maxValue(0)
		, m_hasMinValue(FALSE)
		, m_hasMaxValue(FALSE)
		, m_minBaseValue(0)
		, m_maxBaseValue(0)
		, m_hasMinBaseValue(FALSE)
		, m_hasMaxBaseValue(FALSE)
		, m_bIfCountDown(FALSE)
		, m_pMaxValueCallBack(NULL)
		, m_pValueCallback(NULL)
	{}

	CSSGUParameter::~CSSGUParameter(){

	}

	VOID	CSSGUParameter::Recount(){
		INT32 oldValue = m_value;
		if(m_type == eParaType_Normal && m_percent != 0){
			if(m_bIfCountDown)
				m_value		=	(INT32)(m_baseValue / (1 + (float)m_percent / c_fPersentDevide));
			else
				m_value		=	(INT32)(m_baseValue * (1 + (float)m_percent / c_fPersentDevide));
		}
		else{
			m_value = m_baseValue;
		}
		if(m_hasMinValue && m_value < m_minValue) {
			m_value = m_minValue;
		}
		if(m_hasMaxValue && m_value > m_maxValue){
			m_value = m_maxValue;

		}
		m_bIfChanged = TRUE;

		if(m_pMaxValueCallBack != NULL && oldValue != m_value){
			m_pMaxValueCallBack->OnMaxValueChanged(oldValue,m_value);
		}

		if(m_pValueCallback != NULL && oldValue != m_value && oldValue != 0){
			m_pValueCallback->OnValueChanged(oldValue,m_value);
		}
	}

	VOID	CSSGUParameter::RecountSingleCaculatePercent(){
		INT32 minMinuse = 0;
		for(auto iter = m_percentMinuseVec.begin(); iter != m_percentMinuseVec.end(); iter++){
			if(*iter < minMinuse) minMinuse = *iter;
		}
		m_percent = m_percentAdd + minMinuse;
	}

	VOID	CSSGUParameter::ChangeBaseValue(INT32 value){
		m_baseValue += value;
		if(m_hasMinBaseValue && m_minBaseValue > m_baseValue){
			m_baseValue = m_minBaseValue;
		}
		if(m_hasMaxBaseValue && m_maxBaseValue < m_baseValue){
			m_baseValue = m_maxBaseValue;
		}
	}

	VOID	CSSGUParameter::AddBase(INT32 value,INT32 uniqueID){
		if(uniqueID != 0){
			auto iter = m_uniqueMap.find(uniqueID);
			if(iter != m_uniqueMap.end() && iter->second > 0){
				iter->second++;
			}
			else{
				ChangeBaseValue(value);
				m_uniqueMap[uniqueID] = 1;
			}
		}
		else{
			ChangeBaseValue(value);
		}
		Recount();
	}

	VOID	CSSGUParameter::RemoveBase(INT32 value,INT32 uniqueID){
		if(uniqueID != 0){
			auto iter = m_uniqueMap.find(uniqueID);
			if(iter != m_uniqueMap.end()){
				iter->second--;
				if(iter->second == 0){
					ChangeBaseValue(-value);
				}
			}
		}
		else{
			ChangeBaseValue(-value);
		}
		Recount();
	}

	VOID	CSSGUParameter::AddPercent(INT32 value,INT32 uniqueID){
		bool bIfChange = FALSE;
		if(uniqueID != 0){
			auto iter = m_uniqueMap.find(uniqueID);
			if(iter != m_uniqueMap.end() && iter->second > 0){
				iter->second++;
			}
			else{
				bIfChange = TRUE;
				m_uniqueMap[uniqueID] = 1;
			}
		}
		else{
			bIfChange = TRUE;
		}

		if(FALSE == bIfChange) return;

		//如果是加减分开算的类型
		if(TRUE == m_bIfOnlyOneMinusePercent){
			if(value > 0){
				m_percentAdd += value;
			}
			else{
				m_percentMinuseVec.push_back(value);
			}
			RecountSingleCaculatePercent();
		}
		else{
			m_percent += value;
		}
		Recount();
	}

	VOID	CSSGUParameter::RemovePercent(INT32 value,INT32 uniqueID){
		bool bIfChange = FALSE;
		if(uniqueID != 0){
			auto iter = m_uniqueMap.find(uniqueID);
			if(iter != m_uniqueMap.end()){
				iter->second--;
				if(iter->second == 0){
					bIfChange = TRUE;
				}
			}
		}
		else{
			bIfChange = TRUE;
		}

		if(FALSE == bIfChange) return;

		//如果是加减分开算的类型
		if(TRUE == m_bIfOnlyOneMinusePercent){
			if(value > 0){
				m_percentAdd -= value;
			}
			else{
				for(auto iter = m_percentMinuseVec.begin(); iter != m_percentMinuseVec.end(); iter++){
					if(*iter == value) {
						m_percentMinuseVec.erase(iter);
						break;
					}
				}
			}
			RecountSingleCaculatePercent();
		}
		else{
			m_percent -= value;
		}
		Recount();
	}

	VOID	CSSGUParameter::OnMaxValueChanged(INT32 oldValue,INT32 newValue){
		INT32 targetNewBaseValue = oldValue==0?0:m_baseValue*newValue/oldValue;
		SetMaxBaseValue(newValue);
		if(oldValue != 0){
			AddBase(targetNewBaseValue - m_baseValue);
		}
	}
}