cmake_minimum_required(VERSION 3.22)
project(library)

set(SOURCES 
    SeaBattleField.cpp
)

set(HEADERS 
    SeaBattleField.h
)

add_library(lib SHARED ${SOURCES} ${HEADERS})
target_include_directories(lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})