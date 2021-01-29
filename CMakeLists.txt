cmake_minimum_required(VERSION 3.10)
project(CPU-Simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(CPU-Simulator CPU_Simulator.cpp CPU_Simulator.h Instruction_Parser.cpp Instruction_Parser.h)
