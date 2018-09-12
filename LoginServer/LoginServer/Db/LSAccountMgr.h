
#ifndef _LSACCOUNT_H_
#define _LSACCOUNT_H_
#include <queue>
#include <hash_map>

#include "../../../CommonCPlus/CommonCPlus/WYYTools/DBMgr.h"
using namespace std;

class CLSAccountMgr
{
	std::hash_map<std::string, std::string  > gGlobalMap;
public:
	CLSAccountMgr();
	~CLSAccountMgr();

	static CLSAccountMgr GetInstance() 
	{
		static CLSAccountMgr oo;
		return oo;
	}

	void LoadCfg();


	void AddLoginAccount(std::string & cdkey, std::string param);


	void ConnDb();
private:
	IDBConnector * m_conn;

	SDBCfg		   m_dbCfg;	
};

#endif