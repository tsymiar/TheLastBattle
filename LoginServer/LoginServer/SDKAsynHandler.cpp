#include "StdAfx.h"
#include "SDKAsynHandler.h"
#include "../ELogging/ELogging/SSActive.h"
#include "../../ELogging/ELoggingHeader.h"
#include "../ELogging/ELogging/Buffer.h"
#include <boost/bind.hpp>
#include <map>
#include "tinyxml.h" 
#include "tinystr.h"
#include "Net/LSNetSessionMgr.h"
#include "SdkConnector.h"

const INT32		m_CheckTime = 100;
GlobalInfo gGlobalInfoInstance;
Buffer* pPostBuffer = new Buffer(1024);
bool IfPostVisit(EUserPlatform eplat);
bool IfTestPlatform(EUserPlatform eplat);
bool IfHttps(EUserPlatform eplat);

SDKAsynHandler::SDKAsynHandler()
{
}
SDKAsynHandler::~SDKAsynHandler(void)
{
}
SDKAsynHandler& SDKAsynHandler::GetInstance(){
	static SDKAsynHandler lSDKAsynHandler;
	return lSDKAsynHandler;
}

/* boost::asio related objects
 * using global variables for simplicity
 */
boost::asio::io_service io_service;
boost::asio::deadline_timer timer(io_service);
boost::asio::deadline_timer newtimer(io_service);
std::map<curl_socket_t, boost::asio::ip::tcp::socket *> socket_map;
std::map<CURL*, ConnInfo*> conninfot_map;
static void timer_cb(const boost::system::error_code & error, GlobalInfo *g);

/* Update the event timer after curl_multi library calls */
static int multi_timer_cb(CURLM *multi, long timeout_ms, GlobalInfo *g)
{
  /* cancel running timer */
  timer.cancel();

  if ( timeout_ms > 0 )
  {
    /* update timer */
    timer.expires_from_now(boost::posix_time::millisec(timeout_ms));
    timer.async_wait(boost::bind(&timer_cb, _1, g));
  }
  else
  {
    /* call timeout function immediately */
    boost::system::error_code error; /*success*/
    timer_cb(error, g);
  }

  return 0;
}

/* Die if we get a bad CURLMcode somewhere */
static void mcode_or_die(const char *where, CURLMcode code)
{
  if ( CURLM_OK != code )
  {
    const char *s;
    switch ( code )
    {
    case CURLM_CALL_MULTI_PERFORM: s="CURLM_CALL_MULTI_PERFORM"; break;
    case CURLM_BAD_HANDLE:         s="CURLM_BAD_HANDLE";         break;
    case CURLM_BAD_EASY_HANDLE:    s="CURLM_BAD_EASY_HANDLE";    break;
    case CURLM_OUT_OF_MEMORY:      s="CURLM_OUT_OF_MEMORY";      break;
    case CURLM_INTERNAL_ERROR:     s="CURLM_INTERNAL_ERROR";     break;
    case CURLM_UNKNOWN_OPTION:     s="CURLM_UNKNOWN_OPTION";     break;
    case CURLM_LAST:               s="CURLM_LAST";               break;
    default: s="CURLM_unknown";
      break;
    case     CURLM_BAD_SOCKET:         s="CURLM_BAD_SOCKET";
		ELOG(LOG_ERROR, "ERROR: %s returns %s", where, s);
      //fprintf(MSG_OUT, "\nERROR: %s returns %s", where, s);
      /* ignore this error */
      return;
    }
  }
}

/* Check for completed transfers, and remove their easy handles */
static void check_multi_info(GlobalInfo *g)
{
  char *eff_url;
  CURLMsg *msg;
  int msgs_left;
  ConnInfo *conn;
  CURL *easy;
  CURLcode res;

  while ((msg = curl_multi_info_read(g->multi, &msgs_left)))
  {
    if (msg->msg == CURLMSG_DONE)
    {
	  std::map<CURL*, ConnInfo*>::iterator iter = conninfot_map.find(msg->easy_handle);
	  if (iter != conninfot_map.end())
	  {
		  if (!iter->second->retflag)
		  {
			  //ELOG(LOG_SpecialDebug,"------Delete ConnInfo-------");
			  SDKAsynHandler::GetInstance().PostToLoginFailQueue(eEC_TBInvalidToken, iter->second->gcnetid);
		  }
		  conninfot_map.erase(iter);
	  }
      easy = msg->easy_handle;
      res = msg->data.result;
      curl_easy_getinfo(easy, CURLINFO_PRIVATE, &conn);
      curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &eff_url);
      curl_multi_remove_handle(g->multi, easy);
      curl_easy_cleanup(easy);
	  delete conn;
    }
  }
}

