/*
* file name			:SSGUParameter.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:Nov 26 2014
* summary			:
*
*/

#pragma once

#include "stdafx.h"
#include "ISSParameterChangeCallBack.h"

namespace SceneServer{

	static const float c_fPersentDevide = 1000.0f;

	enum EGUParameterType{
		eParaType_Normal = 0,//通过数字表达的属性
		eParaType_Status,//数字只是用来表达有无的属性
	};

	class CSSGUParameter
	{
	private:

		EGUParameterType	m_type;//属性类型
		bool		m_bIfOnlyOneMinusePercent;//是否减少的比例只计算最高值
		INT32		m_value;//最终的属性值
		INT32		m_baseValue;//基础值
		INT32		m_percent;//千分比改变量
		INT32		m_percentAdd;//千分比增加量
		vector<INT32>		m_percentMinuseVec;//千分比减少量列表
		bool		m_bIfChanged;//值是否被改变过
		INT32		m_minValue;//总值的最小值
		INT32		m_maxValue;//总值的最大值
		bool		m_hasMinValue;//是否有总值的最小值
		bool		m_hasMaxValue;//是否有总值的最大值
		INT32		m_minBaseValue;//基础值的最小值
		INT32		m_maxBaseValue;//基础值的最大值
		bool		m_hasMinBaseValue;//是否有基础值的最小值
		bool		m_hasMaxBaseValue;//是否有基础值的最大值
		bool		m_bIfCountDown;//是否按照倒数计算

		CSSGUParameter*	m_pMaxValueCallBack;//最大值改变的回调接口

		ISSParameterChangeCallBack* m_pValueCallback;//当前值改变的回调接口

		map<INT32, INT8> m_uniqueMap;

		VOID		ChangeBaseValue(INT32 value);
		VOID		Recount();
		VOID		RecountSingleCaculatePercent();

	protected:
		

	public:
		~CSSGUParameter();
		CSSGUParameter();

		VOID		SetType(EGUParameterType type){m_type = type;}
		VOID		SetIfOnlyOneMinusePercent(bool value){m_bIfOnlyOneMinusePercent = value;}
		VOID		SetCountDown(bool value){m_bIfCountDown = value;}
		VOID		SetMinValue(INT32 value){m_hasMinValue = TRUE; m_minValue = value; if(m_value < m_minValue) m_value = m_minValue;}
		VOID		SetMaxValue(INT32 value){m_hasMaxValue = TRUE; m_maxValue = value; if(m_value > m_maxValue) m_value = m_maxValue;}
		VOID		SetMinBaseValue(INT32 value){m_hasMinBaseValue = TRUE; m_minBaseValue = value; if(m_baseValue < m_minBaseValue) m_baseValue = m_minBaseValue;}
		VOID		SetMaxBaseValue(INT32 value){m_hasMaxBaseValue = TRUE; m_maxBaseValue = value; if(m_baseValue > m_maxBaseValue) m_baseValue = m_maxBaseValue;}
		VOID		SetMaxValueChangeCallback(CSSGUParameter* callback){m_pMaxValueCallBack = callback;}
		bool		IfChanged(){return m_bIfChanged;}
		VOID		OnSend(){m_bIfChanged = FALSE;}
		VOID		AddBase(INT32 value,INT32 uniqueID = 0);
		VOID		RemoveBase(INT32 value,INT32 uniqueID = 0);
		VOID		AddPercent(INT32 value,INT32 uniqueID = 0);
		VOID		RemovePercent(INT32 value,INT32 uniqueID = 0);
		INT32		GetValue(){return m_value;}
		INT32		GetValueForSend(){m_bIfChanged = FALSE; return GetValue();}
		VOID		OnMaxValueChanged(INT32 oldValue,INT32 newValue);
		VOID		SetValueChangeCallback(ISSParameterChangeCallBack* pCallBack){m_pValueCallback = pCallBack;}
	};

}