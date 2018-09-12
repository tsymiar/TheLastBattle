#pragma once

#include "stdafx.h"

template <class Element, INT32 N>
struct ElementArr{
	struct ArrInside{
		Element ele;
		bool isVaild;

		ArrInside(){Clear();}
		void Clear(){
			isVaild = FALSE;
		}
	};

	ArrInside m_Arr[N];
	INT32	m_CurSize;
	ElementArr(){
		m_CurSize = 0;
		for (int i = 0; i < N; ++i){
			m_Arr[i].isVaild = false;
		}
	}
	INT32	GetMaxSize(){
		return N;
	}
	INT32	AddElement(const Element& element);
	bool	IsHvElement(const Element& element);
	INT32	RemoveElement(const Element& element);
	void	Clear();
	bool	IsEmpty(){
		return m_CurSize == 0;
	}

	INT32	GetCurSize(){
		return m_CurSize;
	}

	Element*	Begin();
	Element*	Next();
	Element*	End();
	Element*	GetValueFromIndex(int index);
	bool		RemoveIndex(int index);
private:
	INT32		n32NextIndex;
};