cmake_minimum_required(VERSION 3.22)
project(SeaBattleField)

add_subdirectory(libraries)

add_executable(main main.cpp)
target_link_libraries(main lib)