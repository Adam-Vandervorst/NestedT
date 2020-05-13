project(NestedImpl)

set(CMAKE_CXX_STANDARD 20) # 17 works if you replace the set.contains
set(GCC_COVERAGE_COMPILE_FLAGS "-O3") # ~10X speedup
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
add_executable(NestedImpl src/main.cpp src/N.cpp src/N.h src/NS.cpp src/NS.h src/ChainIter.h src/Rewriting.cpp src/Rewriting.h src/Nested.cpp src/Nested.h)
