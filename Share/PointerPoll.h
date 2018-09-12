#pragma once

#include <list>
#include <map>
#include <vector>

#define PP_MAX 10000
#define PP_INVALID 0xFFFFFFFF

template<typename T>
class PointerPool
{
public:
	PointerPool()
	{
		memset(mPointers,0,sizeof(T*)*PP_MAX);
		memset(mIncrease,0,sizeof(UINT32)*PP_MAX);
		mMaxPos = 1;
		mInvalidPos_Begin = mInvalidPos_Finish = PP_INVALID;
	}
	virtual ~PointerPool()
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
	UINT32 AddPointer(T* pointer,UINT32 income,UINT32& vritual_income)
	{
		if (mMaxPos<PP_MAX)
		{//1,增加到最大值//
			mPointers[mMaxPos] = pointer;
			vritual_income = income;
			while(mIncrease[vritual_income%PP_MAX]!=0) { ++vritual_income; }
			mIncrease[vritual_income%PP_MAX] = mMaxPos;
			mPointerMap.insert(make_pair(mMaxPos,pointer));
			return mMaxPos++;
		}
		else
		{//2,获取无效列表//
			UINT32 invalidPos = GetInvalid();
			if (invalidPos!=PP_INVALID)
			{
				mPointers[invalidPos] = pointer;
				vritual_income = income;
				while(mIncrease[vritual_income%PP_MAX]!=0) { ++vritual_income; }
				mIncrease[vritual_income%PP_MAX] = invalidPos;
				mPointerMap.insert(make_pair(invalidPos,pointer));
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
	T* GetPointerWithIncome(UINT32 vritual_income)
	{
		UINT32 pos = mIncrease[vritual_income%PP_MAX];
		return GetPointer(pos);
	}
	void RemovePointer(UINT32 pos,UINT32 vritual_income)
	{
		if (pos<PP_MAX)
		{
			if (mPointers[pos])
			{
				mPointers[pos] = NULL;
				mIncrease[vritual_income%PP_MAX] = 0;
				mPointerMap.erase(pos);
				SetInvalid(pos);
			}
		}
	}
	void BeginIterator()
	{
		mMapIterator = mPointerMap.begin();
	}
	T* GetNextPointer()
	{
		if (mMapIterator!=mPointerMap.end())
			return (mMapIterator++)->second;
		return NULL;
	}
private:
	void SetInvalid(UINT32 pos)//后面放入//
	{
		mPointers[pos] = (T*)PP_INVALID;
		if (mInvalidPos_Begin==PP_INVALID)
		{
			mInvalidPos_Begin = pos;
			mInvalidPos_Finish = pos;
		}
		else
		{
			mPointers[mInvalidPos_Finish] = (T*)pos;
			mInvalidPos_Finish = pos;
		}
	}
	UINT32 GetInvalid()//前面取出//
	{
		if (mInvalidPos_Begin==PP_INVALID) return PP_INVALID;
		UINT32 first = mInvalidPos_Begin;
		mInvalidPos_Begin = (UINT32)mPointers[mInvalidPos_Begin];
		return first;
	}
private:
	T* mPointers[PP_MAX];
	UINT32 mIncrease[PP_MAX];//递增ID
	UINT32 mMaxPos;
	typedef typename std::map<UINT32,T*> PointerList;
	typedef typename std::map<UINT32,T*>::iterator PointerListIt;
	PointerList mPointerMap;
	PointerListIt mMapIterator;
	//////////////////////////////////////////////////////////////////////////
	UINT32 mInvalidPos_Begin;
	UINT32 mInvalidPos_Finish;
};
