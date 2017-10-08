include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(../../include ../../layers ../../include/helpers ../../include/memory ../../include/loops ../../include/graph ../../include/ops ../../include/types ../../include/cnpy ../../blas)

    message("Using apple")
    link_directories(/usr/local/lib)
    link_directories(/usr/lib)
    link_directories(/lib)


set(CMAKE_CXX_FLAGS  " -Wall -O3 -std=c++11 -fassociative-math -funsafe-math-optimizations -Werror-pointer-arith")



    # using Clang
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARCH_TUNE}")



message("CPU BLAS")
add_definitions(-D__CPUBLAS__=true)
add_library(nd4jcpu_test SHARED ../../blas/cpu/NativeOps.cpp ../../include/cnpy/cnpy.cpp  ../../include/nd4jmemset.h ../../include/nd4jmalloc.h ../../include/ops/declarable/generic/parity_ops.h ../../include/helpers/helper_hash.h ../../include/memory/cpu/Workspace.cpp ../../include/ops/declarable/generic/helpers/convolutions.h ../../include/graph/ShapeList.h ../../include/ops/declarable/generic/convo/convo_ops.h ../../include/helpers/ArrayUtils.h ../../include/indexing/NDIndex.h ../../include/indexing/IndicesList.h ../../include/graph/SessionLocalStorage.h ../../include/graph/Stash.h )




    message("OPENMP NOT FOUND")



add_executable(runtests ../../include/memory/cpu/Workspace.cpp AllTests.cpp StashTests.h SessionLocalTests.h)
target_link_libraries(runtests nd4jcpu_test  gtest gtest_main)
