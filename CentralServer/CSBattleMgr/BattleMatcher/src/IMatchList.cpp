#include "stdafx.h"
#include "..\IMatchList.h"
#include "..\..\CSCfgMgr.h"
using namespace CentralServer;

ICSMatchList::ICSMatchList(UINT32 mapId)
{
	mMapLogicCfg = CCSCfgMgr::getInstance().GetMapLogicCfg(mapId);
	Assert(mMapLogicCfg!=NULL);
}
ICSMatchList::~ICSMatchList()
{

}
