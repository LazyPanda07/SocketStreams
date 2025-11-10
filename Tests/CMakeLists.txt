cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(DLL ${CMAKE_SOURCE_DIR}/../SocketStreams)
set(BASE_TCP_SERVER_VERSION 1.15.0)
set(GTEST_VERSION 1.17.0)

project(Tests)

if (UNIX)
	add_definitions(-D__LINUX__)
endif(UNIX)

include(FetchContent)

FetchContent_Declare(
    BaseTCPServer
    GIT_REPOSITORY https://github.com/LazyPanda07/BaseTCPServer.git
    GIT_TAG v${BASE_TCP_SERVER_VERSION}
)
FetchContent_Declare(
	gtest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG v${GTEST_VERSION}
)

FetchContent_MakeAvailable(BaseTCPServer)
FetchContent_MakeAvailable(gtest)

add_executable(
	${PROJECT_NAME}
	main.cpp
	server.cpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	${CMAKE_SOURCE_DIR}/../include
	${basetcpserver_SOURCE_DIR}/src
)

target_link_directories(
	${PROJECT_NAME} PUBLIC
	${CMAKE_SOURCE_DIR}/../SocketStreams/lib
)

target_link_libraries(
	${PROJECT_NAME} PUBLIC
	SocketStreams
	BaseTCPServer
	gtest
	gtest_main
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin)
