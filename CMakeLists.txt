cmake_minimum_required(VERSION 3.10)
project(NieRMouseFix)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(distorm)

include_directories(distorm/include)

add_library(mouse_patch SHARED patch.cpp)
target_link_libraries(mouse_patch DISTORM_LIB)