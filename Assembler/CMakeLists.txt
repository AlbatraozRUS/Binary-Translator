cmake_minimum_required(VERSION 3.10)
project(CPU-Simulator)

set(CMAKE_CXX_STANDARD 17)

include_directories(../common)

add_library(Assembler STATIC Assembler.h Assembler.cpp
                             Instruction.h Instruction.cpp)