cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 17)

project(SocketStreams)

add_library(
	${PROJECT_NAME} STATIC
	src/WebException.cpp
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin)
