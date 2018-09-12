#include "ParseProto.h"

char* pBigMemory = new char[1024*1024];
char* GetStaticBigMemory(){
	return pBigMemory;
}

volatile bool m_IfBigMemoryInuse = false;
bool CheckMemory(){
	return m_IfBigMemoryInuse;
}

void ResetMemory(bool flag){
	m_IfBigMemoryInuse = flag;
}
volatile bool m_IfBigMemoryInuseThread = false;
char* pBigMemoryThread = new char[1024*1024];
char* GetStaticBigMemoryInThread(){
	return pBigMemoryThread;
}

bool CheckThreadMemory(){
	return m_IfBigMemoryInuseThread;
}

void ResetThreadMemory(bool flag){
	m_IfBigMemoryInuseThread = flag;
}


