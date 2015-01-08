
cmake_minimum_required(VERSION 2.8)
project(config-ctor)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
	./include
)

set(HEADERS
	config-ctor/config-ctor.hpp
)

set(SOURCES
	./main.cpp
)

add_executable(config-ctor ${SOURCES})

install(TARGETS config-ctor RUNTIME DESTINATION bin)
