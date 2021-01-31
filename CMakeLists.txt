cmake_minimum_required(VERSION 3.10)
project(CPU-Simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(CPU-Simulator main.cpp Assembler.cpp Assembler.h Instruction.cpp Instruction.h Simulator.h Simulator.cpp)
