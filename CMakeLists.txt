cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)

if (UNIX)
	add_definitions(-D__LINUX__)
endif ()

project(SocketStreams VERSION 1.12.2)

add_library(
	${PROJECT_NAME} STATIC
	src/Network.cpp
	src/WebException.cpp
	src/SocketStreamsUtility.cpp
	src/IOSocketBuffer.cpp
	src/IOSocketStream.cpp
	src/ContainerWrapper.cpp
	src/BufferArray.cpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	include
)

if (DEFINED ENV{MARCH} AND NOT "$ENV{MARCH}" STREQUAL "")
	target_compile_options(${PROJECT_NAME} PRIVATE -march=$ENV{MARCH})
endif()	

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY include DESTINATION .)
