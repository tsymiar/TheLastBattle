
#include "SdkConnector.h"
#include "curl/curl.h"
#include "json/reader.h"
#include "json/value.h"
#include "ELoggingHeader.h"
#include "SDKAsynHandler.h"
#include <sstream>

#pragma comment (lib, "ws2_32.lib" )
#pragma comment (lib, "winmm.lib" )
#pragma comment (lib, "wldap32.lib")
#pragma comment(lib, "Advapi32.lib")

SdkConnector::SdkConnector()
{
	m_buffer.clear();
}

SdkConnector::~SdkConnector()
{
	m_buffer.clear();
}

bool	SdkConnector::GetResult_91(string jsonStr)
{
	Json::Reader reader;
	Json::Value root;
	string ret_str = "";
	ELOG(LOG_DBBUG, "%s", jsonStr.c_str());
	if (reader.parse(jsonStr, root)){
		if (root.isMember("ErrorCode")){
			ret_str = root["ErrorCode"].asString();
		}
	}

	if (0 == strcmp(ret_str.c_str(), "1")){
		return true;
	}

	return false;
}

bool	SdkConnector::GetResult_TB(string str)
{
	ELOG(LOG_DBBUG, "%s", str.c_str());
	int ret = 0;
	stringstream ss_temp;
	ss_temp << str;
	ss_temp >> ret;

	if (ret > 0){
		return true;
	}

	return false;
}

bool	SdkConnector::GetResult_PP(string jsonStr, string& account, string& creator)
{
	Json::Reader reader;
	Json::Value root;

	ELOG(LOG_DBBUG, "%s", jsonStr.c_str());
	UINT32  ret = 0;
	if (reader.parse(jsonStr, root)){
		if (root.isMember("state")){
			auto& state = root["state"];
			ret = state["code"].asInt();
		}

		if (root.isMember("data")){
			auto& data = root["data"];
			account = data["accountId"].asString();
			creator = data["creator"].asString();
		}
	}

	if (ret == 1){
		return true;
	}

	return false;
}

bool	SdkConnector::GetResult_CMGE(string jsonStr, string &tokenStr)
{
	Json::Reader reader;
	Json::Value root;
	int ret_int = 0;

	ELOG(LOG_DBBUG, "%s.", jsonStr.c_str());
	if (reader.parse(jsonStr, root)){

		if (root.isMember("error")){
			if (root.isMember("codes")){
				ret_int = root["codes"].asInt();
			}
		}
		
		if (root.isMember("access_token")){
			if (root.isMember("codes")){
				ret_int = root["codes"].asInt();
			}
			tokenStr = root["access_token"].asString();
		}
		else if(root.isMember("codes")){
			if (root["codes"].isArray()){
				stringstream ss_t;
				for(int i = 0; i < root.size(); i++){
					string t_str = root["codes"][i].asString();
					ss_t<<t_str;
					ss_t>>ret_int;
				}
			}
		}
	}

	if (ret_int == 0){
		return true;
	}

	return false;
}

string SdkConnector::GetResult_CMGE_Info(string jsonStr)
{
	Json::Reader reader;
	Json::Value root;
	int ret_int = -1;
	string ret_str = "";

	ELOG(LOG_DBBUG, "%s.", jsonStr.c_str());
	if (reader.parse(jsonStr, root)){
		if (root.isMember("codes")){
			ret_str = root["codes"].asString();
			stringstream ss_temp;
			ss_temp << ret_str;
			ss_temp >> ret_int;
			if (0 == ret_int){
				ret_int = root["id"].asInt();
				stringstream ss_temp_2;
				ss_temp_2 << ret_int;
				ss_temp_2 >> ret_str;
			}
		}
	}
	return ret_str;
}

bool SdkConnector::GetResult_UC(string jsonstr, string& uin)
{
	Json::Reader reader;
	Json::Value root;
	int ret_int;

	ELOG(LOG_DBBUG, "%s.", jsonstr.c_str());
	if (reader.parse(jsonstr, root)){
		if (root.isMember("state")){
			auto& ret_str = root["state"];
			ret_int = ret_str["code"].asInt();
		}
		if (1 == ret_int){
			if (root.isMember("data")){
				auto& ret_data = root["data"];
				INT32 ret_ucid = ret_data["ucid"].asInt();
				stringstream ss;
				ss << ret_ucid;
				ss >> uin;
			}
			return true;
		}
	}
	return false;
}

