
#ifndef CSGAME_LOG_H_
#define CSGAME_LOG_H_
#include "FBAll_Const.h"
#include <vector>
#include <string>
using namespace std;  

class CSSGameLogMgr
{  
	void AddGameLog(EGameLog elog, std::string & log);
public:
	CSSGameLogMgr();
	~CSSGameLogMgr(); 

	static CSSGameLogMgr& GetInstance() {static CSSGameLogMgr oo; return oo;} 

	void	AddGameLog(EGameLog elog, const	SUserDBData& pdb  );   
	
	void	AddGameLog(EGameLog eLog, std::string& cdkey  , GCToCS::CurtUIEvent& pMsg); 

	void	AddGameLog(EGameLog eLog, const std::string& cdkey , INT32 param,INT32 param2=0,INT32 param3=0 );  
	 
	void	AddGameLog(EGameLog eLog, const std::string& cdkey ,GCToCS::ComposeRunes& sMsg,INT32  idx);

	void	AddGameLog(EGameLog eLog, INT64 param, const std::string& logStr);

	void	AddGameLog(EGameLog eLog, INT64 param,INT64 param2,INT64 param3 = 0);
};

#endif