/* Called by asio when there is an action on a socket */
static void event_cb(GlobalInfo * g, boost::asio::ip::tcp::socket * tcp_socket, int action)
{
  CURLMcode rc = curl_multi_socket_action(g->multi, tcp_socket->native_handle(), action, &g->still_running);
  mcode_or_die("event_cb: curl_multi_socket_action", rc);
  check_multi_info(g);

  if ( g->still_running <= 0 )
  {
    timer.cancel();
  }
}

/* Called by asio when our timeout expires */
static void timer_cb(const boost::system::error_code & error, GlobalInfo *g)
{
  if ( !error)
  {
    CURLMcode rc;
    rc = curl_multi_socket_action(g->multi, CURL_SOCKET_TIMEOUT, 0, &g->still_running);

    mcode_or_die("timer_cb: curl_multi_socket_action", rc);
    check_multi_info(g);
  }
}

/* Clean up any data */
static void remsock(int *f, GlobalInfo *g)
{
  if ( f )
  {
    free(f);
  }
}

static void setsock(int *fdp, curl_socket_t s, CURL*e, int act, GlobalInfo*g)
{
  std::map<curl_socket_t, boost::asio::ip::tcp::socket *>::iterator it = socket_map.find(s);

  if ( it == socket_map.end() )
  {
    return;
  }

  boost::asio::ip::tcp::socket * tcp_socket = it->second;

  *fdp = act;

  if ( act == CURL_POLL_IN )
  {
    tcp_socket->async_read_some(boost::asio::null_buffers(),
                             boost::bind(&event_cb, g,
                               tcp_socket, act));
  }
  else if ( act == CURL_POLL_OUT )
  {
    tcp_socket->async_write_some(boost::asio::null_buffers(),
                              boost::bind(&event_cb, g,
                                tcp_socket, act));
  }
  else if ( act == CURL_POLL_INOUT )
  {
    tcp_socket->async_read_some(boost::asio::null_buffers(),
                             boost::bind(&event_cb, g,
                               tcp_socket, act));

    tcp_socket->async_write_some(boost::asio::null_buffers(),
                              boost::bind(&event_cb, g,
                                tcp_socket, act));
  }
}


static void addsock(curl_socket_t s, CURL *easy, int action, GlobalInfo *g)
{
  int *fdp = (int *)calloc(sizeof(int), 1); /* fdp is used to store current action */

  setsock(fdp, s, easy, action, g);
  curl_multi_assign(g->multi, s, fdp);
}

/* CURLMOPT_SOCKETFUNCTION */
static int sock_cb(CURL *e, curl_socket_t s, int what, void *cbp, void *sockp)
{
  GlobalInfo *g = (GlobalInfo*) cbp;
  int *actionp = (int*) sockp;
  const char *whatstr[]={ "none", "IN", "OUT", "INOUT", "REMOVE"};

  if ( what == CURL_POLL_REMOVE )
  {
    remsock(actionp, g);
  }
  else
  {
    if ( !actionp )
    {
      addsock(s, e, what, g);
    }
    else
    {
      setsock(actionp, s, e, what, g);
    }
  }
  return 0;
}


/* CURLOPT_WRITEFUNCTION */
static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
  ConnInfo* pConnectInfo = (ConnInfo*)data;
  pConnectInfo->resData.append((char*)ptr, size * nmemb);
  pConnectInfo->retflag = true;
  SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg(pConnectInfo);
  return (size * nmemb);;
}

// static size_t write_cb_tb(void *ptr, size_t size, size_t nmemb, void *data)
// {
// 	ConnInfo* pConnectInfo = (ConnInfo*)data;
// 	pConnectInfo->resData.append((char*)ptr, size * nmemb);
// 	pConnectInfo->retflag = true;
// 	SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg_TB(pConnectInfo);
// 	return (size * nmemb);
// }

