
cmake_minimum_required(VERSION 3.10)
project(CPU-Simulator)

set(CMAKE_CXX_STANDARD 17)

add_library(Simulator STATIC Simulator.h Simulator.cpp)

target_include_directories(Simulator PUBLIC ../common)