#pragma once
#include <boost/lockfree/queue.hpp>
#include <queue>
using namespace std;
const int DefaultPoolSize = 1024;
template <class T>
class CObejctPool{
private:
	queue<T*>		m_OriBlock;
	queue<T*>		m_ObjectBlock;
	const int		m_DefaultBlockSize;

public:
	~CObejctPool(){
		while (!m_OriBlock.empty()){
			T* pBlock = m_OriBlock.front();
			m_OriBlock.pop();
			if (NULL != pBlock){
				delete []pBlock;
			}
		}
	}

	CObejctPool(int blockSize = 1024):m_DefaultBlockSize(blockSize){
	}

	T* AcquireObject(){
		if (true == m_ObjectBlock.empty()){
			try
			{
				T* pNewObject = new T[m_DefaultBlockSize];
			}
			catch(std::bad_alloc){
				printf("Warning! MemoryLeak!\n");
				return NULL;
			}
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
			m_OriBlock.pop();
			for (INT32 i = 0; i < m_DefaultBlockSize; ++i){
				m_ObjectBlock.push(pBlock + i);
			}
			
			tempBlock.push(pBlock);
		}

		m_OriBlock = tempBlock;
	}
};