// static size_t write_cb_pp(void *ptr, size_t size, size_t nmemb, void *data)
// {
// 	ConnInfo* pConnectInfo = (ConnInfo*)data;
// 	pConnectInfo->resData.append((char*)ptr, size * nmemb);
// 	pConnectInfo->retflag = true;
// 	SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg_PP(pConnectInfo);
// 	return (size * nmemb);
// }
// 
// static size_t write_cb_cmge(void *ptr, size_t size, size_t nmemb, void *data)
// {
// 	ConnInfo* pConnectInfo = (ConnInfo*)data;
// 	pConnectInfo->resData.append((char*)ptr, size * nmemb);
// 	pConnectInfo->retflag = true;
// 	SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg_Cmge(pConnectInfo);
// 	return (size * nmemb);;
// }
// 
// static size_t write_cb_cmge_info(void *ptr, size_t size, size_t nmemb, void *data)
// {
// 	ConnInfo* pConnectInfo = (ConnInfo*)data;
// 	pConnectInfo->resData.append((char*)ptr, size * nmemb);
// 	pConnectInfo->retflag = true;
// 	SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg_Cmge_Info(pConnectInfo);
// 	return (size * nmemb);;
// }
// 
// static size_t write_cb_uc(void *ptr, size_t size, size_t nmemb, void *data)
// {
// 	ConnInfo* pConnectInfo = (ConnInfo*)data;
// 	pConnectInfo->resData.append((char*)ptr, size * nmemb);
// 	pConnectInfo->retflag = true;
// 	SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg_UC(pConnectInfo);
// 	return (size * nmemb);
// }
// 
// static size_t write_cb_iTools(void *ptr, size_t size, size_t nmemb, void *data)
// {
// 	ConnInfo* pConnectInfo = (ConnInfo*)data;
// 	pConnectInfo->resData.append((char*)ptr, size * nmemb);
// 	pConnectInfo->retflag = true;
// 	SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg_iTools(pConnectInfo);
// 	return (size * nmemb);
// }
// 
// static size_t write_cb_OnlineGame(void *ptr, size_t size, size_t nmemb, void *data)
// {
// 	ConnInfo* pConnectInfo = (ConnInfo*)data;
// 	pConnectInfo->resData.append((char*)ptr, size * nmemb);
// 	pConnectInfo->retflag = true;
// 	SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg_OnlineGame(pConnectInfo);
// 	return (size * nmemb);
// }
// 
// static size_t write_cb_As(void *ptr, size_t size, size_t nmemb, void *data)
// {
// 	ConnInfo* pConnectInfo = (ConnInfo*)data;
// 	pConnectInfo->resData.append((char*)ptr, size * nmemb);
// 	pConnectInfo->retflag = true;
// 	SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg_As(pConnectInfo);
// 	return (size * nmemb);
// }

/* CURLOPT_PROGRESSFUNCTION */
static int prog_cb (void *p, double dltotal, double dlnow, double ult,
                    double uln)
{
  ConnInfo *conn = (ConnInfo *)p;
  (void)ult;
  (void)uln;

  return 0;
}

/* CURLOPT_OPENSOCKETFUNCTION */
static curl_socket_t opensocket(void *clientp,
                                curlsocktype purpose,
                                struct curl_sockaddr *address)
{
  curl_socket_t sockfd = CURL_SOCKET_BAD;

  /* restrict to ipv4 */
  if (purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET)
  {
    /* create a tcp socket object */
    boost::asio::ip::tcp::socket *tcp_socket = new boost::asio::ip::tcp::socket(io_service);

    /* open it and get the native handle*/
    boost::system::error_code ec;
    tcp_socket->open(boost::asio::ip::tcp::v4(), ec);

    if (ec)
    {
      //An error occurred
      std::cout << std::endl << "Couldn't open socket [" << ec << "][" << ec.message() << "]";
    }
    else
    {
      sockfd = tcp_socket->native_handle();

      /* save it for monitoring */
      socket_map.insert(std::pair<curl_socket_t, boost::asio::ip::tcp::socket *>(sockfd, tcp_socket));
    }
  }

