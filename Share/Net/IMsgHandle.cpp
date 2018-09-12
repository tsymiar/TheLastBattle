#include "IMsgHandle.h"

IMsgHandle::IMsgHandle(int base,int max)
{
	mBase = base;
	mMax = max;
	mNodes = new MsgNode[max];
	memset(mNodes,0,sizeof(MsgNode)*max);
	mUnknownHandle = NULL;
}

IMsgHandle::~IMsgHandle()
{
	delete [] mNodes;
}

void IMsgHandle::RegisterMsgFunc(int type, msg_handle fuc_handle, bool isInitMsg/*=false*/)
{
	type -= mBase;
	if (type<0) { char* p = NULL; *p = '\0'; }
	MsgNode* pNode = mNodes+type;
	pNode->mIsInitMsg = isInitMsg;
	pNode->mHandle = fuc_handle;
}

void IMsgHandle::SetUnKnownMsgHandle(msg_handle fuc_handle)
{
	mUnknownHandle = fuc_handle;
}