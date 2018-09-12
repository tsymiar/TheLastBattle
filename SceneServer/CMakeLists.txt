PROJECT(SSServer)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(Boost)
include_directories(Include)
include_directories(Include/SSBattleMgr)
include_directories(Include/SSKernel)
include_directories(Include/SSUserMgr)

add_subdirectory(SSKernel)
add_subdirectory(SSUserMgr)
add_subdirectory(SSBattleMgr)
add_subdirectory(SSConsole)