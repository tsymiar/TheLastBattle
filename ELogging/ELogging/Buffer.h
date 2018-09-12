#pragma once
#include <cstdio>
#include <memory>
#include <string>

class  Buffer
{
public:
	Buffer():m_LogLevel(0), m_MaxLength(1024*10), m_OriMaxLength(m_MaxLength), m_pMsg(new char[m_MaxLength]), m_CurLen(0), m_ActorID(0), m_GOLen(0){}
	~Buffer(){
		if (NULL != m_pMsg)
			delete []m_pMsg;
	}
	Buffer(int size):m_LogLevel(0), m_MaxLength(size),m_OriMaxLength(m_MaxLength)
		, m_pMsg(new char[m_MaxLength]), m_CurLen(0), m_ActorID(0), m_GOLen(0){

	}
	bool append(int num){
		return append((char*)&num, sizeof(num));
	}
	bool append(const char* pData){
		if (!pData){
			return false;
		}
		size_t len = strlen(pData);
		return append(pData, len);
	}
	bool append(const char* pData, size_t length){
		if (!pData){
			return false;
		}
		while(m_CurLen + length > m_MaxLength){
			m_MaxLength *= 2;
			m_pMsg = (char*)realloc(m_pMsg, m_MaxLength);
		}

		memcpy(m_pMsg + m_CurLen, pData, length);
		m_CurLen += length;
		return true;
	}
	bool append(const std::string& str){
		return append(str.c_str(), str.size());
	}
	bool extend(size_t length){
		while(m_CurLen + length > m_MaxLength){
			m_MaxLength *= 2;
			m_pMsg = (char*)realloc(m_pMsg, m_MaxLength);
		}
		m_CurLen += length;
		return true;
	}
	void Clear(){
		if (m_MaxLength > m_OriMaxLength){
			delete []m_pMsg;
			m_pMsg = new char[m_OriMaxLength];
			m_MaxLength = m_OriMaxLength;
		}
		if (m_CurLen > 0){
			m_CurLen = 0;
		}
		m_GOLen = 0;
	}
	char* GetDataHeader(){
		return m_pMsg;
	}
	bool GetInt(int& value){
		if (m_GOLen < m_CurLen){
			value = *(int*)(m_pMsg + m_GOLen);
			m_GOLen += sizeof(int);
			return true;
		}
		return false;
	}
	char* GetCurData(){
		return m_pMsg + m_GOLen;
	}
	bool GetString(std::string& value, int valueSize){
		if (m_GOLen < m_CurLen){
			std::string temp(m_pMsg + m_GOLen, valueSize);
			m_GOLen += valueSize;
			value = temp;
			return true;
		}
		return false;
	}
	int		m_LogLevel;
	int		m_ActorID;
	int		GetMaxLength();
	void	SetCurLen(int len){m_CurLen = len;}
	int		GetDataLength(){
		return m_CurLen;
	}
private:
	int		m_GOLen;
	int		m_MaxLength;
	const int m_OriMaxLength;
	int		m_CurLen;
	char*	m_pMsg;
};
