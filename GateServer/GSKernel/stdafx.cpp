// stdafx.cpp : 只包括标准包含文件的源文件
// UserMgr.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include <boost/date_time/posix_time/posix_time.hpp> 
#include "boost/date_time/gregorian/gregorian.hpp"
// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

INT64 GetMiliSecond(){
	using boost::gregorian::date;
	using boost::posix_time::ptime;
	using boost::posix_time::microsec_clock;
	static ptime const epoch(date(1970, 1, 1));
	return (microsec_clock::universal_time() - epoch).total_milliseconds();
}
