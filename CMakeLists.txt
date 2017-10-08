cmake_minimum_required(VERSION 2.8)
project( libnd4j )

set(CMAKE_CXX_FLAGS_RELEASE  "-Wall -O3 -std=c++11 -fassociative-math -funsafe-math-optimizations -Wno-narrowing")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -O0 -std=c++11 -Wno-narrowing")


include_directories(blas/ include/ include/helpers include/loops include/graph include/ops include/types include/cnpy)

add_subdirectory(blas)
add_subdirectory(tests)

include(CPack)