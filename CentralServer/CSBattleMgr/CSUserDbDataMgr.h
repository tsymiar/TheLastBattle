
#ifndef CSUSERDBMGR_H_
#define CSUSERDBMGR_H_
#include <string>
#include <map>

namespace CentralServer{


class CCSUserDbDataMgr
{
public:
	static void UpdateUserDbData(SUserDBData & userdb,std::stringstream &strSql)  ; 
	void GetUserGuideDbData(UINT64 objId,const SUserGuideSteps &oldGuideSteps,const SUserGuideSteps &newGuideSteps,  std::string &strSql)  ;
	static void GetNewUserDbData(SUserDBData & userdb,std::string &strSql);
	//static void GetUserDbGuideStepsData(   SUserDBData& userdb ,  std::string &strSql );

	static void GetNewUserDbHerosData(UINT64	un64ObjIdx, const SUserHeroDBData &db, std::string &strSql);
	static void GetNewUserDbHerosData(UINT64	un64ObjIdx, const vector<SUserHeroDBData> &db, std::string &strSql);
	static void GetDelUserDbHerosSql(UINT64	un64ObjIdx, UINT32 commodityID, std::string &strSql);

	static void InsertOrUpdateRunesBagData(UINT64 un64ObjIdx, const string& bagSql, std::string &strSql);
	static void InsertOrUpdateRunesBagDataAndToDBThread(UINT64 un64ObjIdx, const string& bagSql);

	static void InsertOrUpdateRunesSlotData(UINT64 un64ObjIdx, const string& bagSql, std::string &strSql);
	static void InsertOrUpdateRunesSlotDataAndToDBThread(UINT64 un64ObjIdx, const string& bagSql);

	static void InsertOrUpdateAllRunesDataAndToDBThread(UINT64 un64ObjIdx, const string& bagstr, const string& slotstr); 
	 
	static void GetUpdateDbHeroData(const UINT64 userid, const UINT32 heroid, const INT64 endtime, std::string &strSql);

private:
	static void CheckStreamAndInit( std::stringstream &mystream );
};
 
}

#endif