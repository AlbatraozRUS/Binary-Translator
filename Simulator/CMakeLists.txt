
cmake_minimum_required(VERSION 3.10)
project(CPU-Simulator)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)

add_library(Simulator STATIC Simulator.h Simulator.cpp)