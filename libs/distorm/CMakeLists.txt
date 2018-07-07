cmake_minimum_required(VERSION 3.10)

project(Distorm)

set(DISTORM_SOURCES
    src/decoder.c
    src/distorm.c
    src/instructions.c
    src/insts.c
    src/mnemonics.c
    src/operands.c
    src/prefix.c
    src/textdefs.c
    src/wstring.c
    src/config.h
    src/decoder.h
    src/instructions.h
    src/insts.h
    src/operands.h
    src/prefix.h
    src/textdefs.h
    src/wstring.h
    src/x86defs.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(DISTORM_LIB STATIC ${DISTORM_SOURCES})