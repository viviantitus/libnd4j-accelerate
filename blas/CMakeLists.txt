set( CMAKE_VERBOSE_MAKEFILE on)

message("Using apple")
link_directories(/usr/local/lib)
link_directories(/usr/lib)
link_directories(/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wl,-rpath,$ORIGIN/")


set(ARCH_TUNE "-march=${ARCH} -mtune=${ARCH}")

# using Clang
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARCH_TUNE}")


message("CPU BLAS")
add_definitions(-D__CPUBLAS__=true)


add_library(nd4jcpu SHARED cpu/NativeOps.cpp ../include/cnpy/cnpy.cpp ../include/nd4jmemset.h ../include/nd4jmalloc.h ../include/memory/cpu/Workspace.cpp)



    
