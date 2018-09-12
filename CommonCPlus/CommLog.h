////////////////////////////////////сно╥хуж╬//////////////////////////////////////
#ifndef COMM_LOG_H_
#define COMM_LOG_H_
#include "Include/FBAll_Const.h"  

const char* 	GetGameLogHeadByType(EGameLog elog)  ;

void   GetGameLogHead(EGameLog eLog,  stringstream  & mystream,INT32 param1 =0, INT64 param2 =0);

#endif