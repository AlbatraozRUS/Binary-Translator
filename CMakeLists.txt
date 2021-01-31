cmake_minimum_required(VERSION 3.10)
project(CPU-Simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(CPU-Simulator main.cpp Assembler/Assembler.cpp Assembler/Assembler.h Assembler/Instruction.cpp Assembler/Instruction.h Simulator/Simulator.h Simulator/Simulator.cpp)
