cmake_minimum_required(VERSION 3.10)
project(CPU-Simulator)

set(CMAKE_CXX_STANDARD 17)

include_directories(Assembler Simulator Translator)

# SET(GCC_COMPILE_FLAGS "-g -Wall")
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")

add_executable(Binary_Translator main.cpp)
add_subdirectory(Assembler)
add_subdirectory(Simulator)
add_subdirectory(Translator)

target_link_libraries(Binary_Translator Assembler Simulator Translator "-lm")