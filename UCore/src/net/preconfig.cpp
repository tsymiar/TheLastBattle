#include "preconfig.h"

// #if SDNet_UsedIn_FrontServer
// #pragma message("[preconfig]You have defined sdnet used in a front server")
// #endif
// 
// #if SDNet_UsedIn_BackendServer
// #pragma message("[preconfig]You have defined sdnet used in a backend server")
// #endif

// #if SDNet_UsedIn_BackendServer && SDNet_UsedIn_FrontServer
// #pragma message("[preconfig]======== sdnet usage conflict!!!! ==========")
// #endif

#ifdef SDNET_HAS_GATHER_SEND 
#pragma message("[preconfig]You have defined sdnet collect buffer before send")
#endif

#if SDNet_Has_Dump_BMCmd
#pragma message("[preconfig]You have defined sdnet dump bm at recving time")
#endif

