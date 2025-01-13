cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(DLL ${CMAKE_SOURCE_DIR}/../SocketStreams)
set(BASE_TCP_SERVER_TAG v1.10.0)

if (UNIX)
	add_definitions(-D__LINUX__)
endif (UNIX)

project(Tests)

include(FetchContent)

FetchContent_Declare(
    BaseTCPServer
    GIT_REPOSITORY https://github.com/LazyPanda07/BaseTCPServer.git
    GIT_TAG        ${BASE_TCP_SERVER_TAG}
)

FetchContent_MakeAvailable(BaseTCPServer)

find_package(GTest)

if(NOT ${GTest_FOUND})
	set(GTEST_BOTH_LIBRARIES gtest gtest_main)
	set(GTEST_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/gtest/include/)
endif()

add_executable(
	${PROJECT_NAME}
	main.cpp
	server.cpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	${CMAKE_SOURCE_DIR}/../src
	${basetcpserver_SOURCE_DIR}/src
	${GTEST_INCLUDE_DIRS}
)

target_link_directories(
	${PROJECT_NAME} PUBLIC
	${CMAKE_SOURCE_DIR}/../SocketStreams/lib
	gtest/lib
)

target_link_libraries(
	${PROJECT_NAME} PUBLIC
	SocketStreams
	BaseTCPServer
	${GTEST_BOTH_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin)
