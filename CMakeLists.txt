cmake_minimum_required(VERSION 3.10)
project(NieRMouseFix)

set(CMAKE_CXX_STANDARD 11)

add_library(mouse_patch SHARED mouse_patch.c)