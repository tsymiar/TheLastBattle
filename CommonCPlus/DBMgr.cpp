#include "DBMgr.h" 


//namespace MysqlDWrapper
//{
	IDBConnector::IDBConnector():mysql_(NULL),myres_(NULL),/*fileds_(NULL),*/m_TotalRows_(0),m_TotalFields_(0),m_curRowIdx_(0),m_connState_(false)
{ 
}
IDBConnector::~IDBConnector()
{
	if (mysql_)
	{
		mysql_ = NULL;
	}
	if (myres_)
	{
		myres_ = NULL;
	}
}

bool IDBConnector::ConnDb(const char *cpszDBHostIP,  const char	 *cpszUserName, const char *cpszUserUserPwd, const char *cpszDBName, int  nPort ){
	Clear();
	mysql_ = mysql_init( NULL ); 
	if( mysql_ == NULL )
	{ 
		return false;
	} 
	char value = 1;
	mysql_options(mysql_, MYSQL_OPT_RECONNECT, &value);
	mysql_options(mysql_, MYSQL_SET_CHARSET_NAME, "utf8");

	bool bFlag = mysql_real_connect(mysql_,cpszDBHostIP,cpszUserName,cpszUserUserPwd,cpszDBName,nPort,NULL,CLIENT_MULTI_STATEMENTS);
	if (!bFlag){
		std::string err(GetErrorStr());	
		return false;
	} 
	strcpy(m_DbIp_,cpszDBHostIP); 
	strcpy(m_DbUser_,cpszUserName);
	strcpy(m_DbPwd_, cpszUserUserPwd); 
	strcpy(m_DbName_, cpszDBName);

	nPort_ = nPort;
	m_connState_ = true;

	return true;
}

#define  gSlectSign1 "select" 
#define  gSlectSign2 "SELECT" 
int IDBConnector::ExecQuery(const string& pData){ 
	if (pData.length() < 1)
	{
		return -1;
	}

	if (mysql_ == NULL)
	{
		if(!ReConn())
		{ 
			return -1;
		}
	}   
	ClearRes();

	//string strSql( strlwr((char*)pData.c_str()) ); 
	int  nRet =  mysql_real_query(mysql_, pData.c_str(), pData.length() );  
	if (nRet != 0)
	{
		return -1;
	}

	if ((-1 !=  pData.find(gSlectSign1) ) || (-1 !=  pData.find(gSlectSign2) ) )
	{   
		myres_ = mysql_store_result(mysql_);
		m_TotalFields_ = mysql_num_fields(myres_); 

		m_TotalRows_ = static_cast<int>(mysql_affected_rows(mysql_)); 

		GetQueryFields();  
	}  
	
	return nRet;
}

void	IDBConnector::ClearRes()
{ 
	bool ifNeedQueryNext = (myres_ != nullptr);
	if (myres_){
		mysql_free_result(myres_);  
	}

	if (ifNeedQueryNext){
		while (mysql_next_result(mysql_) == 0){
			myres_ = mysql_store_result(mysql_);
			if (myres_){
				mysql_free_result(myres_); 
			}
		}
	}
	else{
		do 
		{
			myres_ = mysql_store_result(mysql_);
			if (myres_){
				mysql_free_result(myres_); 
			}
		} while (mysql_next_result(mysql_) == 0); 
	}

	myres_ = NULL;
}

void	IDBConnector::Clear()
{ 
	m_connState_ = false;
	if (mysql_)
	{
		mysql_close(mysql_); 
		mysql_ = NULL; 
	} 

	if (myres_)
	{
		myres_ = NULL;
	}
} 

void	IDBConnector::GetQueryFieldData(const char *cpszFieldName, int &rn32Data)
{ 
	auto iter = m_FieldsValue.find(cpszFieldName);
	if (iter != m_FieldsValue.end()){
		rn32Data = (int)atoi(iter->second.data()); 
	}
}
void	IDBConnector::GetQueryFieldData(const char *cpszFieldName, unsigned int &rn32Data)
{
	auto iter = m_FieldsValue.find(cpszFieldName);
	if (iter != m_FieldsValue.end()){
		rn32Data = (int)atoi(iter->second.data()); 
	}
}
 

void	IDBConnector::GetQueryFieldData(const char *cpszFieldName, int64_t &rn64Data)
{
	auto iter = m_FieldsValue.find(cpszFieldName);
	if (iter != m_FieldsValue.end()){
		rn64Data = _atoi64(iter->second.data()); 
	}
}

void		IDBConnector::GetQueryFieldData(const char *cpszFieldName, uint64_t &run64Data)
{
	auto iter = m_FieldsValue.find(cpszFieldName);
	if (iter != m_FieldsValue.end()){
		run64Data = _atoi64(iter->second.data()); 
	}
}

void	IDBConnector::GetQueryFieldData(const char *cpszFieldName, char *  fileData)
{
	auto iter = m_FieldsValue.find(cpszFieldName);
	if (iter != m_FieldsValue.end()){
		strcpy(fileData, iter->second.data()); 
	}
}

void IDBConnector::GetQueryFieldData(const char *cpszFieldName, string& fileData){
	auto iter = m_FieldsValue.find(cpszFieldName);
	if (iter != m_FieldsValue.end()){
		fileData =  iter->second.data();
	}
}

void	IDBConnector::GetQueryFields()
{
	filedVec.clear();
	m_FieldsValue.clear();
	MYSQL_FIELD* filed = NULL;
	MYSQL_ROW curRow = mysql_fetch_row(myres_);
	if (!curRow){
		return;
	}
	int i = 0;
	while(filed = mysql_fetch_field(myres_))
	{   
		auto tempRes = curRow[i];
		if (tempRes){
			string filedStr(filed->name, filed->name_length);
			m_FieldsValue[filedStr] = tempRes;
			filedVec.push_back(filedStr);
		}

		++i;
	}
}
void	IDBConnector::QueryNext()
{ 
	m_FieldsValue.clear();
	MYSQL_ROW curRow = mysql_fetch_row(myres_);
	if (!curRow){
		return;
	}
	int size = filedVec.size();
	for (int i = 0; i < size; ++i){
		auto tempRes = curRow[i];
		if (tempRes){
			m_FieldsValue[filedVec[i]] = tempRes;
		}
	}
}
 
void	IDBConnector::CloseQuery()
{ 
	m_curRowIdx_ = 0;
	m_FieldsValue.clear();
}


 
bool	IDBConnector::ReConn()
{
	mysql_ = mysql_init(NULL); 
	if( mysql_ == NULL )
	{ 
		return false;
	}  
	char value = 1;
	mysql_options(mysql_, MYSQL_OPT_RECONNECT, &value);
	mysql_options(mysql_, MYSQL_SET_CHARSET_NAME, "utf8");  

	return mysql_real_connect(mysql_,m_DbIp_,m_DbUser_,m_DbPwd_,m_DbName_,nPort_,NULL,CLIENT_MULTI_STATEMENTS); 
}
