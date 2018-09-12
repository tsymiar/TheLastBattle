PROJECT(LuaPlus)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE_DIRECTORIES( /usr/include/curl )
INCLUDE_DIRECTORIES( ../Include/LuaPlus )
INCLUDE_DIRECTORIES( ../Include/LuaPlus/src )
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
AUX_SOURCE_DIRECTORY(./src DIR_SUBSRCS)

LINK_DIRECTORIES( $ENV{SolutionRoot}/Bin/$ENV{Platform}/$ENV{Configuration} )	###lib搜索路径
LINK_DIRECTORIES( $ENV{SolutionRoot}/Lib/$ENV{Platform}/$ENV{Configuration} )	###lib搜索路径

SET(LIBRARY_OUTPUT_PATH $ENV{SolutionRoot}/Bin/$ENV{Platform}/$ENV{Configuration})
SET(EXECUTABLE_OUTPUT_PATH $ENV{SolutionRoot}/Bin/$ENV{Platform}/$ENV{Configuration})	##编译完成后输出路径
IF ($ENV{Configuration} STREQUAL Debug)
	SET(CMAKE_BUILD_TYPE Debug)
ELSE()
	SET(CMAKE_BUILD_TYPE Release)
ENDIF()

#set compile options.
IF( WIN32 )
	IF ($ENV{Configuration} STREQUAL Debug)
		add_definitions(-DDEBUG)		##debug版本
	ENDIF()
ELSE()
	IF ($ENV{Configuration} STREQUAL Debug)
		add_definitions(-g)		##debug版本
		SET(CMAKE_CXX_FLAGS_DEBU "$ENV{CXXFLAGS} -O0 -Wall")##编译Debug
	ELSE()
		SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")##编译release
	ENDIF()
ENDIF()

#generate lib.
IF(WIN32)
    add_library( LuaPlus STATIC ${DIR_SRCS} ${DIR_SUBSRCS})
ELSE()
    add_library( LuaPlus STATIC ${DIR_SRCS} ${DIR_SUBSRCS})
ENDIF()