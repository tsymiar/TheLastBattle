#pragma once
#include <concurrent_queue.h>
#include "Buffer.h"
#include <boost/atomic.hpp>

using namespace std;

template <class T>
class CThreadSafeObejctPool{
private:
	Concurrency::concurrent_queue<T*>		m_OriBlock;
	Concurrency::concurrent_queue<T*>		m_ObjectBlock;
	const int		m_DefaultBlockSize;
	const int		m_BufferSize;

public:
	~CThreadSafeObejctPool(){
		while (!m_OriBlock.empty()){
			T* pBlock = NULL;
			m_OriBlock.try_pop(pBlock);
			if (NULL != pBlock){
				delete []pBlock;
			}
		}
	}

	CThreadSafeObejctPool(int blockSize = 1024, int n32BufferSize = 1024):m_DefaultBlockSize(blockSize), m_BufferSize(n32BufferSize){
	}
	int	GetMaxBlockSize(){
		return m_DefaultBlockSize;
	}
	T* AcquireObject(){
		if (true == m_ObjectBlock.empty()){
			try
			{
				T* pNewObject = new T[m_DefaultBlockSize];
			}
			catch (std::bad_alloc)
			{
				printf("Warning! MemoryLeak!\n");
				return NULL;
			}
			m_OriBlock.push(pNewObject);
			for (int i = 0; i < m_DefaultBlockSize; ++i){
				m_ObjectBlock.push(pNewObject+i);
			}
		}

		T* pGetObject = NULL;
		m_ObjectBlock.try_pop(pGetObject);
		return pGetObject;
	}

	void ReleaseObejct(T* pObject){
		if (nullptr == pObject){
			return;
		}

		pObject->Clear();

		m_ObjectBlock.push(pObject);
	}

	void Clear(){
		while (!m_ObjectBlock.empty()){
			T* pObject = nullptr;
			m_ObjectBlock.try_pop(pObject);
		}

		queue<T*> tempBlock;
		while (!m_OriBlock.empty()){
			T* pBlock = NULL;
			m_OriBlock.pop(pBlock);
			for (INT32 i = 0; i < m_DefaultBlockSize; ++i){
				m_ObjectBlock.push(pBlock + i);
			}

			tempBlock.push(pBlock);
		}

		m_OriBlock = tempBlock;
	}
};

template<>
class CThreadSafeObejctPool<Buffer>{
private:
	Concurrency::concurrent_queue<Buffer*>		m_ObjectBlock;
	const int		m_DefaultBlockSize;
	const int		m_BufferSize;
	boost::atomic_int32_t	m_TotBufferSize;
public:
	~CThreadSafeObejctPool(){
		while (!m_ObjectBlock.empty()){
			Buffer* pBlock = NULL;
			m_ObjectBlock.try_pop(pBlock);
			if (NULL != pBlock){
				delete pBlock;
			}
		}
	}

	CThreadSafeObejctPool(int blockSize = 1024, int n32BufferSize = 1024):m_DefaultBlockSize(blockSize), m_BufferSize(n32BufferSize),m_TotBufferSize(0){
	}
	int	GetMaxBlockSize(){
		return m_DefaultBlockSize;
	}
	Buffer* AcquireObject(){
		if (true == m_ObjectBlock.empty()){
			for (int i = 0; i < m_DefaultBlockSize; ++i){
				Buffer* pNewObject = new Buffer(m_BufferSize);
				m_ObjectBlock.push(pNewObject);
				++m_TotBufferSize;
			}
		}

		Buffer* pGetObject = NULL;
		m_ObjectBlock.try_pop(pGetObject);
		if (!pGetObject){
			pGetObject = new Buffer(m_BufferSize);
			++m_TotBufferSize;
		}
		return pGetObject;
	}

	void ReleaseObejct(Buffer* pObject){
		if (nullptr == pObject){
			return;
		}
		if (m_TotBufferSize > m_DefaultBlockSize * 2){
			--m_TotBufferSize;
			delete pObject;
			return;
		}
		pObject->Clear();
		m_ObjectBlock.push(pObject);
	}

	void Clear(){
	}
};