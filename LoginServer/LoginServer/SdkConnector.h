#pragma once
#include "stdafx.h"
#include <string>
#include "LSDefine.h"
#include "md5Handler.h"
#include "../ELogging/ELogging/ThreadSafeQueue.h"

using namespace std;
class Buffer;
typedef map<string, LoginUserInfo> LoginUserMap;

class SdkConnector
{
private:
	MD5Handler		m_md5;
	string	m_buffer;
	Concurrency::concurrent_queue<Buffer*>		m_DBCallbackQueue;
	CThreadSafeObejctPool<Buffer>	m_DBCallbackQueuePool;
	SdkConnector();
	LoginUserMap m_AllLoginUserInfo;
public:
	LoginUserMap&			GetLoginUserMap(){return m_AllLoginUserInfo;}
	static SdkConnector&	GetInstance();


	~SdkConnector();
	void	Update();
	void	SendToInsertData(const string& uid, LoginUserInfo& loginInfo, int gcnetid);
	void	SendToFailData(const int errorcode, int gcnetid);

	bool GetResult_91(string jsonStr);
	bool GetResult_TB(string str);
	bool GetResult_PP(string jsonStr, string& account, string& creator);
	bool GetResult_CMGE(string jsonStr, string &tokenStr);
	string GetResult_CMGE_Info(string jsonStr);
	bool GetResult_UC(string jsonstr,string& uin);
	bool GetResult_iTools(string jsonstr);
	bool GetResult_OnlineGame(string jsonstr, string& guid);
	bool GetLoginResult_As(string jsonstr);
	bool GetLoginResult_XY(string jsonstr);

	string MakeJsonStr_PP(string sessionid);
	string MakeJsonStr_UC(string sessionid);

	//Asyn Login
	string HttpSendData_91(const char* uin, const char* sessionid);
	string HttpSendData_TB(const char* sessionid);
	string HttpSendData_PP(const char* sessionid);
	string HttpSendData_Cmge_JB(const char *AuthCode);
	string HttpSendData_Cmge_ZB(const char *AuthCode);
	string HttpSendData_Cmge_Android(const char *AuthCode);
	string HttpSendData_Cmge_Info(const char *Token);
	string HttpSendData_UC(const char *Token);
	string HttpSendData_iTools(string sessionid);
	string HttpSendData_OnlineGame(string tokenKey);
	string HttpSendData_As(string token);
	string HttpSendData_XY(string uin,string token);

	string ToMD5(string surstr);
	static size_t getVersionCode(void *ptr, size_t size, size_t nmemb, void *userdata);
	static wstring Acsi2WideByte(string& strascii) ;
	static string Unicode2Utf8(const wstring& widestring);

	void	PostMsgToGC_NotifyServerList(int gcnetid);
	void	PostMsgToGC_NotifyLoginFail(int errorcode, int gcnetid);
	string  HttpPost(const char* goUrl, const char* goInfo);
};
