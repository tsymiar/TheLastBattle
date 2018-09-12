#pragma once

#include "../PointerPoll.h"

template<typename T>
class NetPointerPool
{
public:
	typedef typename std::vector<T*> PointerList;
	typedef typename std::vector<T*>::iterator PointerListIt;
	NetPointerPool()
	{
		memset(mPointers,0,sizeof(T*)*PP_MAX);
		mMaxPos = 1;
	}
	virtual ~NetPointerPool()
	{
	}
public:
	bool IsFull()
	{ 
		return mPointerMap.size()==PP_MAX;
	}
	UINT32 GetSize()
	{
		return mPointerMap.size();
	}
	UINT32 AddPointer(T* pointer)
	{
		if (mMaxPos<PP_MAX)
		{//1,增加到最大值//
			mPointers[mMaxPos] = pointer;
			mPointerMap.push_back(pointer);
			return mMaxPos++;
		}
		else if (mInvalidPos.size()>0)
		{//2,获取无效列表//
			UINT32 invalidPos = mInvalidPos.front();
			if (invalidPos!=PP_INVALID)
			{
				mPointers[invalidPos] = pointer;
				mPointerMap.push_back(pointer);
				mInvalidPos.pop_front();
				return invalidPos;
			}
		}
		return PP_INVALID;
	}
	T* GetPointer(UINT32 pos)
	{
		if (pos<PP_MAX)
		{
			return mPointers[pos];
		}
		return NULL;
	}
	void RemovePointer(UINT32 pos)
	{
		if (pos<PP_MAX)
		{
			T* tempPointer = mPointers[pos];
			if (tempPointer!=NULL)
			{
				for (auto it=mPointerMap.begin();it!=mPointerMap.end();++it)
				{
					if (tempPointer==(*it))
					{
						mPointerMap.erase(it);
						break;
					}
				}
				mPointers[pos] = NULL;
				mInvalidPos.push_back(pos);
			}
		}
	}
	PointerList& GetPointerMap() { return mPointerMap; }
private:
	T* mPointers[PP_MAX];
	UINT32 mMaxPos;
	PointerList mPointerMap;
	std::list<UINT32> mInvalidPos;
};
