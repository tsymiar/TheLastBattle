// BalanceServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

bool DoUserCmd(const char* cmd);
bool LoadAllConfig();
bool Init();
void Run();
void UnInit();

CBSNetSessionMgr* gNetSessionMgr;

int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleTitle(_T("bs id=1"));

	if (!Init()) 
	{
		system("pause");
		return -1;
	}

	while(true)
	{
		if (kbhit())
		{
			static char CmdArray[1024] = {0};
			static int CmdPos = 0;
			char CmdOne = getche();
			CmdArray[CmdPos++] = CmdOne;
			bool bRet = 0;
			if (CmdPos>=1024 || CmdOne==13) { CmdArray[--CmdPos]=0; bRet = DoUserCmd(CmdArray); CmdPos=0; if (bRet) break; }
		}
		Run();
		::Sleep(1);
	}
	UnInit();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}

bool DoUserCmd(const char* cmd)
{
	printf("start parse cmd %s\n",cmd);

	if (stricmp(cmd,"exit")==0) return true;

	return false;
}

bool Init()
{
	gNetSessionMgr = new CBSNetSessionMgr;
	if (!LoadAllConfig())
	{
		printf("can't load all config.\n");
		return false;
	}
	INetSessionMgr::GetInstance()->CreateListener(gBsConfig.gs_listen_port,102400,102400,0,&gGateSessionFactory);//GS端口长连接
	INetSessionMgr::GetInstance()->CreateListener(gBsConfig.client_listen_port,102400,102400,1,&gClientSessionFactory);//GC端口短连接
	INetSessionMgr::GetInstance()->CreateConnector(ST_CLIENT_B2L,gBsConfig.ls_ip.c_str(),gBsConfig.ls_port,102400,102400,0);
	return true;
}

void UnInit()
{
	delete INetSessionMgr::GetInstance();
}

void Run()
{
	INetSessionMgr::GetInstance()->Update();
}

bool LoadAllConfig()
{
	gBsConfig.client_listen_port = GetPrivateProfileInt(_T("MainClient"),_T("ListernPortForClient"),0,_T("BSConfig\\Setup.ini"));
	char login_address[64];
	GetPrivateProfileStringA("MainLogin","LSIP","",login_address,64,"BSConfig\\Setup.ini");
	gBsConfig.ls_ip = login_address;
	gBsConfig.ls_port = GetPrivateProfileInt(_T("MainLogin"),_T("LSPort"),0,_T("BSConfig\\Setup.ini"));
	gBsConfig.gs_listen_port = GetPrivateProfileInt(_T("MainGate"),_T("ListernPortForGate"),0,_T("BSConfig\\Setup.ini"));
	gBsConfig.gs_base_index = GetPrivateProfileInt(_T("MainGate"),_T("GateBaseIndex"),0,_T("BSConfig\\Setup.ini"));
	gBsConfig.gs_max_count = GetPrivateProfileInt(_T("MainGate"),_T("GateMaxCount"),0,_T("BSConfig\\Setup.ini"));
	gBsConfig.gs_full_count = GetPrivateProfileInt(_T("MainGate"),_T("GateFullCount"),0,_T("BSConfig\\Setup.ini"));
	
	char server_key[64];
	char server_address[64];
	char server_address_ex[64];
	for (int i=1;i<=gBsConfig.gs_max_count;++i)
	{
		_snprintf(server_key, sizeof(server_key), "GateServer%u",i);
		GetPrivateProfileStringA("MainGate",server_key,"",server_address,64,"BSConfig\\Setup.ini");
		_snprintf(server_key, sizeof(server_key), "GateServer%uExport",i);
		GetPrivateProfileStringA("MainGate",server_key,"",server_address_ex,64,"BSConfig\\Setup.ini");
		gBsConfig.gs_ip_list.push_back(server_address);
		sOneGsInfo& oneGsInfo = gAllGsInfo[gBsConfig.gs_base_index+i-1];
		oneGsInfo.gs_Id = gBsConfig.gs_base_index+i-1;
		oneGsInfo.gs_nets = 0;
		oneGsInfo.gs_isLost = true;
		oneGsInfo.gs_gc_count = 0;
		//////////////////////////////////////////////////////////////////////////
		{
			char* portPos = strchr(server_address,':'); *portPos=0; ++portPos;
			oneGsInfo.gs_Ip = server_address;
			oneGsInfo.gs_Port = atoi(portPos);
		}
		//////////////////////////////////////////////////////////////////////////
		{
			char* exportPos = strchr(server_address_ex,':'); *exportPos=0; ++exportPos;
			oneGsInfo.gs_IpExport = server_address_ex;
			int exPos = atoi(exportPos);
			if (exPos>0)
				gNetSessionMgr->getnetip(oneGsInfo.gs_IpExport,exPos-1);
		}
	}
	return true;
}
