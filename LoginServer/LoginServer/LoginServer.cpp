// LoginServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SDKAsynHandler.h"
#include "SdkConnector.h"

bool DoUserCmd(const char* cmd);
bool LoadAllConfig();
bool LoadServerList();
bool Init();
void Run();
void UnInit();

CLSNetSessionMgr* gNetSessionMgr;

int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleTitle(_T("ls id=1"));

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
		SdkConnector::GetInstance().Update();
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
	gNetSessionMgr = new CLSNetSessionMgr;
	if (!LoadAllConfig())
	{
		printf("can't load all config.\n");
		return false;
	}
	if (!LoadServerList())
	{
		printf("can't load server list config.\n");
		return false;
	}
	INetSessionMgr::GetInstance()->CreateListener(gLsConfig.bs_listen_port,102400,102400,0,&gBalanceSessionFactory);//GS端口长连接
	INetSessionMgr::GetInstance()->CreateListener(gLsConfig.client_listen_port,102400,102400,1,&gClientSessionFactory);//GC端口短连接
	SDKAsynHandler::GetInstance().Init();
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
    char szPath[MAX_PATH];
    ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
    for (int i = strlen(szPath) - 1; i >= 0; --i)
    {
        if (szPath[i] == '\\')
        {
            szPath[i] = 0;
            break;
        }
    }

    std::string strExeDir = szPath;
    strExeDir += '\\';
    
    std::string str1 = strExeDir + "LSConfig\\Setup.ini";
    gLsConfig.client_listen_port = GetPrivateProfileIntA("MainGC","ListernPortForClient",0,str1.c_str());
    gLsConfig.bs_listen_port = GetPrivateProfileIntA("MainBS","ListernPortForBS",0,str1.c_str());
	gLsConfig.bs_base_index = GetPrivateProfileIntA("MainBS","BSBaseIndex",0,str1.c_str());
	gLsConfig.bs_max_count = GetPrivateProfileIntA("MainBS","BSMaxCount",0,str1.c_str());

	char server_key[64];
	char server_address[64];
	char server_address_ex[64];
	for (int i=1;i<=gLsConfig.bs_max_count;++i)
	{
		_snprintf(server_key, sizeof(server_key), "BS%u",i);
		GetPrivateProfileStringA("MainBS",server_key,"",server_address,64,str1.c_str());
		_snprintf(server_key, sizeof(server_key), "BS%uExport",i);
		GetPrivateProfileStringA("MainBS",server_key,"",server_address_ex,64,str1.c_str());
		sOneBsInfo& oneBsInfo = gAllBsInfo[gLsConfig.bs_base_index+i-1];
		oneBsInfo.bs_Id = gLsConfig.bs_base_index+i-1;
		oneBsInfo.bs_nets = 0;
		oneBsInfo.bs_isLost = true;
		//////////////////////////////////////////////////////////////////////////
		{
			char* portPos = strchr(server_address,':'); *portPos=0; ++portPos;
			oneBsInfo.bs_Ip = server_address;
			oneBsInfo.bs_Port = atoi(portPos);
		}
		//////////////////////////////////////////////////////////////////////////
		{
			char* exportPos = strchr(server_address_ex,':'); *exportPos=0; ++exportPos;
			oneBsInfo.bs_IpExport = server_address_ex;
			int exPos = atoi(exportPos);
			if (exPos>0)
				gNetSessionMgr->getnetip(oneBsInfo.bs_IpExport,exPos-1);
		}
	}
	return true;
}

bool LoadServerList()
{
    char szPath[MAX_PATH];
    ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
    for (int i = strlen(szPath) - 1; i >= 0; --i)
    {
        if (szPath[i] == '\\')
        {
            szPath[i] = 0;
            break;
        }
    }

    std::string strExeDir = szPath;
    strExeDir += "\\LSConfig\\Serverlist.ini";
    
    int listnum = GetPrivateProfileIntA("MainList","ServerNum",0,strExeDir.c_str());
	char server_name[32];
	char server_addr[32];
	char t_name[32];
	char t_addr[32];
	memset(server_name, 0, sizeof(server_name));
	memset(server_addr, 0, sizeof(server_addr));
	memset(t_name, 0, sizeof(t_name));
	memset(t_addr, 0, sizeof(t_addr));

	//UINT32 port_id; 
	for (int i = 1; i <= listnum; ++i){
		sServerAddr serveraddr;
		memset(&serveraddr, 0, sizeof(serveraddr));

		_snprintf(t_name, sizeof(t_name), "Name%u",i);
		GetPrivateProfileStringA("MainList", t_name, "", server_name, 32, strExeDir.c_str());
		_snprintf(t_addr, sizeof(t_addr), "Addr%u",i);
		GetPrivateProfileStringA("MainList", t_addr, "", server_addr, 32, strExeDir.c_str());
		char* portPos = strchr(server_addr,':'); *portPos=0; ++portPos;
		serveraddr.str_port = atoi(portPos);

		string temp_addr = server_addr;
		string temp_name = server_name;

		//先把 ascii 转为 unicode 
		std::wstring  wstr_addr = SdkConnector::Acsi2WideByte(temp_addr);
		//最后把 unicode 转为 utf8  
		string utf8_addr = SdkConnector::Unicode2Utf8(wstr_addr);

		std::wstring  wstr_name = SdkConnector::Acsi2WideByte(temp_name);
		string utf8_name = SdkConnector::Unicode2Utf8(wstr_name);

		serveraddr.str_addr = utf8_addr;
		serveraddr.str_name = utf8_name;
		gAllServerAddr[i] = serveraddr; 
	}
	return true;
}