bool SdkConnector::GetResult_iTools(string jsonstr)
{
	Json::Reader read;
	Json::Value root;
	string str_ret = "";

	ELOG(LOG_DBBUG, "%s.", jsonstr.c_str());
	if (read.parse(jsonstr, root)){
		if (root.isMember("status")){
			str_ret = root["status"].asString();
		}
	}

	if(strcmp("success", str_ret.c_str()) == 0){
		return true;
	}

	return false;
}

bool SdkConnector::GetResult_OnlineGame(string jsonstr, string& guid)
{
	Json::Reader read;
	Json::Value root;

	int int_ret = -1;
	ELOG(LOG_DBBUG, "%s.", jsonstr.c_str());
	if (read.parse(jsonstr, root)){
		if (root.isMember("code")){
			int_ret = root["code"].asInt();
			if (0 == int_ret){
				string t_str = "";
				if (root.isMember("data")){
					auto& t_str = root["data"];

					guid = t_str["guid"].asString();
				}
			}
		}
	}

	if (int_ret == 0){
		return true;
	}

	return false;
}

bool SdkConnector::GetLoginResult_As(string jsonstr)
{
	Json::Reader reader;
	Json::Value root;

	int int_ret = -1;
	bool b_ret = false;
	ELOG(LOG_DBBUG, "%s.", jsonstr.c_str());
	if (reader.parse(jsonstr, root))
	{
		if (root.isMember("status"))
		{
			int_ret = root["status"].asInt();
			if (0 == int_ret)
			{
				b_ret = true;
			}
		}
	}
	return b_ret;
}

bool SdkConnector::GetLoginResult_XY(string jsonstr)
{
	Json::Reader reader;
	Json::Value root;

	int int_ret = -1;
	ELOG(LOG_DBBUG, "%s.", jsonstr.c_str());
	if (reader.parse(jsonstr, root))
	{
		if (root.isMember("ret"))
		{
			int_ret = root["ret"].asInt();
		}
	}

	if (int_ret == 0){
		return true;
	}
	return false;
}

string SdkConnector::ToMD5(string surstr)
{
	m_md5.update(surstr);
	return m_md5.toString();
}

string SdkConnector::MakeJsonStr_PP(string sessionid)
{
	Json::Value root;
	Json::Value value;

	string str_sign = "sid=";
	str_sign += sessionid;
	str_sign += appkey_pp;
	string sign = ToMD5(str_sign);

	time_t now_time;
	now_time = time(NULL);
	int cur_time = now_time;

	Json::Value json_sid;
	json_sid["sid"] = Json::Value(sessionid);

	Json::Value json_gameid;
	json_gameid["gameId"] = Json::Value(4069);

	root["id"] = Json::Value(cur_time); 
	root["service"] = Json::Value("account.verifySession");
	root["data"] = json_sid;
	root["game"] = json_gameid;
	root["encrypt"] = Json::Value("md5");
	root["sign"] = Json::Value(sign);

	Json::ValueType type = root.type();
	std::string strResult = root.toStyledString();

	return strResult;
}

string SdkConnector::MakeJsonStr_UC(string sessionid)
{
	Json::Value root;
	Json::Value value;

	time_t now_time;
	now_time = time(NULL);
	int cur_time = now_time;

	string str_sign = uc_cpId;
	str_sign += "sid=";
	str_sign += sessionid;
	str_sign += appkey_uc;
	string sign = ToMD5(str_sign);

	Json::Value json_sid;
	json_sid["sid"] = Json::Value(sessionid);

	Json::Value json_gameid;
	json_gameid["cpId"] = Json::Value(109);
	json_gameid["gameId"] = Json::Value(123456);
	json_gameid["channelId"] = Json::Value("2");
	json_gameid["serverId"] = Json::Value(654);

	root["id"] = Json::Value(cur_time); 
	root["service"] = Json::Value("ucid.user.sidInfo");
	root["data"] = json_sid;
	root["game"] = json_gameid;
	root["sign"] = Json::Value(sign);

	Json::ValueType type = root.type();
	std::string strResult = root.toStyledString();

	return strResult;
}

