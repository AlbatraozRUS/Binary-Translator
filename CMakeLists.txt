cmake_minimum_required(VERSION 3.10)
project(CPU-Simulator)

set(CMAKE_CXX_STANDARD 17)

include_directories(Assembler Simulator)

add_executable(CPU_Simulator main.cpp)
add_subdirectory(Assembler) 
add_subdirectory(Simulator) 

target_link_libraries(CPU_Simulator Assembler Simulator "-lm")