  return sockfd;
}

/* CURLOPT_CLOSESOCKETFUNCTION */
static int Closesocket(void *clientp, curl_socket_t item)
{
  std::map<curl_socket_t, boost::asio::ip::tcp::socket *>::iterator it = socket_map.find(item);

  if ( it != socket_map.end() )
  {
    delete it->second;
    socket_map.erase(it);
  }
 // ELOG(LOG_SpecialDebug, "Socket %d is closed", item);
  return 0;
}

/* Create a new easy handle, and add it to the global curl_multi */
static void new_conn(int gcnetid, int msgid, const string& url, GlobalInfo *g, EUserPlatform eplat, bool IfPost)
{
  ConnInfo *pConn = new ConnInfo;
  CURLMcode rc;
  pConn->error[0]='\0';
  pConn->msgid = msgid;
  pConn->gcnetid = gcnetid;
  pConn->easy = curl_easy_init();
  pConn->platformid = eplat;
  if (!pConn->easy)
  {
	ELOG(LOG_ERROR, "");
	delete pConn;
    return;
  }
  pConn->global = g;
  //ELOG(LOG_DBBUG, "AsynSendData:%s with size:%u", url.c_str(), url.size());

  if (IfPost)
  {
	  switch(eplat)
	  {
	  case ePlatformiOS_PP:
		  curl_easy_setopt(pConn->easy, CURLOPT_URL, goUrl_pp.c_str());
		  break;
	  case ePlatformiOS_CMGE:
	  case ePlatformiOS_CMGE_ZB:
	  case ePlatformAndroid_CMGE:
		  curl_easy_setopt(pConn->easy, CURLOPT_URL, goUrl_cmge_login.c_str());
		  break;
	  case ePlatformiOS_CMGEInfo:
		  curl_easy_setopt(pConn->easy, CURLOPT_URL, goUrl_cmge_info.c_str());
		  break;
	  case ePlatformAndroid_UC:
		  curl_easy_setopt(pConn->easy, CURLOPT_URL, goUrl_uc.c_str());
		  break;
	  case ePlatformiOS_XY:
		  curl_easy_setopt(pConn->easy, CURLOPT_URL, goUrl_XY.c_str());
		  break;
	  default:
		  {
			  delete pConn;
			  SDKAsynHandler::GetInstance().PostToLoginFailQueue(eEC_TBInvalidToken, gcnetid);
		  }
		  return;
	  }  
	  curl_easy_setopt(pConn->easy, CURLOPT_POST, 1);
	  curl_easy_setopt(pConn->easy, CURLOPT_POSTFIELDSIZE, url.size());
	  curl_easy_setopt(pConn->easy, CURLOPT_COPYPOSTFIELDS, url.c_str());
  }
  else{
	  curl_easy_setopt(pConn->easy, CURLOPT_URL, url.c_str());
  }

  if (IfHttps(eplat))
  {
	  curl_easy_setopt(pConn->easy, CURLOPT_SSL_VERIFYPEER, 0L);
	  curl_easy_setopt(pConn->easy, CURLOPT_SSL_VERIFYHOST, 0L);
  }

  curl_easy_setopt(pConn->easy, CURLOPT_WRITEFUNCTION, write_cb);
//   case ePlatformiOS_TB:
// 	  curl_easy_setopt(pConn->easy, CURLOPT_WRITEFUNCTION, write_cb_tb);
// 	  break;
//   case ePlatformiOS_PP:
// 	  curl_easy_setopt(pConn->easy, CURLOPT_WRITEFUNCTION, write_cb_pp);
// 	  break;
//   case ePlatformiOS_CMGE:
// 	  curl_easy_setopt(pConn->easy, CURLOPT_WRITEFUNCTION, write_cb_cmge);
// 	  break;
//   case ePlatformiOS_CMGEInfo:
// 	  curl_easy_setopt(pConn->easy, CURLOPT_WRITEFUNCTION, write_cb_cmge_info);
// 	  break;
//   case ePlatformAndroid_UC:
// 	  curl_easy_setopt(pConn->easy, CURLOPT_WRITEFUNCTION, write_cb_uc);
// 	  break;
//   case ePlatformiOS_iTools:
// 	  curl_easy_setopt(pConn->easy, CURLOPT_WRITEFUNCTION, write_cb_iTools);
// 	  break;
//   case ePlatformiOS_OnlineGame:
// 	  curl_easy_setopt(pConn->easy, CURLOPT_WRITEFUNCTION, write_cb_OnlineGame);
// 	  break;
//   case ePlatformiOS_As:
// 	  curl_easy_setopt(pConn->easy, CURLOPT_WRITEFUNCTION, write_cb_As);
// 	  break;

  curl_easy_setopt(pConn->easy, CURLOPT_WRITEDATA, pConn);
  curl_easy_setopt(pConn->easy, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(pConn->easy, CURLOPT_ERRORBUFFER, pConn->error);
  curl_easy_setopt(pConn->easy, CURLOPT_PRIVATE, pConn);
  curl_easy_setopt(pConn->easy, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(pConn->easy, CURLOPT_PROGRESSFUNCTION, prog_cb);
  curl_easy_setopt(pConn->easy, CURLOPT_PROGRESSDATA, pConn);
  curl_easy_setopt(pConn->easy, CURLOPT_LOW_SPEED_TIME, 1L);
  curl_easy_setopt(pConn->easy, CURLOPT_LOW_SPEED_LIMIT, 10L);
  curl_easy_setopt(pConn->easy, CURLOPT_TIMEOUT, 10);
  /* call this function to get a socket */
  curl_easy_setopt(pConn->easy, CURLOPT_OPENSOCKETFUNCTION, opensocket);

  /* call this function to close a socket */
  curl_easy_setopt(pConn->easy, CURLOPT_CLOSESOCKETFUNCTION, Closesocket);

  rc = curl_multi_add_handle(g->multi, pConn->easy);
  mcode_or_die("new_conn: curl_multi_add_handle", rc);
  conninfot_map[pConn->easy] = pConn;
  /* note that the add_handle() will set a time-out to trigger very soon so
     that the necessary socket_action() call will be called by this app */
}

static void newtimer_cb(const boost::system::error_code & error){
	SDKAsynHandler& pHanlder = SDKAsynHandler::GetInstance();
	while (!pHanlder.m_SDKCallbackQueue.empty()){
		Buffer* pBuffer = nullptr;
		pHanlder.m_SDKCallbackQueue.try_pop(pBuffer);
		if (pBuffer->m_LogLevel == GCToLS::eMsgToLSFromGC_AskLogin){
			int gcnetid = 0;
			pBuffer->GetInt(gcnetid);
			EUserPlatform eplat;
			pBuffer->GetInt(*(int*)&eplat);
			if (IfTestPlatform(eplat)){
				SDKAsynHandler::GetInstance().AsynHandleLoiginCheckMsg_PC(gcnetid);
			}
			else{
				bool bState = IfPostVisit(eplat);
				int strlength = 0;
				pBuffer->GetInt(strlength);
				string str;
				pBuffer->GetString(str, strlength);
				new_conn(gcnetid, pBuffer->m_LogLevel, str, &gGlobalInfoInstance, eplat, bState);
			}
		}
		pHanlder.m_SDKCallbackQueuePool.ReleaseObejct(pBuffer);
	}
	newtimer.expires_from_now(boost::posix_time::microseconds(m_CheckTime));
	newtimer.async_wait(boost::bind(newtimer_cb, _1));
}

void run()
{
	io_service.run();
}

bool IfPostVisit(EUserPlatform eplat)
{
	switch(eplat){
	case ePlatformiOS_PP:
	case ePlatformiOS_CMGE:
	case ePlatformiOS_CMGE_ZB:
	case ePlatformAndroid_CMGE:
	case ePlatformiOS_CMGEInfo:
	case ePlatformAndroid_UC:
	case ePlatformiOS_XY:
	//case ePlatformiOS_iTools:
		return true;
	default:
		return false;
	}
}

bool IfHttps(EUserPlatform eplat)
{
	switch(eplat){
	case ePlatformiOS_XY:
	case ePlatformiOS_iTools:
	case ePlatformiOS_As:
		return true;
	default:
		return false;
	}
}

bool IfTestPlatform(EUserPlatform eplat)
{
	switch(eplat){
	case ePlatform_PC:
	case ePlatformAndroid_UC:
	case ePlatformiOS_OnlineGame:
	//case ePlatformiOS_XY:
	//case ePlatformiOS_iTools:
	//case ePlatformiOS_As:
		return true;
	default:
		return false;
	}
}

void SDKAsynHandler::InitThread(){
//	CURLMcode rc;
	memset(&gGlobalInfoInstance, 0, sizeof(GlobalInfo));
	gGlobalInfoInstance.multi = curl_multi_init();

	curl_multi_setopt(gGlobalInfoInstance.multi, CURLMOPT_SOCKETFUNCTION, sock_cb);
	curl_multi_setopt(gGlobalInfoInstance.multi, CURLMOPT_SOCKETDATA, &gGlobalInfoInstance);
	curl_multi_setopt(gGlobalInfoInstance.multi, CURLMOPT_TIMERFUNCTION, multi_timer_cb);
	curl_multi_setopt(gGlobalInfoInstance.multi, CURLMOPT_TIMERDATA, &gGlobalInfoInstance);
	curl_multi_setopt(gGlobalInfoInstance.multi, CURLMOPT_MAXCONNECTS, 30);
	//curl_multi_setopt(gGlobalInfoInstance.multi, CURLMOPT_MAX_TOTAL_CONNECTIONS, 30);CURLMOPT_MAXCONNECTS

	newtimer.expires_from_now(boost::posix_time::microseconds(m_CheckTime));
	newtimer.async_wait(boost::bind(newtimer_cb, _1));
	boost::asio::io_service::work work(io_service);
	boost::thread thread1(run);
}

void SDKAsynHandler::PostMsg(const char* pMsg, int length, int msgid, int gcnetid, EUserPlatform eplat){
	Buffer* pBuffer = nullptr;
	pBuffer = m_SDKCallbackQueuePool.AcquireObject();
	
	bool res = pBuffer->append(gcnetid);
	if (!res){
		//ELOG(LOG_ERROR, "pBuffer->append error");
		PostToLoginFailQueue(eEC_PostLoginMsgFail, gcnetid);
		return;
	}

	res = pBuffer->append(eplat);
	if (!res){
		//ELOG(LOG_ERROR, "pBuffer->append error");
		PostToLoginFailQueue(eEC_PostLoginMsgFail, gcnetid);
		return;
	}

	pBuffer->append(length);
	res = pBuffer->append(pMsg, length);
	if (!res){
		//ELOG(LOG_ERROR, "pBuffer->append error");
		PostToLoginFailQueue(eEC_PostLoginMsgFail, gcnetid);
		return;
	}
	pBuffer->m_LogLevel = msgid;

	m_SDKCallbackQueue.push(pBuffer);
}

void SDKAsynHandler::AsynHandleLoiginCheckMsg(ConnInfo* pConnInfo)
{
	//将用户 uid 和 平台 结合为唯一帐号
	bool ret_flag = false;
	string account = "";
	string creator = "";
	string token = "";

	switch(pConnInfo->platformid){
	case ePlatformiOS_91:
		ret_flag = SdkConnector::GetInstance().GetResult_91(pConnInfo->resData.c_str());
		break;
	case ePlatformiOS_TB:
		ret_flag = SdkConnector::GetInstance().GetResult_TB(pConnInfo->resData.c_str());
		break;
	case ePlatformiOS_PP:
		ret_flag = SdkConnector::GetInstance().GetResult_PP(pConnInfo->resData.c_str(), account, creator);
		break;
	case ePlatformiOS_CMGE:
	case ePlatformiOS_CMGE_ZB:
	case ePlatformAndroid_CMGE:
		ret_flag = SdkConnector::GetInstance().GetResult_CMGE(pConnInfo->resData.c_str(), token);
		break;
	case ePlatformiOS_UC:
		ret_flag = SdkConnector::GetInstance().GetResult_UC(pConnInfo->resData.c_str(), account);
		break;
	case ePlatformiOS_iTools:
		ret_flag = SdkConnector::GetInstance().GetResult_iTools(pConnInfo->resData.c_str());
		break;
	case ePlatformiOS_OnlineGame:
		break;
	case ePlatformiOS_As:
		ret_flag = SdkConnector::GetInstance().GetLoginResult_As(pConnInfo->resData.c_str());
		break;
	case ePlatformiOS_XY:
		ret_flag = SdkConnector::GetInstance().GetLoginResult_XY(pConnInfo->resData.c_str());
		break;
	case ePlatformAndroid_UC:
		break;
	case ePlatformiOS_CMGEInfo:
		ret_flag = true;
		account = SdkConnector::GetInstance().GetResult_CMGE_Info(pConnInfo->resData.c_str());
		break;
	default:
		PostToLoginFailQueue(eEC_UnknowPlatform, pConnInfo->gcnetid);
		break;
	}

	if (ret_flag){
		if (ePlatformiOS_CMGE == pConnInfo->platformid || ePlatformiOS_CMGE_ZB == pConnInfo->platformid || ePlatformAndroid_CMGE == pConnInfo->platformid){
			string sSendData = SdkConnector::GetInstance().HttpSendData_Cmge_Info(token.c_str());
			PostMsg(sSendData.c_str(), sSendData.size(), pConnInfo->msgid, pConnInfo->gcnetid, ePlatformiOS_CMGEInfo);
			return;
		}

		string account_id = creator + account;
		AddUserToLoginMap(pConnInfo, (EUserPlatform)pConnInfo->platformid, account_id);
	}
	else{
		PostToLoginFailQueue(eEc_91LoginFail, pConnInfo->gcnetid);
	}
}

int SDKAsynHandler::CheckLogin(GCToLS::AskLogin& sAskLogin, int gcnetid){
	string str_uin = sAskLogin.uin();
	const char *uin = str_uin.c_str();
	const char *sessionid = sAskLogin.sessionid().c_str();
	UINT32 un32platform = sAskLogin.platform();
	INT32 retflag = eNormal;

	UserLoginData pData;
	pData.platFrom = un32platform;
	pData.sessionid = sAskLogin.sessionid();
	pData.uin = sAskLogin.uin();
	{
		boost::mutex::scoped_lock lock(m_UserLoginDataMapMutex);
		if (m_UserLoginDataMap.find(gcnetid) != m_UserLoginDataMap.end()){
			ELOG(LOG_WARNNING, "玩家(%d)多次登录！！但服务器数据还没返回数据给客户端", sAskLogin.uin());
			return eNormal;
		}
		m_UserLoginDataMap[gcnetid] = pData;
	}

	ELOG(LOG_DBBUG, "GC Try To Login with uin:%s(%u), sessionid:%s, platform:%d", uin, gcnetid, sessionid, un32platform);

	string sSendData;
	switch(un32platform)
	{
	case ePlatformiOS_91:
		sSendData = SdkConnector::GetInstance().HttpSendData_91(uin, sessionid);
		break;
	case  ePlatformiOS_TB:
		sSendData = SdkConnector::GetInstance().HttpSendData_TB(sessionid);
		break;
	case ePlatformiOS_PP:
		sSendData = SdkConnector::GetInstance().HttpSendData_PP(sessionid);
		break;
	case ePlatformiOS_CMGE:
		sSendData = SdkConnector::GetInstance().HttpSendData_Cmge_JB(sessionid);
		break;
	case ePlatformAndroid_UC:
		sSendData = SdkConnector::GetInstance().HttpSendData_UC(sessionid);
		break;
	case ePlatformiOS_iTools:
		sSendData = SdkConnector::GetInstance().HttpSendData_iTools(sessionid);
		break;
	case ePlatformiOS_OnlineGame:
		sSendData = SdkConnector::GetInstance().HttpSendData_OnlineGame(sessionid);
		break;
	case ePlatformiOS_As:
		sSendData = SdkConnector::GetInstance().HttpSendData_As(sessionid);
		break;
	case ePlatformiOS_XY:
		sSendData = SdkConnector::GetInstance().HttpSendData_XY(uin, sessionid);
		break;
	case ePlatformAndroid_CMGE:
		sSendData = SdkConnector::GetInstance().HttpSendData_Cmge_Android(sessionid);
		break;
	case ePlatform_PC:
		sSendData = "PCTest";
		break;
	default:
		PostToLoginFailQueue(eEC_UnknowPlatform, gcnetid);
		break;
	}
	ELOG(LOG_SpecialDebug, "%s", sSendData.c_str());
	PostMsg(sSendData.c_str(), sSendData.size(), sAskLogin.msgid(), gcnetid, (EUserPlatform)un32platform);
	return 0;
}

void SDKAsynHandler::InitCfg(){
	const char* filepath = "./LSConfig/LSCfg.xml";
	TiXmlDocument doc(filepath);
	bool loadOkay = doc.LoadFile();
	if (!loadOkay){
		ELOG(LOG_ERROR, "load LSCfg.xml failed for %s\n", doc.ErrorDesc());
	}
}

void SDKAsynHandler::Init(){
	InitCfg();
	InitThread();
	//m_pActive = Active::CreateActive(std::bind(&SDKAsynHandler::AsynPostHandler, this, std::placeholders::_1));
}

int SDKAsynHandler::AddUserToLoginMap(ConnInfo* pConnInfo, EUserPlatform eplat, string account)
{
	UserLoginData sUserData;
	{
		boost::mutex::scoped_lock lock(m_UserLoginDataMapMutex);
		UserLoginDataMap::iterator iter = m_UserLoginDataMap.find(pConnInfo->gcnetid);
		if (iter == m_UserLoginDataMap.end()){
			ELOG(LOG_ERROR, "");
			return FALSE;
		}
		sUserData = iter->second;
		m_UserLoginDataMap.erase(iter);
	}

	if (sUserData.uin.size() < 3){
		PostToLoginFailQueue(eEC_InvalidUserNameLegth, pConnInfo->gcnetid);
		//ELOG(LOG_WARNNING, "sUserData.uin.size() < 3");
		return FALSE;
	}

	//应当把平台的枚举值与uin整合为唯一标识存储
	LoginUserInfo sTempInfo;
	sTempInfo.plat = sUserData.platFrom;
	sTempInfo.sessionid = sUserData.sessionid;
	switch(eplat){
	case ePlatformiOS_91:
	case ePlatformiOS_TB:
	case ePlatformiOS_iTools:
	case ePlatformiOS_OnlineGame:
	case ePlatformiOS_As:
	case ePlatformiOS_XY:
		sTempInfo.uin = sUserData.uin;
		break;
	case ePlatformiOS_CMGEInfo:
	case ePlatformiOS_PP:
	case ePlatformAndroid_UC:
		sTempInfo.uin = account;
		break;
	default:
		//ELOG(LOG_WARNNING,"Unkonw platform.");
		PostToLoginFailQueue(eEC_UnknowPlatform, pConnInfo->gcnetid);
		break;
	}
	SdkConnector::GetInstance().SendToInsertData(sUserData.uin, sTempInfo, pConnInfo->gcnetid);

	return eNormal;
}

void SDKAsynHandler::AsynHandleLoiginCheckMsg_PC(int gcnetid){
	UserLoginData sUserData;
	{ 
		boost::mutex::scoped_lock lock(m_UserLoginDataMapMutex);
		UserLoginDataMap::iterator iter = m_UserLoginDataMap.find(gcnetid);
		if (iter == m_UserLoginDataMap.end()){
			//ELOG(LOG_ERROR, "");
			PostToLoginFailQueue(eEC_UserNotExist, gcnetid);
			return ;
		}
		sUserData = iter->second;
		m_UserLoginDataMap.erase(iter);
	}

	string temp = "";
	if (sUserData.platFrom == ePlatform_PC){
		stringstream ss;
		ss<<sUserData.platFrom;
		ss<<sUserData.uin;
		ss>>temp;
	}
	else{
		temp = sUserData.sessionid;
	}

	LoginUserInfo sTempInfo;
	sTempInfo.plat = sUserData.platFrom;
	sTempInfo.sessionid = temp;
	sTempInfo.uin = sUserData.uin;

	SdkConnector::GetInstance().SendToInsertData(sUserData.uin, sTempInfo, gcnetid);
}

void SDKAsynHandler::PostToLoginFailQueue(EFBAllErrorCode ErrorCode, int gcnetid)
{
	ELOG(LOG_WARNNING, "Fail with netid:%d.", gcnetid);
	SdkConnector::GetInstance().SendToFailData(ErrorCode, gcnetid);
}