string SdkConnector::HttpSendData_91(const char* uin, const char* sessionid)
{
	string act = "4";
	string strSign = appid_91 + act + uin + sessionid + appkey_91;
	string md5Str = ToMD5(strSign);
	return goUrl_91 + "Appid=115561&Act=4&Uin=" + uin +"&SessionID=" + sessionid + "&Sign=" + md5Str;
}

string SdkConnector::HttpSendData_TB(const char* sessionid)
{
	string msg = goUrl_tb2;
	msg += "k=";
	msg += sessionid;
	return msg;
}

string SdkConnector::HttpSendData_PP(const char* sessionid)
{
	string str_json = MakeJsonStr_PP(sessionid);
	return str_json;
}

string SdkConnector::HttpSendData_Cmge_ZB(const char *AuthCode)
{
	string str_sign = "__e__=1";
	str_sign += "&client_id=";
	str_sign += appkey_cmge_ZB;
	str_sign += "&client_secret=";
	str_sign += gamekey_cmge_ZB;
	str_sign += "&code=";
	str_sign += AuthCode;
	str_sign += "&grant_type=";
	str_sign += grant_type_cmge;
	str_sign += "&productId=";
	str_sign += gameid_cmge_ZB;
	str_sign += "&redirect_uri=1";
	str_sign +="&";
	str_sign += appkey_cmge_ZB;

	string sign = ToMD5(str_sign);
	char temp = sign[1];
	sign[1] = sign[13];
	sign[13] = temp;

	temp = sign[5];
	sign[5] = sign[17];
	sign[17] = temp;

	temp = sign[7];
	sign[7] = sign[23];
	sign[23] = temp;

	string strResult = "__e__=1";
	strResult += "&client_id=";
	strResult += appkey_cmge_ZB;
	strResult += "&client_secret=";
	strResult += gamekey_cmge_ZB;
	strResult += "&code=";
	strResult += AuthCode;
	strResult += "&grant_type=";
	strResult += "authorization_code";
	strResult += "&productId=";
	strResult += gameid_cmge_ZB;
	strResult += "&redirect_uri=1";		
	strResult += "&sign=";
	strResult += sign;

	return strResult;
}

string SdkConnector::HttpSendData_Cmge_JB(const char *AuthCode)
{
	string str_sign = "__e__=1";
	str_sign += "&client_id=";
	str_sign += appkey_cmge_JB;
	str_sign += "&client_secret=";
	str_sign += gamekey_cmge_JB;
	str_sign += "&code=";
	str_sign += AuthCode;
	str_sign += "&grant_type=";
	str_sign += grant_type_cmge;
	str_sign += "&productId=";
	str_sign += gameid_cmge_JB;
	str_sign += "&redirect_uri=1";
	str_sign +="&";
	str_sign += appkey_cmge_JB;

	string sign = ToMD5(str_sign);
	char temp = sign[1];
	sign[1] = sign[13];
	sign[13] = temp;

	temp = sign[5];
	sign[5] = sign[17];
	sign[17] = temp;

	temp = sign[7];
	sign[7] = sign[23];
	sign[23] = temp;

	string strResult = "__e__=1";
	strResult += "&client_id=";
	strResult += appkey_cmge_JB;
	strResult += "&client_secret=";
	strResult += gamekey_cmge_JB;
	strResult += "&code=";
	strResult += AuthCode;
	strResult += "&grant_type=";
	strResult += "authorization_code";
	strResult += "&productId=";
	strResult += gameid_cmge_JB;
	strResult += "&redirect_uri=1";		
	strResult += "&sign=";
	strResult += sign;

	return strResult;
}

