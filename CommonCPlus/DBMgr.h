#pragma once 

#include <map> 
#include <string>
#include <stdint.h>
#include <windows.h>  
#include "MySql/mysql.h"
#include "Include/FBAll_Const.h"
#include <string>
using namespace std;

//namespace MysqlDWrapper{

//enum	EDBFieldType
//{
//	eFieldType_None = 0,
//	eFieldType_INT8,
//	eFieldType_UINT8,
//	eFieldType_INT16,
//	eFieldType_UINT16,
//	eFieldType_INT32,
//	eFieldType_UINT32,
//	eFieldType_INT64,
//	eFieldType_UINT64,
//	eFieldType_FLOAT,
//	eFieldType_DOUBLE,
//	eFieldType_String,
//	eFieldType_Blob,
//};



const	int		valueLen = 256;
const	int		arrayLen = 32;

struct tagTableInfo 
{ 
	char columName[arrayLen];
	char valueType[arrayLen];
	char valueContent[valueLen];

	tagTableInfo(){memset(this,0,sizeof(*this));}
	~tagTableInfo(){memset(this,0,sizeof(*this));}
};

class IDBConnector
{ 
public:
		IDBConnector();
		~IDBConnector(); 
		 
		int						ExecQuery(const string& pData);
		 
		void					GetQueryFieldData(const char *cpszFieldName, int &rn32Data);
		void					GetQueryFieldData(const char *cpszFieldName, unsigned int &rn32Data);
		 
		void					GetQueryFieldData(const char *cpszFieldName, int64_t &rn64Data);
		void					GetQueryFieldData(const char *cpszFieldName, uint64_t &run64Data); 

		void					GetQueryFieldData(const char *cpszFieldName, char *  fileData);
		void					GetQueryFieldData(const char *cpszFieldName, string&  fileData);
		void					GetQueryFields();   
		void					QueryNext();   
		void					CloseQuery();  
public:
		bool					ConnDb(const char *cpszDBHostIP,  const char	 *cpszUserName, const char *cpszUserUserPwd, const char *cpszDBName, int  nPort = 3306); 
 
		void					Clear();
		bool					ReConn();
		void					ClearRes();


		int						GetErrorCode()				{	return mysql_errno( mysql_ );}
		const char*				GetErrorStr()				{	return mysql_error( mysql_ ); }

		int						GetQueryRowNum()			{	return m_TotalRows_;	}
		int						GetQueryFieldNum()			{	return m_TotalFields_;	}

		MYSQL *					GetMySql()					{	return mysql_;}
		MYSQL_RES	*			GetMyRes()					{	return myres_;}
private:
	MYSQL * mysql_; 
	MYSQL_RES	*myres_;
	int		m_TotalRows_;
	int       m_TotalFields_;
	int		m_curRowIdx_;
	vector<string> filedVec;

	char m_DbIp_[arrayLen];
	char m_DbUser_[arrayLen];
	char m_DbPwd_[arrayLen];
	char m_DbName_[arrayLen];  
	int  nPort_; 
	bool		m_connState_;

	std::map<string, std::string>  m_FieldsValue;// columValue
};
 
//}
