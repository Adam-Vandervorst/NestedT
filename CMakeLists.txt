# cmake_minimum_required(VERSION <specify CMake version here>)
project(ExelixisBase)

set(CMAKE_CXX_STANDARD 20)
set(GCC_COVERAGE_COMPILE_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
add_executable(ExelixisBase src/main.cpp src/N.cpp src/N.h src/NS.cpp src/NS.h src/ChainIter.h src/Rewriting.cpp src/Rewriting.h src/Nested.cpp src/Nested.h)