string SdkConnector::HttpSendData_Cmge_Android(const char *AuthCode)
{
	string str_sign = "__e__=1";
	str_sign += "&client_id=";
	str_sign += appkey_cmge_android;
	str_sign += "&client_secret=";
	str_sign += gamekey_cmge_android;
	str_sign += "&code=";
	str_sign += AuthCode;
	str_sign += "&grant_type=";
	str_sign += grant_type_cmge;
	str_sign += "&productId=";
	str_sign += gameid_cmge_android;
	str_sign += "&redirect_uri=1";
	str_sign +="&";
	str_sign += appkey_cmge_android;

	string sign = ToMD5(str_sign);
	char temp = sign[1];
	sign[1] = sign[13];
	sign[13] = temp;

	temp = sign[5];
	sign[5] = sign[17];
	sign[17] = temp;

	temp = sign[7];
	sign[7] = sign[23];
	sign[23] = temp;

	string strResult = "__e__=1";
	strResult += "&client_id=";
	strResult += appkey_cmge_android;
	strResult += "&client_secret=";
	strResult += gamekey_cmge_android;
	strResult += "&code=";
	strResult += AuthCode;
	strResult += "&grant_type=";
	strResult += "authorization_code";
	strResult += "&productId=";
	strResult += gameid_cmge_android;
	strResult += "&redirect_uri=1";		
	strResult += "&sign=";
	strResult += sign;

	return strResult;
}

string SdkConnector::HttpSendData_Cmge_Info(const char* Token)
{
	string goInfo = "access_token=";
	goInfo += Token;
	goInfo += "&productId=P11290";
	return goInfo;
}

string SdkConnector::HttpSendData_UC(const char *Token)
{
	string sendstr = MakeJsonStr_UC(Token);
	ELOG(LOG_DBBUG, "%s.", sendstr.c_str());
	return sendstr;
}

string SdkConnector::HttpSendData_iTools(string sessionid)
{
	string ts_sign = "appid=";
	ts_sign += appid_iTools;
	ts_sign += "&sessionid=";
	ts_sign += sessionid;

	string s_sign = ToMD5(ts_sign);

	string s_goMsg = "&";
	s_goMsg += ts_sign;
	s_goMsg += "&sign=";
	s_goMsg += s_sign;

	return goUrl_iTools + s_goMsg;
}

string SdkConnector::HttpSendData_OnlineGame(string tokenKey)
{
	string t_sign = appkey_OnlineGame;
	t_sign += tokenKey;

	string s_sign = ToMD5(t_sign);

	string goMsg = "tokenKey=";
	goMsg += tokenKey;
	goMsg += "&sign=";
	goMsg += s_sign;

	return goUrl_OnlineGame + goMsg;
}

string SdkConnector::HttpSendData_As(string token)
{
	string temp = "token=" + token;
	return goUrl_As + temp;
}

string SdkConnector::HttpSendData_XY(string uid, string token)
{
	string ret_str = "uid=";
	ret_str += uid;
	ret_str += "&appid=";
	ret_str += appid_XY;
	ret_str +="&token=";
	ret_str += token;
	return ret_str;
}

SdkConnector& SdkConnector::GetInstance(){
	static SdkConnector lSdkConnector;
	return lSdkConnector;
}

void SdkConnector::Update(){
	while (!m_DBCallbackQueue.empty()){
		Buffer* pBuffer = nullptr;
		m_DBCallbackQueue.try_pop(pBuffer);
		if (pBuffer->m_LogLevel == 1){
			int gcnetid = 0;
			pBuffer->GetInt(gcnetid);

			int uidlength = 0;
			pBuffer->GetInt(uidlength);
			string uid;
			pBuffer->GetString(uid, uidlength);

			int uinlength = 0;
			pBuffer->GetInt(uinlength);
			string uin;
			pBuffer->GetString(uin, uinlength);

			int sessionidlength = 0;
			pBuffer->GetInt(sessionidlength);
			string sessionid;
			pBuffer->GetString(sessionid, sessionidlength);

			int platform = 0;
			pBuffer->GetInt(platform);

			LoginUserInfo pLoginUserInfo;
			pLoginUserInfo.sessionid = sessionid;
			pLoginUserInfo.uin = uin;
			pLoginUserInfo.plat = platform;

			m_AllLoginUserInfo[sessionid] = pLoginUserInfo;
			ELOG(LOG_DBBUG, "Add uid:%s, sessionid:%s", uid.c_str(), sessionid.c_str());

			PostMsgToGC_NotifyServerList(gcnetid);
		}
		else if (pBuffer->m_LogLevel == 2){
			int gcnetid = 0;
			pBuffer->GetInt(gcnetid);
			int errorcode = 0;
			pBuffer->GetInt(errorcode);
			ELOG(LOG_DBBUG, "User Login Fail with netid:%d, errorcode:%d.", gcnetid, errorcode);
			PostMsgToGC_NotifyLoginFail(errorcode, gcnetid);
		}
		m_DBCallbackQueuePool.ReleaseObejct(pBuffer);
	}
}

