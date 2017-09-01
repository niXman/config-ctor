
cmake_minimum_required(VERSION 2.8)
project(config-ctor)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

include_directories(
	./include
)

set(HEADERS
	config-ctor/config-ctor.hpp
)

set(SOURCES
	./main.cpp
	./include/config-ctor/config-ctor.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
