#pragma once
#include <boost/shared_ptr.hpp>
char* GetStaticBigMemory();
char* GetAnotherStaticBigMemory();
template <class T>
class Deleter  
{  
public:  
	void operator()(T* pointer)  
	{  
		ResetMemory(false);
		pointer->~T();
	}  
}; 
bool CheckMemory();
void ResetMemory(bool flag);
template <class T>
boost::shared_ptr<T> ParseProtoMsg(const char* pData, int n32DataLength)
{
	if (CheckMemory()){
		boost::shared_ptr<T> msg(new T);
		bool res = msg->ParseFromArray(pData, n32DataLength);
		if (!res){
			return NULL;
		}
		return msg;
	}
	else{
		boost::shared_ptr<T> msg(new(GetStaticBigMemory()) T, Deleter<T>());
		bool res = msg->ParseFromArray(pData, n32DataLength);
		if (!res){
			return NULL;
		}

		ResetMemory(true);
		return msg;
	}
}

template <class T>
bool ParseProtoMsg(const char* pData, int n32DataLength, T& sMsg)
{
	return sMsg.ParseFromArray(pData, n32DataLength);
}

bool CheckThreadMemory();
void ResetThreadMemory(bool flag);
char* GetStaticBigMemoryInThread();
template <class T>
boost::shared_ptr<T> ParseProtoMsgInThread(const char* pData, int n32DataLength)
{
	if (CheckThreadMemory()){
		boost::shared_ptr<T> msg(new T);
		bool res = msg->ParseFromArray(pData, n32DataLength);
		if (!res){
			return NULL;
		}
		return msg;
	}
	else{
		boost::shared_ptr<T> msg(new(GetStaticBigMemoryInThread()) T, Deleter<T>());
		bool res = msg->ParseFromArray(pData, n32DataLength);
		if (!res){
			return NULL;
		}

		ResetThreadMemory(true);
		return msg;
	}
}

template <class T>
boost::shared_ptr<T> ParseProtoMsgWithNewMemory(const char* pData, int n32DataLength)
{
	boost::shared_ptr<T> msg(new T);
	bool res = msg->ParseFromArray(pData, n32DataLength);
	if (!res){
		return NULL;
	}
	return msg;
}