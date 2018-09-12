#pragma once

#include "stdafx.h"
#include "SSElementArr.h"

template <class Element, INT32 N>
INT32 ElementArr<Element, N>::AddElement(const Element& element){
	if (m_CurSize == N){
		return -1;
	}

	for (INT32 i = 0; i < N; ++i){
		if (FALSE == m_Arr[i].isVaild){
			m_Arr[i].ele = element;
			m_Arr[i].isVaild = TRUE;
			++m_CurSize;
			return i;
		}
	}

	return -1;
}

template <class Element, INT32 N>
INT32 ElementArr<Element, N>::RemoveElement(const Element& element){
	if (m_CurSize == 0){
		return -1;
	}

	for (INT32 i = 0; i < N; ++i){
		if (m_Arr[i].isVaild && m_Arr[i].ele == element){
			m_Arr[i].isVaild = FALSE;
			--m_CurSize;
			return i;
		}
	}

	return -1;
}


template <class Element, INT32 N>
bool ElementArr<Element, N>::RemoveIndex(int index){
	m_Arr[index].isVaild = FALSE;
	--m_CurSize;
	return true;
}

template <class Element, INT32 N>
bool ElementArr<Element, N>::IsHvElement(const Element& element){
	if (m_CurSize == 0){
		return FALSE;
	}

	for (INT32 i = 0; i < N; ++i){
		if (TRUE == m_Arr[i].isVaild && m_Arr[i].ele == element){
			return TRUE;
		}
	}

	return FALSE;
}

template <class Element, INT32 N>
void ElementArr<Element, N>::Clear(){
	for (INT32 i = 0; i < N; ++i){
		m_Arr[i].Clear();
	}

	m_CurSize = 0;
}

template <class Element, INT32 N>
Element* ElementArr<Element, N>::Begin(){
	n32NextIndex = 0;
	if (m_CurSize == 0){
		return NULL;
	}

	for (INT32 i = 0; i < N; ++i){
		if (TRUE == m_Arr[i].isVaild){
			n32NextIndex = i+1;
			return &m_Arr[i].ele;
		}
	}

	return NULL;
}

template <class Element, INT32 N>
Element* ElementArr<Element, N>::Next(){
	if (m_CurSize == 0){
		return NULL;
	}

	for (INT32 i = n32NextIndex; i < N; ++i){
		if (TRUE == m_Arr[i].isVaild){
			n32NextIndex = i+1;
			return &m_Arr[i].ele;
		}
	}

	return NULL;
}

template <class Element, INT32 N>
Element* ElementArr<Element, N>::End(){
	return NULL;
}


template <class Element, INT32 N>
Element* ElementArr<Element, N>::GetValueFromIndex(int index){
	if (index < 0 || !m_Arr[index].isVaild){
		return NULL;
	}
	return &m_Arr[index].ele;
}
