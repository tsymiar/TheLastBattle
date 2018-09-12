SET(ProjectName ELogging)
PROJECT(${ProjectName})
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
INCLUDE_DIRECTORIES( ../../../CommonCPlus/CommonCPlus)
INCLUDE_DIRECTORIES( ../../../CommonCPlus/CommonCPlus/Boost)
INCLUDE_DIRECTORIES(..)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

add_definitions(-std=c++11)

SET(ENV{Platform} x64)
SET(ENV{Configuration} Debug)
SET(CompilerSetting "clang")
SET(LIBRARY_OUTPUT_PATH ${SERVER_SOURCE_DIR}/Lib/$ENV{Platform}/$ENV{Configuration})
SET(EXECUTABLE_OUTPUT_PATH	${SERVER_SOURCE_DIR}/Bin/$ENV{Platform}/$ENV{Configuration})
SET(LINK_LIBPATH ${SERVER_SOURCE_DIR}/Lib/$ENV{Platform}/$ENV{Configuration})

IF ($ENV{Configuration} STREQUAL Debug)
	SET(CMAKE_BUILD_TYPE Debug)
ELSE()
	SET(CMAKE_BUILD_TYPE Release)
ENDIF()

LINK_DIRECTORIES(${LINK_LIBPATH})

IF ($ENV{Configuration} STREQUAL Debug)
	add_definitions(-g -lpthread)
	SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -ggdb3")
ELSE()
	SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
ENDIF()

ADD_LIBRARY(${ProjectName} SHARED ${DIR_SRCS})
