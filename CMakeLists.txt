cmake_minimum_required(VERSION 3.10)
project(NieRMouseFix)

set(CMAKE_CXX_STANDARD 17)

add_library(mouse_patch SHARED patch.cpp)