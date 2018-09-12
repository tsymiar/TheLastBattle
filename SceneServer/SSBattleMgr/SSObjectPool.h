#pragma once

#include "stdafx.h"
#include <queue>
#include <boost/thread/mutex.hpp>

namespace SceneServer{
const int DefaultPoolSize = 1024;
template <class T>
class CSSObejctPool{
private:
	queue<T*>		m_OriBlock;
	queue<T*>		m_ObjectBlock;
	const INT32		m_DefaultBlockSize;

public:
	~CSSObejctPool(){
		while (!m_OriBlock.empty()){
			T* pBlock = m_OriBlock.front();
			delete []pBlock;
			m_OriBlock.pop();
		}
	}

	CSSObejctPool(INT32 blockSize = DefaultPoolSize):m_DefaultBlockSize(blockSize){
	}

	T* AcquireObject(){
		if (true == m_ObjectBlock.empty()){
			T* pNewObject = new T[m_DefaultBlockSize];
			m_OriBlock.push(pNewObject);
			for (int i = 0; i < m_DefaultBlockSize; ++i){
				m_ObjectBlock.push(pNewObject+i);
			}
		}

		T* pGetObject = m_ObjectBlock.front();
		m_ObjectBlock.pop();
		return pGetObject;
	}

	void ReleaseObejct(T* pObject){
		if (0 == pObject){
			return;
		}

		m_ObjectBlock.push(pObject);
	}

	void Clear(){
		while (!m_ObjectBlock.empty()){
			T* pObject = m_ObjectBlock.front();
			pObject->Clear();
			m_ObjectBlock.pop();
		}

		queue<T*> tempBlock;
		while (!m_OriBlock.empty()){
			T* pBlock = m_OriBlock.front();
			for (INT32 i = 0; i < m_DefaultBlockSize; ++i){
				m_ObjectBlock.push(pBlock + i);
			}
			
			tempBlock.push(pBlock);
			m_OriBlock.pop();
		}

		m_OriBlock = tempBlock;
	}
};

}