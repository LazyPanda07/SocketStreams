cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install-dir CACHE STRING "")

if (UNIX)
	add_definitions(-D__LINUX__)
endif ()

project(SocketStreams)

add_library(
	${PROJECT_NAME} STATIC
	src/Network.cpp
	src/WebException.cpp
	src/SocketStreamsUtility.cpp
	src/IOSocketBuffer.cpp
	src/IOSocketStream.cpp
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
	install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")
endif ()
