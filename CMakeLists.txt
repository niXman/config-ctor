cmake_minimum_required(VERSION 2.8)
project(config-ctor)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

#add_definitions(-DCONFIG_CTOR_HEADER_ONLY)

include_directories(
	./include
)

set(SOURCES
	./main.cpp
    ./include/config-ctor/config-ctor.hpp
    ./include/config-ctor/config-ctor.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