void SdkConnector::SendToInsertData(const string& uid, LoginUserInfo& loginInfo, int gcnetid){
	Buffer* pBuffer =  m_DBCallbackQueuePool.AcquireObject();
	pBuffer->append(gcnetid);
	pBuffer->append(uid.size());
	pBuffer->append(uid.c_str(), uid.size());

	pBuffer->append(loginInfo.uin.size());
	pBuffer->append(loginInfo.uin.c_str(), loginInfo.uin.size());

	pBuffer->append(loginInfo.sessionid.size());
	pBuffer->append(loginInfo.sessionid.c_str(), loginInfo.sessionid.size());

	pBuffer->append((char*)&loginInfo.plat, sizeof(loginInfo.plat));
	pBuffer->m_LogLevel = 1;
	m_DBCallbackQueue.push(pBuffer);
}

void SdkConnector::SendToFailData(const int errorcode, int gcnetid)
{
	Buffer* pBuffer =  m_DBCallbackQueuePool.AcquireObject();
	pBuffer->append(gcnetid);
	pBuffer->append(errorcode);
	pBuffer->m_LogLevel = 2;
	ELOG(LOG_DBBUG, "User Login Fail with netid:%d, errorcode:%d.", gcnetid, errorcode);
	m_DBCallbackQueue.push(pBuffer);
}

size_t SdkConnector::getVersionCode(void *ptr, size_t size, size_t nmemb, void *userdata){
	std::string *version = (std::string*)userdata;
	version->append((char*)ptr, size * nmemb);
	return (size * nmemb);
}

string SdkConnector::Unicode2Utf8(const wstring& widestring)  
{  
	int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);  
	if (utf8size == 0)  
	{  
		throw std::exception("Error in conversion.");  
	}  

	vector<char> resultstring(utf8size);  

	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);  

	if (convresult != utf8size)  
	{  
		throw std::exception("La falla!");  
	}  

	return std::string(&resultstring[0]);  
}

wstring SdkConnector::Acsi2WideByte(string& strascii)  
{  
	int widesize = MultiByteToWideChar (CP_ACP, 0, (char *)strascii.c_str(), -1, NULL, 0);  
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)  
	{  
		throw std::exception("Invalid UTF-8 sequence.");  
	}  
	if (widesize == 0)  
	{  
		throw std::exception("Error in conversion.");  
	}  
	vector<wchar_t> resultstring(widesize);  
	int convresult = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);  


	if (convresult != widesize)  
	{  
		throw std::exception("La falla!");  
	}  

	return std::wstring(&resultstring[0]);  
}

void SdkConnector::PostMsgToGC_NotifyServerList(int gcnetid)
{
	// 发送第2消息：登录成功，bs服务器列表
	LSToGC::ServerBSAddr ServerList;
	map<UINT32,sServerAddr>::iterator iter = gAllServerAddr.begin();
	for(; iter != gAllServerAddr.end(); iter ++){
		LSToGC::ServerInfo *pInfo = ServerList.add_serverinfo();
		pInfo->set_servername(iter->second.str_name);
		pInfo->set_serveraddr(iter->second.str_addr);
		pInfo->set_serverport(iter->second.str_port);
	}
	ELOG(LOG_DBBUG, "Post Server List To User.");
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_LS_OnlyGC, gcnetid, ServerList, ServerList.msgid());
}

void SdkConnector::PostMsgToGC_NotifyLoginFail(int errorcode, int gcnetid)
{
	// 发送第1消息：登录失败
	LSToGC::LoginResult sMsg;
	sMsg.set_result(errorcode);
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_LS_OnlyGC, gcnetid, sMsg, sMsg.msgid());
}

string  SdkConnector::HttpPost(const char* goUrl, const char* goInfo)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, goUrl);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, goInfo);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getVersionCode);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		cout << m_buffer;
		cout << endl;
	}
	return m_buffer;
}
