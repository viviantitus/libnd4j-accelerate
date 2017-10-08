/*
 * shape.h
 *
 *  Created on: Dec 28, 2015
 *      Author: agibsonccc
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include <cstring>
#include <cstdio>
#include "../dll.h"
#include "../nd4jmalloc.h"
#include "../templatemath.h"
#include "../helpers/logger.h"
#include "../pointercast.h"
#include "../cnpy/cnpy.h"

#define MAX_DIMENSION 0x7fffffff
#define MAX_NUM_THREADS  1024
#define MAX_RANK 32
#define MAX_COORD 3
#define PREALLOC_SIZE 33554432
#ifdef __CUDACC__
#include <cuda.h>
#include <cuda_runtime.h>
#include <helpers/sharedmem.h>
#endif


#ifdef __CUDACC__
#define INLINEDEF inline
#else
#define INLINEDEF inline
#endif

#include "../pairwise_util.h"
#include "../../blas/NativeOps.h"
namespace shape {

/**
 * Shape information approximating
 * the information on an ndarray
 */
    struct ShapeInformation {
#ifdef __CUDACC__
        __host__ __device__
#endif
        ShapeInformation(int *shape_ = nullptr, int *stride_ = nullptr, char order_ = 0, int rank_ = 0, int offset_ = 0, int elementWiseStride_ = 0)
                : shape(shape_), stride(stride_), order(order_), rank(rank_), offset(offset_), elementWiseStride(elementWiseStride_)
        {}

        int *shape;
        int *stride;
        char order;
        int rank;
        int offset;
        int elementWiseStride;
    };

/**
 * Indexing information
 * for bounds checking
 */
    struct CurrentIndexing {
        int numElementsPerThread;
        int blockStartingIndex;
        int startingThreadIndex;
        int endingThreadIndex;

    };



#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool shapeEquals(int shape1Rank,int *shape1,int shape2Rank,int *shape2);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool shapeEquals(int *shapeInfo1,int *shapeInfo2);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool strideEquals(int shape1Rank,int *shape1,int shape2Rank,int *shape2);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool strideEquals(int *shapeInfo1,int *shapeInfo2);
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool strideEquals(int *stride1,int rank1,int *stride2,int rank2);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool equalsSoft(int *shapeA, int *shapeB);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool equalsStrict(int *shapeA, int *shapeB);

#ifdef __CUDACC__
    __host__ __device__
    INLINEDEF void traceNew(int id) {
        //printf("new happened: [%i]\n", id);
    }
#else
    INLINEDEF void traceNew(int id) {
        //printf("new happened: [%i]\n", id);
        //fflush(stdout);
    }
#endif

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tadIndexForLinear(int linearIndex, int tadLength);

#ifdef __CUDACC__
    __host__
#endif
    INLINEDEF bool canReshape(const int oldRank, int* oldShape, const int newRank, int* newShape, bool isFOrder);

#ifdef __CUDACC__
    __host__
#endif
    INLINEDEF bool reshapeCF(const int oldRank, int* oldShape, const int newRank, int* newShape, bool isFOrder, int* target);
/**
 * Get the shape info buffer
 * for the given rank and shape.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBuffer(int rank, int *shape);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBuffer(int rank, int *shape, int *buffer);

    /**
 * Get the shape info buffer
 * for the given rank and shape.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferFortran(int rank, int *shape);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferFortran(int rank, int *shape, int *output);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void doPermuteShapeBuffer(int *shapeBuffer,int *rearrange, int *tmpBuffer);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void doPermuteShapeBuffer(int rank,int *shapeBuffer,int *rearrange, int *tmpBuffer);

#ifdef __CUDACC__
    template <typename T>
    __device__ INLINEDEF int *cuMalloc(int *buffer, long size, UnifiedSharedMemory *manager);


    __device__ INLINEDEF int *cuMalloc(int *buffer, long size);
#endif



/**
 * Computes the standard packed array strides for a given shape.
 *
 * @param shape    the shape of a matrix:
 * @param startNum the start number for the strides
 * @return the strides for a matrix of n dimensions
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * calcStridesFortran(int *shape, int rank);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * calcStridesFortran(int *shape, int rank, int* ret);

/**
 * Computes the standard packed array strides for a given shape.
 *
 * @param shape    the shape of a matrix:
 * @param startNum the start number for the strides
 * @return the strides for a matrix of n dimensions
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* calcStrides(int *shape, int rank);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* calcStrides(int *shape, int rank, int* ret);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void updateStrides(int *shape, const char order);


/**
 * Computes the standard packed array strides for a given shape.
 *
 * @param shape    the shape of a matrix:
 * @param startNum the start number for the strides
 * @return the strides for a matrix of n dimensions
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* calcStridesFortran(int *shape, int rank, int startNum);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* calcStridesFortran(int *shape, int rank, int startNum, int* ret);

/**
 * Computes the standard packed array strides for a given shape.
 *
 * @param shape    the shape of a matrix:
 * @param startNum the start number for the strides
 * @return the strides for a matrix of n dimensions
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* calcStrides(int *shape, int rank, int startNum);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* calcStrides(int *shape, int rank, int startNum, int* ret);

/**
 * @param toCopy the shape to copy
 * @return a copy of the original struct
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF ShapeInformation *shapeCopy( ShapeInformation *toCopy);


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool strideDescendingCAscendingF(int *shapeBuffer);

/**
 * Compute the element wise stride
 * for a given shape/stride configuration
 * @param rank the rank of the shape/stride
 * @param shape the shape
 * @param stride the stride
 * @param isFOrder 0 or 1 for whether the array is f
 * ordered or not
 * @return -1 if there is no element wise stride the
 * element wise stride of reshape(1,length) otherwise
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int computeElementWiseStride(int rank, int *shape, int *stride, int isFOrder);

/**
 * Compute the element wise stride
 * for a given shape/stride configuration
 * @param rank the rank of the shape/stride
 * @param shape the shape
 * @param stride the stride
 * @param isFOrder 0 or 1 for whether the array is f
 * ordered or not
 * @return -1 if there is no element wise stride the
 * element wise stride of reshape(1,length) otherwise
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int computeElementWiseStride(int rank, int *shape, int *stride, int isFOrder,
                                           int *dimension, int dimensionLength);
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeInfoOnlyShapeAndStride(int *shapeInfo, int *dimension, int dimensionLength,bool reverseCopyStride);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeInfoOnlyShapeAndStride(int *shapeInfo, int *dimension, int dimensionLength,bool reverseCopyStride, int *buffer);
/**
 *
 * @param length
 * @param shape
 * @param rearrange
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *doPermuteSwap(int length, int *shape, int *rearrange);



/**
 * In place permute swap
 * @param length
 * @param shape
 * @param rearrange
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void doPermuteSwap(int length, int **shape, int *rearrange);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *permuteShapeBuffer(int *shapeBuffer,int *rearrange);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void permuteShapeBufferInPlace(int *shapeBuffer,int *rearrange,int *out);


#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void doPermuteShapeBuffer(int *shapeBuffer,int *rearrange);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void doPermuteShapeBuffer(int rank,int *shapeBuffer,int *rearrange);
    /**
     * Rearrange the permute indexes
     * according to which  dimensions are specified.
     *
     * For example, dimension is implicitly:
     * 0,1,2
     *
     * If you want to do a reduce along dimensions 0 and 1,
     * you need to permute the indexes to be:
     * 2,0,1
     *
     * which will give us the ability to ierate along an element
     * wise stride.
     */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *createPermuteIndexes(int originalRank,int *dimension,int dimensionLength);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *computeResultShape(int *originalShapeBuffer,int *dimension,int dimensionLength);


/**
 * Get the ordering for the device
 * @param length
 * @param shape
 * @param stride
 * @param elementStride
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF char getOrder(int length, int *shape, int *stride, int elementStride);

/**
 * Ensure that every value in the re arrange
 * array is unique
 * @param arr
 * @param shape
 * @param arrLength
 * @param shapeLength
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int checkArrangeArray(int *arr, int arrLength, int shapeLength);

/**
 * Permute the shape information
 * @param info the shape information to permute
 * @param rearrange the order to re arrange
 * @param rank the rank of the rearrange array
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void permute(ShapeInformation **info, int *rearrange, int rank);

/**
 * Returns whether the
 * given shape is a vector or not
 * @param shape the shape of the array
 * @param rank the rank of cthe shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isVector(int *shape, int rank);


    /**
     * When 1 dimension is the whole length of the
     * array
     */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int oneDimEqualToLength(int *shape, int rank);
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int oneDimEqualToLength(int *shapeInfo);
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isVector(int *shapeInfo);
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF bool isRowVector(int *shapeInfo);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF bool isColumnVector(int *shapeInfo);
    /**
 * Returns whether the
 * given shape is a vector or not
 * @param shape the shape of the array
 * @param rank the rank of the shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isMatrix(int *shape, int rank);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isMatrix(int *shapeInfo);
/**
 * Returns the shape portion of an information
 * buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int *shapeOf(int *buffer);

/**
 * Return a copy of a buffer.
 * This buffer allocates memory
 * that must be freed elsewhere.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int *copyOf(int length, int *toCopy);


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *copyOf(int length, int *toCopy, int *ret);

    /**
 * Return a copy of a buffer.
 * This buffer allocates memory
 * that must be freed elsewhere.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void copyTo(int length, int *from, int *to);
    /**
* Return a copy of a buffer.
* This buffer allocates memory
* that must be freed elsewhere.
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void copyTo(int length, int *from, int *to, int *indexes);

/**
 * Permute the given strides
 * in the given rearrange order
 * @param toPermute the buffer to permute
 * @param shapeRank the length of the buffer to permute
 * @param rearrange the rearrange order (must be 0 based indexes
 * and all must be filled in)
 * @return the rearranged array
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *permutedStrides(int *toPermute, int shapeRank, int *rearrange);

/**
 * Return the slice (shape + 1 in pointer arithmetic)
 * @param shape the shape to take the slice of
 * @return the shape array - the first entry
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *slice(int *shape);
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int slices(int *shapeBuffer);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *sliceOfShapeBuffer(int sliceIdx,int *shapeBuffer);
/**
 * Returns the length of the
 * shape information buffer:
 * rank * 2 + 3
 * @param rank the rank to get the shape
 * info length for
 * @return rank * 2 + 4
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int shapeInfoLength(int rank);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int shapeInfoLength(int* shapeInfo);


#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF size_t shapeInfoByteLength(int rank);


#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF size_t shapeInfoByteLength(int* shapeInfo);

/**
 * Returns the rank portion of
 * an information buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int rank( int *buffer);

/**
 * Converts a raw int buffer of the layout:
 * rank
 * shape
 * stride
 * offset
 * elementWiseStride
 *
 * where shape and stride are both straight int pointers
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    ShapeInformation *infoFromBuffer(int *buffer);

/**
 * Returns the stride portion of an information
 * buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int *stride(int *buffer);

/**
 * Compute the length of the given shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    Nd4jIndex length(int *shapeInfo);

#ifdef __CUDACC__
    __host__ __device__
#endif
    Nd4jIndex length(std::initializer_list<int>& shape);

/***
 * Returns the offset portion of an information buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int offset(int *buffer);

/**
 * Returns the ordering
 * for this shape information buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF char order(int *buffer);

/**
 * Returns the element wise stride for this information
 * buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int elementWiseStride(int *buffer);


    /**
 * Returns the element wise stride for this information
 * buffer
     * relative to a dimension and ordering for a reduction index
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int reductionIndexElementWiseStride(int *buffer, int *dimension, int dimensionLength);

/**
 * Returns whether
 * the given shape info buffer
 * represents a scalar shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int isScalar(int *info);

/**
 * Returns whether
 * the given shape information
 * represents a scalar
 * shape or not
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isScalar(volatile ShapeInformation *info);

/**
 * Return a copy of this array with the
 * given index omitted
 *
 * @param data  the data to copy
 * @param indexes the index of the item to remove
 * @param dataLength the length of the data array
 * @param indexesLength the length of the data array
 * @return the new array with the omitted
 *
 * item
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void removeIndex(int *data, int *indexes, int dataLength, int indexesLength,
                               int *out);

    /**
 * Return a copy of this array with the
 * given index omitted
 *
 * @param data  the data to copy
 * @param indexes the index of the item to remove
 * @param dataLength the length of the data array
 * @param indexesLength the length of the data array
 * @return the new array with the omitted
 *
 * item
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int * removeIndex(int *data, int *indexes, int dataLength, int indexesLength);

    /**
     * Iterate over a given set of indexes
     * the begin and end indexes are 0 based.
     * 1 padding is automatically assumed for the ending.
     *
     * For example if you want to iterate over 0 to 4
     * it will go to 4 rather than 3.
     *
     * indexes should be the indexes to exclude
     * indexes length should be the length of indexes
     */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* everyIndexBut(int *indexes,int indexesLength,int begin,int end);

/**
 * Computes the offset for accessing
 * a global element given the shape information
 * and the offset to be read.
 */
#ifdef __CUDACC__
    __device__
#endif
    INLINEDEF int tadOffset(shape::ShapeInformation *xInfo, int offset);

/**
 * Returns a shape
 * forces the given length to be 2.
 * @param shape the shape to modify
 * @param dimension the dimension (row or column)
 * for the shape to be returned as
 * @return the new shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int* ensureVectorShape(int *shape);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* createScalarShapeInfo();

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* createScalarShapeInfo(int *ret);

/**
 * Generate an int buffer
 * up to the given length
 * at the specified increment
 *
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *range(int from, int to, int increment);

/**
 * Range between from and two with an
 * increment of 1
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *range(int from, int to);

/**
 * Keep the given indexes
 * in the data
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *keep(volatile int *data, int *index, int indexLength, int dataLength);

/**
 * Generate reverse copy of the data
 * @param data
 * @param length
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *reverseCopy(int *data, int length);
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void reverseCopyTo(int *from, int *to, int length);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void reverseCopyTo(int *from, int *to, int *indexes,int length);
/**
 *
 * @param arr1
 * @param arr1Length
 * @param arr2
 * @param arr2Length
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *concat(int *arr1, int arr1Length, int *arr2, int arr2Length);

/**
 *
 * @param numArrays
 * @param numTotalElements
 * @param arr
 * @param lengths
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int *concat(int numArrays, int numTotalElements, int **arr, int *lengths);

/**
 * Get the length per slice of the
 * given shape and the dimension
 * @param rank the rank of the shape
 * @param shape the shape of to get
 * the length per slice for
 * @param dimension the dimension to
 * get the length per slice for
 * @param dimensionLength the length of the dimension array
 * @return the length per slice of the given shape
 * along the given dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int lengthPerSlice(int rank, int *shape, int *dimension, int dimensionLength);

/**
 * calculates the offset for a tensor
 * @param index
 * @param arr
 * @param tensorShape
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int sliceOffsetForTensor(int rank,
                                       int index,
                                       int *shape,
                                       int *tensorShape,
                                       int tensorShapeLength,
                                       int *dimension,
                                       int dimensionLength);

/**
 * calculates the offset for a tensor
 * @param index
 * @param arr
 * @param tensorShape
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int sliceOffsetForTensor(int index,int tensorLength,int lengthPerSlice2);
/**
 * Computes the tensor along dimension
 * offset
 * @param index the index to get the offset for the tad for
 * @param rank the rank of the shapes and strides
 * @param info the shape information to use for tad
 * @param dimension the dimensions to use for computing the tensor along dimensions
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int offset(int index,
                         int rank,
                         shape::ShapeInformation *info,
                         int *dimension,
                         int dimensionLength);


/**
 * Computes the number
 * of tensors along
 * a given dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tensorsAlongDimension(int rank,
                                        volatile int length,
                                        volatile int *shape,
                                        int *dimension,
                                        int dimensionLength);

/**
 * Computes the number
 * of tensors along
 * a given dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tensorsAlongDimension(int *shapeInfo, int *dimension, int dimensionLength);



/**
 * Returns the tensor along dimension
 * for the given block index
 * @param blockSize
 * @param blockIdx
 * @param i
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tadForBlockIndex(int blockSize, int blockIdx, int i);

/**
 * Computes the number of tads per block
 *
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tadsPerBlock(int blockSize, int tads);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *tadShapeInfo(int index, int *xShapeInfo, int *dimension,
                                int dimensionLength);

/**
 * Returns a shape buffer
 * for the shape information metadata.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *toShapeBuffer( ShapeInformation *info);

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *toShapeBuffer( ShapeInformation *info, int* ret);

/**
 * Returns the number of elements per thread
 */
#ifdef __CUDACC__
    __device__
#endif
    int numElementsPerThread(int N);

/**
 * Returns the block starting index
 */
#ifdef __CUDACC__
    __device__
#endif
    int blockStartingIndex(int N);

/**
 * Returns the thread starting index
 */
#ifdef __CUDACC__
    __device__
#endif
    int threadStartingIndex(int N, int stride, int offset);

/**
 * Returns the thread ending index
 */
#ifdef __CUDACC__
    __device__
#endif
    int threadEndingIndex(int N, int stride, int offset);

/**
 * Returns indexing information
 * for the current kernel invocation
 */
#ifdef __CUDACC__
    __device__
#endif
    CurrentIndexing *currentIndex(int N, int offset, int stride);

/** Given an linear index, element wise stride
 * and the length of each tad
 * map a linear index to a tad
 * @param i the index to map
 * @param the element wise stride for the tads
 * @param numElementsPerTad the number of elements
 * per tad
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    int tadIndex(int i, int elementWiseStride, int numElementsPerTad);

/**
 * Map a tad to a
 * reduction index.
 * @param tadIndexForOriginal the original tad index for the
 * split up problem (eg: split is dimension 3 mapping to a 2,3 problem)
 * @param tadsForReduced the number of tads for the shrunk down problem (eg: 2,3)
 * @param tadsForOriginal the number of tads for the smaller problem (eg: 3)
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int reductionIndexForTad(int tadIndexForOriginal, int tadsForReduced,
                             int tadsForOriginal);

/**
 * Computes the number of tads
 * per reduce index for the
 * reduction tad.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int tadsPerReduceIndex(int tadsForReduce, int tadsForOriginal);

/**
 * Maps a linear index to a reduction index
 * @param i the linear index to map
 * @param elementWiseStride the element wise stride
 * for the multiple problem
 * @param tadNum the number of tads for the shrunken problem
 * @param originalTadNum the tad number for the reduced version of the problem
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int reductionIndexForLinear(int i, int elementWiseStride, int numElementsPerTad,
                                int tadNum, int originalTadNum);

/**
 * Returns the prod of the data
 * up to the given length
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    int prod(int *data, int length);
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF Nd4jIndex prodLong( int *data, int length);

    /**
     * Returns the rear most left over item not present in
     * the dimension array. This assumes that the dimension array is sorted.
     *
     * For example, given a dimension array of:
     * 0,2
     *
     * and
     *
     * 12,4,2,1 in data
     *
     * You end up with 1 (data[3])
     * since the first item won't match
     * the last item of the dimension array
     */
#ifdef __CUDACC__
    __host__ __device__
#endif
    int rearMostLeftOverItem(int *data,int length,int *dimension,int dimensionLength);

    /**
* Get an offset for retrieval
* from a data buffer
* based on the given
* shape stride and given indices
* @param baseOffset the offset to start from
* @param shape the shape of the array
* @param stride the stride of the array
* @param indices the indices to iterate over
* @return the double at the specified index
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF Nd4jIndex getOffset(Nd4jIndex baseOffset,  int *shape,  int *stride,  int *indices,int rank);
#ifdef __CUDACC__
    __host__ __device__
#endif
    int* createShapeInfo(int *shape, int *stride, int rank);

#ifdef __CUDACC__
    __host__ __device__
#endif
    int* createShapeInfo(int *shape, int *stride, int rank, int *buffer);

    /**
 * Convert a linear index to
 * the equivalent nd index
 * @param shape the shape of the dimensions
 * @param index the index to map
 * @param numIndices the number of total indices (typically prod of shape(
 * @return the mapped indexes along each dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* ind2sub(int rank,  int *shape,int index,int numIndices);


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *ind2sub(int rank,  int *shape,int index);

    /**
     * Convert a linear index to
     * the equivalent nd index
     * @param shape the shape of the dimensions
     * @param index the index to map
     * @param numIndices the number of total indices (typically prod of shape(
     * @return the mapped indexes along each dimension
     */
#ifdef __CUDACC__
    __host__ __device__
#endif
    void  ind2sub(int rank,int *shape,int index,int numIndices,int *out);

/**
     * Convert a linear index to
     * the equivalent nd index.
     * Infers the number of indices from the specified shape.
     *
     * @param shape the shape of the dimensions
     * @param index the index to map
     * @return the mapped indexes along each dimension
     */
#ifdef __CUDACC__
    __host__ __device__
#endif
    void ind2sub(int rank, int *shape, int index, int *out);

    /**
  * Convert a linear index to
  * the equivalent nd index
  * @param shape the shape of the dimensions
  * @param index the index to map
  * @param numIndices the number of total indices (typically prod of shape(
  * @return the mapped indexes along each dimension
  */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* ind2subC(int rank, int *shape, int index);
    /**
  * Convert a linear index to
  * the equivalent nd index
  * @param shape the shape of the dimensions
  * @param index the index to map
  * @param numIndices the number of total indices (typically prod of shape(
  * @return the mapped indexes along each dimension
  */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* ind2subC(int rank, int *shape, int index, int numIndices);

    /**
   * Convert a linear index to
   * the equivalent nd index
   * @param shape the shape of the dimensions
   * @param index the index to map
   * @param numIndices the number of total indices (typically prod of shape(
   * @return the mapped indexes along each dimension
   */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void  ind2subC(int rank, int *shape, int index, int numIndices, int *out);

/**
     * Convert a linear index to
     * the equivalent nd index.
     * Infers the number of indices from the specified shape.
     *
     * @param shape the shape of the dimensions
     * @param index the index to map
     * @return the mapped indexes along each dimension
     */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void ind2subC(int rank, int *shape, int index, int *out);

    /**
  * Convert the given index (such as 1,1)
  * to a linear index
  * @param shape the shape of the indexes to convert
  * @param indices the index to convert
  * @return the linear index given the shape
  * and indices
  */
#ifdef __CUDACC__
    __host__ __device__
#endif
    int sub2Ind(int rank, int *shape, int *indices);

    /**
   * Compute the real linear indices for the given shape and stride
   */
#ifdef __CUDACC__
    __host__ __device__
#endif
    Nd4jIndex *computeIndices(int rank,  int *shape,  int *stride);

    /**
   * Compute the real linear indices for the
     * given shape buffer. Shape,stride and rank are derived
     * from the buffer
   */
#ifdef __CUDACC__
    __host__ __device__
#endif
    Nd4jIndex *computeIndices( int *shapeBuffer);

    /**
 * Convert a linear index to
 * the equivalent nd index
 * @param shape the shape of the dimensions
 * @param index the index to map
 * @param numIndices the number of total indices (typically prod of shape(
 * @return the mapped indexes along each dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    void  ind2subOrder(int *shapeInfo,int index,int numIndices,int *out);

    /**
 * Convert a linear index to
 * the equivalent nd index
 * @param shape the shape of the dimensions
 * @param index the index to map
 * @param numIndices the number of total indices (typically prod of shape(
 * @return the mapped indexes along each dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    void  ind2subOrder(int *shapeInfo,int index,int *out);


#ifdef __CUDACC__
    __host__ __device__
#endif
    void printShapeInfo(int *shapeInfo);

#ifdef __CUDACC__
    __host__ __device__
#endif
    void printShapeInfoLinear(int *shapeInfo);

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void printIntArray(int *arr,int length);

#ifdef __CUDACC__
    __host__ __device__
#endif
    void printArray(float *arr,int length);


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferOfNpy(int rank, unsigned int *shape,bool fortranOrder);


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferOfNpy(cnpy::NpyArray arr);




#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferOfNpyBuffer(char *buffer);

//END HEADERS


    //BEGIN IMPLEMENTATIONS

    /**
     * Dimension collapse is an algorithm
     * for collapsing singular dimensions.
     * This algorithm will adjust the dimensions
     * wrt the original.
     *
     * The algorithm has 3 components:
     * trailing ones
     * middle ones
     * beginning ones
     *
     * dimensions that are specified to reduce along
     * that are singular should be truncated
     *
     * dimensions that are specified that are singular
     * at the beginning should be removed with middle dimensions
     * decremented.
     *
     * For any time there is a no op, a collapse will
     * set the first dimension to be -1.
     *
     *
     */
    class TAD {
    public:
        int tadIndex = 0;
        int dimensionLength;
        int *dimension = nullptr;
        int *shapeInfo = nullptr;
        int *tadOnlyShapeInfo = nullptr;
        int numTads = 0;
        int tadRank = 0;
        int *tadShape = nullptr;
        int *tadStride = nullptr;
        Nd4jIndex *tadOffsets = nullptr;
        int tadOffsetForBlock = 0;
        int rank = 0;
        int numOnes = 0;
        //pointers to original
        int originalDimensionLength;
        int *originalDimension = nullptr;
        int *originalShapeInfo = nullptr;
        bool squeezed = false;
        bool newSqueezeDimensions = false;
        int numOnesInMiddle = 0;
        bool wholeThing = false;
        //need to track whether we create a new dimension array or not, we could have just moved the pointer forward
        //due to leading ones
        bool createdNewDimension = false;

        // special case for CUDA, we're passing in __shared__ memory pointers to be used instead of new/malloc
        void *ptrManager = nullptr;
        int *ptrOutput = nullptr;
#ifdef __CUDACC__
        __host__ __device__
#endif
        TAD() {}

#ifdef __CUDACC__
        __host__ __device__
#endif
        TAD(int tadIndex,int *shapeInfo,int *dimension,int dimensionLength) {
            this->tadIndex = tadIndex;
            this->init(shapeInfo, dimension, dimensionLength);
        }

#ifdef __CUDACC__
        __host__ __device__
#endif
        TAD(int *shapeInfo,int *dimension,int dimensionLength) {
            this->init(shapeInfo, dimension, dimensionLength);
        }

#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF void setExternalBuffers(void *ptrManager) {
            this->ptrManager = ptrManager;
        }

#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF void setOutputBuffer(int *ptrOutput) {
            this->ptrOutput = ptrOutput;
        }

#ifdef __CUDACC__
        __host__ __device__
#endif
        /**
         * This method is for GPU mostly, it allows to initialize TAD instance with precalculated tadOnlyShapeInfo
         */
        INLINEDEF void initWithExternalTAD(int *existingTAD, int *originalShape, int *dimension, int dimensionLength) {
            this->tadOnlyShapeInfo = existingTAD;
            this->rank = shape::rank(originalShape);

            this->originalShapeInfo = originalShape;
            this->originalDimension = dimension;
            this->originalDimensionLength = dimensionLength;

            this->shapeInfo = originalShape;
            this->dimension = dimension;
            this->dimensionLength = dimensionLength;

            this->tadShape = shape::shapeOf(existingTAD);
            this->tadStride = shape::stride(existingTAD);

            int ews = shape::elementWiseStride(originalShape);

            this->numTads = shape::length(originalShape) / shape::length(existingTAD); // this->tensorsAlongDimension(this->shapeInfo, this->dimension, this->dimensionLength);//shape::length(originalShape) / shape::length(existingTAD);
            this->wholeThing = this->numTads == 1 || ((this->dimensionLength == this->rank || this->numTads == shape::length(this->shapeInfo)) && ews == 1);
        }


#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF void init(int *shapeInfo,int *dimension,int dimensionLength) {
            this->originalShapeInfo = shapeInfo;
            this->originalDimension = dimension;
            this->originalDimensionLength = dimensionLength;
            //start off as original references
            this->shapeInfo = shapeInfo;
            this->dimensionLength = dimensionLength;
            this->dimension = dimension;
            this->rank = shape::rank(shapeInfo);
            this->numTads = this->tensorsAlongDimension(this->shapeInfo, this->dimension, this->dimensionLength);

            int ews = shape::elementWiseStride(shapeInfo);

            if(!shape::isVector(shapeInfo))
                wholeThing = this->numTads == 1 || ((this->dimensionLength == this->rank || this->numTads == shape::length(shapeInfo)) && ews == 1);
            else if(shape::isScalar(shapeInfo))
                wholeThing = true;
                //vector case
            else {
                if(dimension == 0 && shape::shapeOf(shapeInfo)[dimension[0]] == 1) {
                    wholeThing = true;
                }
            }

        }

        template <typename T>

#ifdef __CUDACC__
        __host__ __device__
#endif
        void printTADsND(T *x) {
            if(wholeThing) {
                for(int i = 0; i < shape::length(tadOnlyShapeInfo); i++) {
                    printf(" %f ",x[i]);
                }
                printf("\n");
            }
            else {
                for (int i = 0; i <  numTads; i++) {
                    int offset = tadOffsets[i];
                    int shapeIter[MAX_RANK];
                    int coord[MAX_RANK];
                    int dim;
                    int rankIter = shape::rank(tadOnlyShapeInfo);
                    int xStridesIter[MAX_RANK];
                    T *xPointer = x + offset;
                    if (PrepareOneRawArrayIter<T>(rankIter,
                                                  shape::shapeOf(tadOnlyShapeInfo),
                                                  xPointer,
                                                  shape::stride(tadOnlyShapeInfo),
                                                  &rankIter,
                                                  shapeIter,
                                                  &xPointer,
                                                  xStridesIter) >= 0) {
                        ND4J_RAW_ITER_START(dim, shape::rank(tadOnlyShapeInfo), coord, shapeIter); {
                                /* Process the innermost dimension */
                                printf(" %f ",xPointer[0]);
                            }
                        ND4J_RAW_ITER_ONE_NEXT(dim,
                                               rankIter,
                                               coord,
                                               shapeIter,
                                               xPointer,
                                               xStridesIter);
                        printf("\n");

                    }
                    else {
                        printf("Unable to prepare array\n");
                    }
                }
            }

        }


#ifdef __CUDACC__
        __host__ __device__
#endif

        INLINEDEF void permuteShapeBufferInPlace(int *shapeBuffer,int *rearrange,int *out) {
            memcpy(out,shapeBuffer,sizeof(int) * shape::shapeInfoLength(this->rank));
            doPermuteShapeBuffer(this->rank,out,rearrange);
        }

#ifdef __CUDACC__
        __host__ __device__
#endif

        INLINEDEF int *permuteShapeBuffer(int *shapeBuffer,int *rearrange) {
            int len = shape::shapeInfoLength(this->rank);
            int *copy = shape::copyOf(len,shapeBuffer);
            doPermuteShapeBuffer(rank,copy,rearrange);
            return copy;
        }


#ifdef __CUDACC__
        __host__ __device__
#endif
        void createTadOnlyShapeInfo() {
            this->tadOnlyShapeInfo = this->shapeInfoOnlyShapeAndStride();

            if (this->tadShape != nullptr)
                delete[] this->tadShape;

            this->tadShape = shape::shapeOf(this->tadOnlyShapeInfo);
            this->tadStride = shape::stride(this->tadOnlyShapeInfo);
            /* if(tadIndex > 0) {
                 this->createOffsets();
                 this->tadOnlyShapeInfo[shape::shapeInfoLength(shape::rank(this->tadOnlyShapeInfo)) - 3] = this->tadOffsets[tadIndex];
             }*/
        }


#ifdef __CUDACC__
        __host__ __device__
#endif
        int lengthPerSlice(int *shapeBuffer) {
            int dimension = 0;
            int *remove = shape::removeIndex(shape::shapeOf(shapeBuffer),&dimension,shape::rank(shapeBuffer),1);
            int prod = shape::prod(remove,shape::rank(shapeBuffer) - 1);
            delete[] remove;
            return prod;
        }


#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF int * tad2Sub(int index) {
            int *shape = shape::shapeOf(shapeInfo);
            int rank = shape::rank(shapeInfo);
            int leftOverIndexLen = rank - originalDimensionLength;
#ifdef __CUDACC__
            int *ret;
        int *tadShape;
        int *leftOverIndexes;
        int *sub;
        if (ptrManager != nullptr) {
            UnifiedSharedMemory *manager = (UnifiedSharedMemory *) ptrManager;
            ret = manager->getTempRankBuffer1();
            tadShape = manager->getTempRankBuffer2();
            leftOverIndexes = manager->getTempRankBuffer3();
            sub = manager->getTempRankBuffer4();
        } else {
            ret = new int[rank];
            tadShape = new int[leftOverIndexLen];
            leftOverIndexes = new int[leftOverIndexLen];
            sub = new int[rank];
        }
#else
            int *ret = new int[rank];
            //shape of the tad
            int *tadShape = new int[leftOverIndexLen];
            int *leftOverIndexes = new int[leftOverIndexLen];
            int *sub = new int[rank];
#endif

            //indexes not specified in the tad indexes

            //every coordinate starts as zero
            memset(ret,0,sizeof(int) * rank);

            //find the length of the elements we
            //are iterating over
            int len = 1;
            //left over index cursor for initializing elements
            int leftOverIndex = 0;
            for(int i = 0; i < rank; i++) {
                //look for dimensions NOT found in dimension length (basically compute shape - dimension (set difference)
                bool found = false;
                for(int j = 0; j < originalDimensionLength; j++) {
                    //skip over specified dimensions when computing left over length
                    if(i == originalDimension[j]) {
                        found = true;
                        break;
                    }

                }

                //add to the indexes that aren't specified as part of the tad dimension
                //indexes
                if(!found) {
                    //accumulate the list of indexes left over used for initializing the return value
                    leftOverIndexes[leftOverIndex] = i;
                    //accumulate the tad shape
                    tadShape[leftOverIndex] = shape[i];
                    //accumulate the length (product) of the indexes that will be iterated over
                    len *= shape[i];
                    leftOverIndex++;

                }
            }


            //sub for indices
            /* int *sub = new int[leftOverIndexLen];
             shape::ind2subOrder(tadShape,index,len,sub);
            */
            shape::ind2subC(leftOverIndexLen,tadShape,index,len, sub);


            for(int i = 0; i < leftOverIndexLen; i++) {
                ret[leftOverIndexes[i]] = sub[i];
            }

            if (ptrManager == nullptr) {
                delete[] tadShape;
                delete[] leftOverIndexes;
                delete[] sub;
            }

            return  ret;

        }


#ifdef __CUDACC__
        __host__ __device__
#endif
        ~TAD() {
            //we may have just moved the pointer forward, we may not need to delete the pointer here
            if(originalDimension != this->dimension && createdNewDimension) {
                delete[] this->dimension;
            }
            if(this->originalShapeInfo != this->shapeInfo) {
                delete[] this->shapeInfo;
            }
            if(this->tadOffsets != nullptr) {
                delete[] this->tadOffsets;
            }

            if(this->tadOnlyShapeInfo != nullptr && this->tadOnlyShapeInfo != shapeInfo) {
                delete[] this->tadOnlyShapeInfo;
            }

        }


#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF  int* permuteDims() {
            //permute dimensions for tad
            int dimIdx = 0;
            //loop backwards assuming dimension is sorted

            int *permuteDims = new int[shape::rank(shapeInfo)];

            for(int i = 0; i < shape::rank(shapeInfo); i++) {
                bool found = false;
                for(int j = 0; j < originalDimensionLength; j++) {
                    if(i == originalDimension[j]) {
                        found = true;
                        break;
                    }


                }

                //not found, append it to the end for permute
                if(!found)
                    permuteDims[dimIdx++] = i;
            }



            for(int i = originalDimensionLength - 1; i >= 0; i--) {
                permuteDims[dimIdx++] = originalDimension[i];
            }

/*
            for (int i = 0; i < originalDimensionLength; i++) {
                permuteDims[i] = originalDimension[i];
            }
*/

            //permute dimensions for tad
            return permuteDims;

        }

        /**
        * Compute the tad offset given a dimension.
        *
        * The general pattern for computing a tad offset is as follows:
        * Every $STRIDE that was removed (the first dimension)
        * do a jump by the major stride of the parent array
        * (stride[0] of the parent array)
        *
        * For example given a c ordered 2,2,3,2 with stride 12,6,2,1
        * A tad of dimension 1 will jump 12 every 6 tads.
        *
        * You then end up with offsets of:
        * 0
        * 1
        * 2
        * 3
        * 4
        * 5
        * 12
        * 13
        * 14
        * 15
        * 16
        * 17
        *
        * notice there are 12 tads here. This same incremental jump will happen
        * every time.
        * Note here that by default the
        * stride of element wise stride is used for the hops.
        *
        * Sometimes a jump doesn't happen. If there are less tads
        * than the stride of the dimension you removed, the
        * element wise stride will always be used.
        *
        * For example in a dimension of 0,1, you end up with offsets of:
        * 0,1,2,3,4,5
        *
        * Given that the inner most stride of the dimensions that was removed (1)
        * had a stride of 6, we never need to do a major stride jump.
        *
        */
#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF Nd4jIndex tadOffset(int index) {
            if(tadOnlyShapeInfo == nullptr) {
                this->createTadOnlyShapeInfo();
            }

            if(wholeThing)
                return index;

            if(dimensionLength > 1) {
                int *tad2Sub = this->tad2Sub(index,ptrManager);

                Nd4jIndex ret = shape::getOffset(0,shape::shapeOf(shapeInfo),shape::stride(shapeInfo),tad2Sub,shape::rank(shapeInfo));

                if(ret < 0) {
                    if (ptrManager == nullptr)
                        delete[] tad2Sub;
                    return -1;
                }
                if (ptrManager == nullptr)
                    delete[] tad2Sub;

                return ret;

            }
            else {
                int *tad2Sub = this->tad2Sub(index,ptrManager);

                Nd4jIndex ret = shape::getOffset(0,shape::shapeOf(shapeInfo),shape::stride(shapeInfo),tad2Sub,shape::rank(shapeInfo));

                if (ptrManager == nullptr)
                    delete[] tad2Sub;

                return ret;
            }



        }

#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF int *tensorShape() {
            if(this->tadShape != nullptr)
                return this->tadShape;
            int *theShape = shape::shapeOf(shapeInfo);
            int *tensorShape = shape::keep(theShape,dimension,dimensionLength,shape::rank(shapeInfo));
            this->tadShape = tensorShape;
            this->tadRank = dimensionLength;
            return tensorShape;
        }
#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF int * tad2Sub(int index, void *ptrManager) {
            int *shape = shape::shapeOf(shapeInfo);
            int rank = shape::rank(shapeInfo);
            int leftOverIndexLen = rank - originalDimensionLength;
            int *tadShape;
            int *leftOverIndexes;
            int *sub;
            int *ret;

#ifdef __CUDACC__

            if (ptrManager != nullptr) {
                UnifiedSharedMemory *manager = (UnifiedSharedMemory *) ptrManager;
                ret = manager->getTempRankBuffer1();
                tadShape = manager->getTempRankBuffer2();
                leftOverIndexes = manager->getTempRankBuffer3();
                sub = manager->getTempRankBuffer4();
            } else {
                ret = new int[rank];
                //shape of the tad
                leftOverIndexes = new int[leftOverIndexLen];
                sub = new int[rank];
                tadShape = new int[leftOverIndexLen];
            }
#else
            ret = new int[rank];
            //shape of the tad
            leftOverIndexes = new int[leftOverIndexLen];
            sub = new int[rank];
            tadShape = new int[leftOverIndexLen];
#endif

            //indexes not specified in the tad indexes

            //every coordinate starts as zero
            memset(ret,0,sizeof(int) * rank);


            //find the length of the elements we
            //are iterating over
            int len = 1;
            //left over index cursor for initializing elements
            int leftOverIndex = 0;
            for(int i = 0; i < rank; i++) {
                //look for dimensions NOT found in dimension length (basically compute shape - dimension (set difference)
                bool found = false;
                for(int j = 0; j < originalDimensionLength; j++) {
                    //skip over specified dimensions when computing left over length
                    if(i == originalDimension[j])  {
                        found = true;
                        break;
                    }

                }

                //add to the indexes that aren't specified as part of the tad dimension
                //indexes
                if(!found) {
                    //accumulate the list of indexes left over used for initializing the return value
                    leftOverIndexes[leftOverIndex] = i;
                    //accumulate the tad shape
                    tadShape[leftOverIndex] = shape[i];
                    //accumulate the length (product) of the indexes that will be iterated over
                    leftOverIndex++;
                    len *= shape[i];

                }
            }


            //sub for indices
            /* int *sub = new int[leftOverIndexLen];
             shape::ind2subOrder(tadShape,index,len,sub);
            */
            shape::ind2subC(leftOverIndexLen,tadShape,index,len, sub);

            for(int i = 0; i < leftOverIndexLen; i++) {
                ret[leftOverIndexes[i]] = sub[i];
            }

            if (ptrManager == nullptr) {
                delete[] leftOverIndexes;
                delete[] tadShape;
                delete[] sub;
            }

            return  ret;

        }

#ifdef __CUDACC__
        __host__ __device__
#endif
        void createOffsets() {
            this->tadOffsets = new Nd4jIndex[this->numTads];
#pragma omp parallel for schedule(guided) proc_bind(close) default(shared)
            for(int i = 0; i < this->numTads; i++) {
                this->tadOffsets[i] = this->tadOffset(i);

            }
        }

#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF int *shapeInfoOnlyShapeAndStride() {
            if(wholeThing && dimensionLength == 1 && dimension[0] == MAX_DIMENSION) {
                return shape::createScalarShapeInfo();
            }
            //ensure tad shapes get setup right for vectors
            if(dimensionLength < 1 && !shape::isVector(shapeInfo)) {
                return shape::copyOf(shape::shapeInfoLength(shape::rank(shapeInfo)),shapeInfo);
            }

            int *theShape = shape::shapeOf(shapeInfo);
            int rank = shape::rank(shapeInfo);

            if(dimensionLength == 1) {
                if(dimension[0] == 0 && shape::isVector(shapeInfo) && theShape[1] == 1) {
                    int permuted[2] = {1,0};
                    int *permutedRet2 = shape::permuteShapeBuffer(shapeInfo,permuted);
                    return permutedRet2;
                } else if(dimension[0] == 1 && shape::isVector(shapeInfo) && theShape[0] == 1) {
                    return shape::copyOf(shape::shapeInfoLength(shape::rank(shapeInfo)),shapeInfo);
                }
                else if(shape::shapeOf(shapeInfo)[dimension[0]] == 1) {
                    int *scalarInfo = shape::createScalarShapeInfo();
                    scalarInfo[shape::shapeInfoLength(shape::rank(scalarInfo)) - 3] = this->tadIndex;
                    return scalarInfo;
                }
            }

            int *tensorShape = this->tensorShape();
            int *reverseDimensions = shape::reverseCopy(dimension,dimensionLength);
            int *rankRange = shape::range(0,rank);
            int *remove  = shape::removeIndex(rankRange,dimension,rank,dimensionLength);
            //concat is wrong here with the length
            int *newPermuteDims = shape::concat(remove,rank - dimensionLength,reverseDimensions,dimensionLength);
            int *permuted = shape::permuteShapeBuffer(shapeInfo,newPermuteDims);


            int sliceIndex = shape::sliceOffsetForTensor(shape::rank(permuted),
                                                         this->tadIndex,
                                                         shape::shapeOf(shapeInfo),
                                                         tensorShape,
                                                         dimensionLength,
                                                         dimension,
                                                         dimensionLength);



            int *ret2 = shape::sliceOfShapeBuffer(sliceIndex,permuted);
            int tensorLength = shape::prod(tensorShape,tadRank);

            int compLength = shape::isVector(ret2) ? shape::length(ret2)  : shape::prod(tensorShape,tadRank);
            if(dimensionLength == tadRank && compLength == shape::length(ret2)) {
                if(dimensionLength == 1 && shape::isVector(ret2) && shape::shapeOf(ret2)[0] == 1) {
                    //go to the bottom and return ret2 after proper freeing of pointers
                    //basic idea; we *don't* permute row vectors
                }
                else if(dimensionLength > 1) {
                    //permute *then* return ret2
                    int *finalPermuteDims = new int[shape::rank(ret2)];
                    int forward = 0;
                    for(int i = shape::rank(ret2) - 1; i >= 0; i--) {
                        finalPermuteDims[forward++] = i;
                    }
                    shape::permuteShapeBufferInPlace(ret2,finalPermuteDims,ret2);
                    delete[] finalPermuteDims;

                }

            }
            else {
                int length = tensorLength;
                int lengthPerSlice = this->lengthPerSlice(ret2);
                int offset = tadIndex * tensorLength /lengthPerSlice;
                if(sliceIndex == 0 && length == lengthPerSlice) {
                    int *newRet2 = shape::sliceOfShapeBuffer(offset,ret2);
                    delete[] ret2;
                    ret2 = newRet2;
                    int *finalPermuteDims = new int[shape::rank(ret2)];
                    int forward = 0;
                    for(int i = shape::rank(ret2) - 1; i >= 0; i--) {
                        finalPermuteDims[forward++] = i;
                    }
                    bool isRowVector2 = shape::isRowVector(ret2);
                    if(isRowVector2 == false) {
                        shape::permuteShapeBufferInPlace(ret2, finalPermuteDims, ret2);
                    }

                    delete[] finalPermuteDims;

                }
                else if(length == lengthPerSlice) {
                    offset -= shape::slices(ret2) * (offset / shape::slices(ret2));
                    int *newRet2 = shape::sliceOfShapeBuffer(offset,ret2);
                    delete[] ret2;
                    ret2 = newRet2;
                    if(dimensionLength == 1 && shape::isVector(ret2) && shape::shapeOf(ret2)[0] == 1) {
                        //go to the bottom and return ret2 after proper freeing of pointers
                        //basic idea; we *don't* permute row vectors
                    }
                    else {
                        int *finalPermuteDims = new int[shape::rank(ret2)];
                        int forward = 0;
                        for(int i = shape::rank(ret2) - 1; i >= 0; i--) {
                            finalPermuteDims[forward++] = i;
                        }
                        int *newRet = shape::permuteShapeBuffer(ret2,finalPermuteDims);
                        delete[] ret2;
                        delete[] finalPermuteDims;
                        ret2 = newRet;

                    }

                }
                else {
                    //execute final part, note that this is mainly so delete[] gets called
                    //at the bottom of the method
                    while(shape::length(ret2) > length) {
                        int lengthPerSlice2 = this->lengthPerSlice(ret2);
                        sliceIndex =    sliceOffsetForTensor(sliceIndex,shape::length(ret2),lengthPerSlice2);
                        sliceIndex -= shape::slices(ret2) * (sliceIndex / shape::slices(ret2));
                        int *newRet2 = shape::sliceOfShapeBuffer(sliceIndex,ret2);
                        delete[] ret2;
                        ret2 = newRet2;
                    }

                    //don't permute on a row vector
                    if(dimensionLength == 1 &&  shape::isVector(ret2) && shape::shapeOf(ret2)[0] == 1) {
                        //go to the bottom and return ret2 after proper freeing of pointers
                        //basic idea; we *don't* permute row vectors
                    }
                    else if(dimensionLength > 1){
                        //permute *then* return ret
                        int *finalPermuteDims = new int[shape::rank(ret2)];
                        int forward = 0;
                        for(int i = shape::rank(ret2) - 1; i >= 0; i--) {
                            finalPermuteDims[forward++] = i;
                        }
                        int *newPermute = shape::permuteShapeBuffer(ret2,finalPermuteDims);
                        delete[] ret2;
                        delete[] finalPermuteDims;
                        ret2 = newPermute;
                    }

                }
            }


            delete[] permuted;
            delete[] newPermuteDims;
            delete[] rankRange;
            delete[] remove;
            delete[] reverseDimensions;
            return ret2;
        }




        /**
       * Length of a tad given
       * the shape information
       */
#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF int tadLength(int *shapeInfo, int *dimension, int dimensionLength) {
            if(dimensionLength == 1) {
                return shape::shapeOf(shapeInfo)[dimension[0]];
            }
            else {
                int ret = 1;
                for(int i = 0; i < shape::rank(shapeInfo); i++) {
                    for(int j = 0; j < dimensionLength; j++) {
                        if(i == dimension[j])
                            ret *= shape::shapeOf(shapeInfo)[dimension[j]];
                    }
                }
                return ret;
            }
        }

/**
 * Computes the number
 * of tensors along
 * a given dimension
 */
#ifdef __CUDACC__
        __host__ __device__
#endif

        INLINEDEF int tensorsAlongDimension(int *shapeInfo, int *dimension, int dimensionLength) {
            return shape::length(shapeInfo) / this->tadLength(shapeInfo,dimension,dimensionLength);
        }

#ifdef __CUDACC__
        __host__ __device__
    INLINEDEF void createOffsetForBlock(int blockIdx) {
        this->tadOffsetForBlock = this->tadOffset(blockIdx);
    }
#endif


#ifdef __CUDACC__
        __host__ __device__
#endif
        INLINEDEF void collapse() {
            int *shape = shape::shapeOf(shapeInfo);
            //handle negative dimensions/backwards indexing
            for(int i = 0; i < dimensionLength; i++) {
                if((dimension)[i] < 0)
                    (dimension)[i] += shape::rank(this->shapeInfo);
            }

            this->dimension =  new int[dimensionLength];
            memcpy(this->dimension,this->originalDimension,sizeof(int) * dimensionLength);

            //we can drop trailing dimensions where it's all singular for example:
            // shape: 4,3,1,2
            //dimension: 0,2
            // the problem for 0,2 is equivalent to: 0
            //the rest of the algorithm handles cases suchas
            //shape: 4,1,1,2
            //dimension: 0,1
            //when this happens there are other dimensions (eg: at the end) that matter
            int trailingOneDimensions = 0;
            //trailing ones
            for(int i = dimensionLength - 1; i >= 0; i--) {
                if(shape[dimension[i]] != 1) {
                    break;
                }
                else if(shape[dimension[i]] == 1)
                    trailingOneDimensions++;
            }

            dimensionLength -= trailingOneDimensions;

            int leadingOneDimensions = 0;
            //trailing ones
            for(int i = 0; i < dimensionLength; i++) {
                if(shape[dimension[i]] != 1) {
                    break;
                }
                else if(shape[dimension[i]] == 1)
                    leadingOneDimensions++;
            }

            //bump the dimension pointer forward for however many leadingones there are
            dimension += leadingOneDimensions;
            //decrease the dimension length by the amount of leading ones
            dimensionLength -= leadingOneDimensions;


            bool preConverged = true;
            for(int i = 0; i < dimensionLength; i++) {
                if(shape[dimension[i]] == 1) {
                    preConverged = false;
                    break;
                }
            }

            //we took away all the singular dimensions, we can just return
            if(preConverged)
                return;

            //no more singular dimensions specified
            bool done = false;
            int onesDecrement = 0;
            bool changed = false;
            while(!done) {
                //terminate early: only singular dimensions specified for reduce
                if((dimensionLength) < 1) {
                    done = true;
                    //signal as a no op
                    dimension[0] = -1;
                    break;
                }
                //captures intermediary result from the for loop
                traceNew(3);

                int intermediaryResult[MAX_RANK];
                for(int i = 0; i < dimensionLength; i++) {
                    intermediaryResult[i] = (dimension)[i];
                }

                bool oneEncountered = false;
                bool nonOneEncountered = false;
                bool hitBeginning = false;
                //assume intermediate collapsing of dimensions
                bool collapseMiddleDimensions = true;
                //note here that dimension length MAY end up being zero
                for(int i = (dimensionLength) - 1; i >= 0; i--) {
                    if(shape[(dimension)[i]] == 1) {
                        oneEncountered = true;
                        //trailing ones
                        if(!nonOneEncountered) {
                            //just drop trailing ones
                            dimensionLength--;
                            nonOneEncountered = false;
                            collapseMiddleDimensions = false;
                            //intermediary result just needs to have the results copied from dimension since we're just removing the tail
                            memcpy(intermediaryResult,dimension,sizeof(int) * dimensionLength);
                            changed = true;
                            //break the for loop and force it to go back around starting from the new index
                            break;
                        }
                        else {
                            //already decremented all dimensions
                            //this was a result of hitting beginning ones
                            //we will only need to loop once
                            if(i == 0) {
                                hitBeginning = true;
                            }
                            //will need to shift dimensions that aren't trailing ones
                            //back by onesDecrement
                            //mark the intermediary result as -1 for non inclusion
                            intermediaryResult[i] = -1;
                            onesDecrement++;
                        }
                    }
                    else {
                        intermediaryResult[i] = (dimension)[i];
                        nonOneEncountered = true;
                    }
                }

                if(collapseMiddleDimensions && oneEncountered) {
                    //collapse dimensions
                    int newIntermediary[MAX_RANK];
                    int idx = 0;
                    for(int i = 0; i < dimensionLength; i++) {
                        //of note: dimension will decrease by the number of ones encountered
                        if(intermediaryResult[i] >= 0) {
                            //dimension 0 doesn't need to be decremented
                            if(intermediaryResult[i] > 0)
                                newIntermediary[idx++] = intermediaryResult[i] - onesDecrement;
                            else
                                newIntermediary[idx++] = intermediaryResult[i];
                        }
                    }


                    //decrement by the number of dimensions where ones appeared
                    (dimensionLength) -= onesDecrement;
                    //update to current result
                    memcpy(dimension,newIntermediary,sizeof(int) * (dimensionLength));
                    changed = true;

                }
                    //converged: no need to change result
                else {
                    //update to current result
                    memcpy(dimension,intermediaryResult,sizeof(int) * dimensionLength);
                }

                //converge when there are no singular dimensions specified in the reduce
                done = (!oneEncountered && nonOneEncountered) || hitBeginning;
                //delete[] intermediaryResult;
            }

            //nothing changed but need to collapse dimension
            if(!changed && this->numOnes > 0) {
                for(int i = 0; i < dimensionLength ;i++) {
                    dimension[i] -= numOnes;
                }
            }


        }


    };


#ifdef __CUDACC__
    template <typename T>
__device__ INLINEDEF int *cuMalloc(int *buffer, long size, UnifiedSharedMemory *manager) {
    // if we go for 3 dimensions coord space or below - just use shared memory for that
    if (size <= MAX_COORD * 4) {
        int *ptr = new int[size / 4];//manager->getSharedCoordBuffer() + (threadIdx.x * MAX_COORD);
        return ptr;
    } else {
        // otherwise go to preallocated global memory :(
        int tid = blockIdx.x * blockDim.x + threadIdx.x;
        if (tid * size > PREALLOC_SIZE - size) {
            return (int *) malloc(size);
        } else {
            int *ret = buffer;
            ret += (tid * size);
            return ret;
        }
    }
}
#endif

#ifdef __CUDACC__
    /**
* BEWARE: THIS METHOD DOES NOT CHECKS ALLOCATION BOUNDARIES
*/
__device__ INLINEDEF int *cuMalloc(int *buffer, long size) {
    int *ret = buffer;
    ret += (threadIdx.x * size);
    return ret;
}
#endif

/**
* Length of a tad given
* the shape information
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int tadLength(int *shapeInfo, int *dimension, int dimensionLength) {
        if(dimensionLength == 1) {
            return shape::shapeOf(shapeInfo)[dimension[0]];
        }
        else {
            int ret = 1;
            for(int i = 0; i < shape::rank(shapeInfo); i++) {
                for(int j = 0; j < dimensionLength; j++) {
                    if(i == dimension[j])
                        ret *= shape::shapeOf(shapeInfo)[dimension[j]];
                }
            }
            return ret;
        }
    }



/**
 * Tad element wise stride:
 * given the inner most dimension (the sorted dimension of the last)
 * the element wise stride of the tad (disregarding order) is the
 * last dimension's stride.
 *
 * For a given singular dimension this will just be the only entry.
 * For example, given the following c order shape/stride:
 * 2,2,3,2
 * 12,6,2,1
 *
 * The tad element wise stride for 3 will be 1.
 * For zero it wil be 12
 *
 * For 2,3 it's 1
 *
 * Note here that the multi dimensional 2,3 case
 * is equivalent to the singular 3 case.
 *
 *
 * Note that this is for the dimension that ultimately
 * ends up removed.
 *
 * Again: this may not preserve ordering of the tad
 * but maybe used for reductions.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int tadElementWiseStride(int *shapeInfo,int *dimension,int dimensionLength) {
        return reductionIndexElementWiseStride(shapeInfo,dimension,dimensionLength);
    }





#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool shapeEquals(int shape1Rank,int *shape1,int shape2Rank,int *shape2) {
        if(shape1Rank != shape2Rank)
            return false;
        //rank not equals
        for(int i = 0; i < shape1Rank; i++) {
            if(shape1[i] != shape2[i])
                return false;
        }

        return true;
    }


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool shapeEquals(int *shapeInfo1,int *shapeInfo2) {
        return shape::shapeEquals(shape::rank(shapeInfo1),shape::shapeOf(shapeInfo1),shape::rank(shapeInfo2),shape::shapeOf(shapeInfo2));
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool strideEquals(int shape1Rank,int *shape1,int shape2Rank,int *shape2) {
        if(shape1Rank != shape2Rank)
            return false;
        //rank not equals
        for(int i = 0; i < shape1Rank; i++) {
            if(shape1[i] != shape2[i])
                return false;
        }

        return true;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool strideEquals(int *shapeInfo1,int *shapeInfo2) {
        return shape::strideEquals(shape::rank(shapeInfo1),shape::stride(shapeInfo1),shape::rank(shapeInfo2),shape::stride(shapeInfo2));

    }

#ifdef __CUDACC__
    __host__ __device__
#endif
	INLINEDEF bool strideEquals(int *stride1,int rank1 , int *stride2, int rank2) {
		if(rank1 != rank2)
            return false;
		
		for(int i = 0; i < rank1; i++) {
            if(stride1[i] != stride2[i])
                return false;
        }
	}

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *computeResultShape(int *originalShapeBuffer,int *dimension,int dimensionLength) {
        int *retShape;
        int retShapeLength;
        if(dimensionLength == 1 && dimension[0] == 2147483647) {
            retShape = new int[2];
            retShape[0] = 1;
            retShape[1] = 1;
            retShapeLength = 2;
        }
        else {
            retShape = shape::removeIndex(shape::shapeOf(originalShapeBuffer), dimension,
                                          shape::shapeInfoLength(shape::rank(originalShapeBuffer)), dimensionLength);
            retShapeLength =   shape::rank(originalShapeBuffer) - dimensionLength;
        }
        //ensure vector is proper shape
        if (retShapeLength == 1) {
            if (dimension[0] == 0) {
                int *newRetShape = new int[2]{1, retShape[0]};
                delete[] retShape;
                retShape = newRetShape;
                retShapeLength = 2;
            }
            else {
                int *newRetShape = new int[2]{retShape[0], 1};
                delete[] retShape;
                retShape = newRetShape;
                retShapeLength = 2;
            }
        } else if (retShapeLength == 0) {
            int *newRetShape = new int[2]{1, 1};
            delete[] retShape;
            retShape = newRetShape;
            retShapeLength = 2;
        }

        int *ret = shape::shapeBuffer(retShapeLength,retShape);
        delete[] retShape;

        return ret;

    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeInfoOnlyShapeAndStride(int *shapeInfo, int *dimension, int dimensionLength,bool reverseCopyStride, int *buffer) {
        int *theShape = shape::shapeOf(shapeInfo);
        int *theStride = shape::stride(shapeInfo);
        int rank = dimensionLength == 1 ? 2 : dimensionLength;
        int *ret = buffer;
        //set the rank
        ret[0] = rank;
        int *retShape = shape::shapeOf(ret);
        int *retStride = shape::stride(ret);
        int len = rank;

        if(dimensionLength == 1) {
            if(shape::isMatrix(theShape,shape::rank(shapeInfo))) {
                if(dimension[0] == 0) {
                    int newStride[2] = {theStride[dimension[0]],1};
                    int newShape[2] = {theShape[dimension[0]],1};
                    retShape[0] = newShape[0];
                    retShape[1] = newShape[1];
                    retStride[0] = newStride[0];
                    retStride[1] = newStride[1];
                }
                else {
                    int newStride[2] = {theStride[dimension[0]],1};
                    int newShape[2] = {theShape[dimension[0]],1};
                    retShape[0] = newShape[0];
                    retShape[1] = newShape[1];
                    retStride[0] = newStride[0];
                    retStride[1] = newStride[1];
                }
            }
            else {
                int newStride[2] = {1,theStride[dimension[0]]};
                int newShape[2] = {1,theShape[dimension[0]]};
                retShape[0] = newShape[0];
                retShape[1] = newShape[1];
                retStride[0] = newStride[0];
                retStride[1] = newStride[1];
            }



        }
        else {
            int *newIndexes = dimension;
            if(reverseCopyStride)
                shape::reverseCopyTo(theStride, retStride, newIndexes, len);
            else
                shape::copyTo(len, theStride, retStride, newIndexes);
            shape::copyTo(len, theShape, retShape, newIndexes);

        }


        ret[shape::shapeInfoLength(rank) - 1] = shape::order(shapeInfo);
        return ret;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeInfoOnlyShapeAndStride(int *shapeInfo, int *dimension, int dimensionLength,bool reverseCopyStride) {
        int rank = dimensionLength == 1 ? 2 : dimensionLength;

        traceNew(4);

        int *ret = new int[shape::shapeInfoLength(rank)];
        return shapeInfoOnlyShapeAndStride(shapeInfo, dimension, dimensionLength, reverseCopyStride, ret);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * createShapeInfo(int *shape, int *stride, int rank) {

        traceNew(5);

        int *ret = new int[shape::shapeInfoLength(rank)];

        return createShapeInfo(shape, stride, rank, ret);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * createShapeInfo(int *shape, int *stride, int rank, int *buffer) {
        buffer[0] = rank;
        int *retShape = shape::shapeOf(buffer);
        int *retStride = shape::stride(buffer);
        for(int i = 0;i < rank; i++) {
            retShape[i] = shape[i];
            retStride[i] = stride[i];
        }

        return buffer;
    }

/**
 * Computes the standard packed array strides for a given shape.
 *
 * @param shape    the shape of a matrix:
 * @param startNum the start number for the strides
 * @return the strides for a matrix of n dimensions
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * calcStridesFortran(int *shape, int rank, int startNum) {
        if (isVector(shape, rank)) {

            traceNew(5);

            int *ret = new int[2];
            for (int i = 0; i < 2; i++)
                ret[i] = 1;
            return ret;

        }

        int dimensions = rank;

        traceNew(6);

        int *stride = new int[dimensions];
        int st = startNum;
        for (int j = 0; j < rank; j++) {
            stride[j] = st;
            st *= shape[j];
        }

        return stride;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * calcStridesFortran(int *shape, int rank, int startNum, int *ret) {
        if (isVector(shape, rank)) {
            for (int i = 0; i < 2; i++)
                ret[i] = 1;
            return ret;

        }

        int dimensions = rank;

        int st = startNum;
        for (int j = 0; j < rank; j++) {
            ret[j] = st;
            st *= shape[j];
        }

        return ret;
    }

/**
 * Computes the standard packed array strides for a given shape.
 *
 * @param shape    the shape of a matrix:
 * @param startNum the start number for the strides
 * @return the strides for a matrix of n dimensions
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * calcStrides(int *shape, int rank, int startNum) {

        traceNew(7);

        int *stride = new int[rank];

        if (shape::isVector(shape, rank)) {
            for (int i = 0; i < 2; i++)
                stride[i] = 1;
            return stride;

        }

        int st = startNum;
        for (int j = rank - 1; j >= 0; j--) {
            stride[j] = st;
            st *= shape[j];
        }

        return stride;
    }


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * calcStrides(int *shape, int rank, int startNum, int* ret) {

        if (shape::isVector(shape, rank)) {
            for (int i = 0; i < 2; i++)
                ret[i] = 1;
            return ret;

        }

        int st = startNum;
        for (int j = rank - 1; j >= 0; j--) {
            ret[j] = st;
            st *= shape[j];
        }

        return ret;
    }

/**
 * Computes the standard packed array strides for a given shape.
 *
 * @param shape    the shape of a matrix:
 * @param startNum the start number for the strides
 * @return the strides for a matrix of n dimensions
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * calcStridesFortran(int *shape, int rank) {
        return calcStridesFortran(shape, rank, 1);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * calcStridesFortran(int *shape, int rank, int* ret) {
        return calcStridesFortran(shape, rank, 1, ret);
    }

/**
 * Computes the standard packed array strides for a given shape.
 *
 * @param shape    the shape of a matrix:
 * @param startNum the start number for the strides
 * @return the strides for a matrix of n dimensions
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* calcStrides(int *shape, int rank) {
        return calcStrides(shape, rank, 1);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* calcStrides(int *shape, int rank, int* ret) {
        return calcStrides(shape, rank, 1, ret);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void updateStrides(int *shape, const char order) {
        int rank = shape[0];
		int doubleRank = 2*rank;
		if(order == 'c') {
			shape[doubleRank] = 1;          // set unity as last stride for c order
			for(int j=1; j<rank; ++j)
				shape[doubleRank-j] = shape[doubleRank-j+1]*shape[rank+1-j];
		}
		else {
			shape[rank+1] = 1;             // set unity as first stride for f order
			for(int j=rank+1; j<doubleRank; ++j)
				shape[j+1] = shape[j]*shape[j-rank];
		}
		// set last 3 elements in shape
		shape[doubleRank + 1] = 0;                  
		shape[doubleRank + 2] = 1;
		shape[doubleRank + 3] = (int)order;
    }

/**
 * @param toCopy the shape to copy
 * @return a copy of the original struct
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF ShapeInformation *shapeCopy( ShapeInformation *toCopy) {
        ShapeInformation *copy = new ShapeInformation;

        traceNew(8);

        copy->shape = new int[toCopy->rank];

        memcpy(copy->shape, toCopy->shape, toCopy->rank * sizeof(int));

        traceNew(9);

        copy->stride = new int[toCopy->rank];
        for (int i = 0; i < toCopy->rank; i++) {
            copy->stride[i] = toCopy->stride[i];
        }
        copy->order = toCopy->order;
        copy->rank = toCopy->rank;
        copy->offset = toCopy->offset;
        copy->elementWiseStride = toCopy->elementWiseStride;
        return copy;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int computeElementWiseStride(int rank, int *shape, int *stride, int isFOrder) {
        if(shape::isVector(shape,rank)) {
            return stride[rank - 1];
        }

        else {
            int oldnd;
            int *olddims = shape::copyOf(rank, shape);
            int *oldstrides = shape::copyOf(rank, stride);
            int np, op, last_stride;
            int oi, oj, ok, ni, nj, nk;

            traceNew(10);

            int *newStrides = new int[rank];
            oldnd = 0;
            //set the shape to be 1 x length
            int newShapeRank = 2;
            int *newShape = new int[newShapeRank];
            newShape[0] = 1;
            newShape[1] = shape::prodLong(shape, rank);

            /*
             * Remove axes with dimension 1 from the old array. They have no effect
             * but would need special cases since their strides do not matter.
             */
            for (oi = 0; oi < rank; oi++) {
                if (shape[oi] != 1) {
                    olddims[oldnd] = shape[oi];
                    oldstrides[oldnd] = stride[oi];
                    oldnd++;
                }
            }

            np = 1;
            for (ni = 0; ni < newShapeRank; ni++) {
                np *= newShape[ni];
            }
            op = 1;
            for (oi = 0; oi < oldnd; oi++) {
                op *= olddims[oi];
            }
            if (np != op) {
/* different total sizes; no hope */
                delete[] newStrides;
                delete[] newShape;
                delete[] oldstrides;
                delete[] olddims;
                return -1;
            }

            if (np == 0) {
/* the current code does not handle 0-sized arrays, so give up */
                delete[] newStrides;
                delete[] newShape;
                delete[] oldstrides;
                delete[] olddims;
                return -1;
            }

/* oi to oj and ni to nj give the axis ranges currently worked with */
            oi = 0;
            oj = 1;
            ni = 0;
            nj = 1;
            while (ni < newShapeRank && oi < oldnd) {
                np = newShape[ni];
                op = olddims[oi];

                while (np != op) {
                    if (np < op) {
/* Misses trailing 1s, these are handled later */
                        np *= newShape[nj++];
                    } else {
                        op *= olddims[oj++];
                    }
                }

/* Check whether the original axes can be combined */
                for (ok = oi; ok < oj - 1; ok++) {
                    if (isFOrder) {
                        if (oldstrides[ok + 1] != olddims[ok] * oldstrides[ok]) {
/* not contiguous enough */
                            delete[] newStrides;
                            delete[] newShape;
                            delete[] oldstrides;
                            delete[] olddims;
                            return -1;
                        }
                    } else {
/* C order */
                        if (oldstrides[ok] != olddims[ok + 1] * oldstrides[ok + 1]) {
/* not contiguous enough */
                            delete[] newStrides;
                            delete[] newShape;
                            delete[] oldstrides;
                            delete[] olddims;
                            return -1;
                        }
                    }
                }

/* Calculate new strides for all axes currently worked with */
                if (isFOrder) {
                    newStrides[ni] = oldstrides[oi];
                    for (nk = ni + 1; nk < nj; nk++) {
                        newStrides[nk] = newStrides[nk - 1] * newShape[nk - 1];
                    }
                } else {
/* C order */
                    newStrides[nj - 1] = oldstrides[oj - 1];
                    for (nk = nj - 1; nk > ni; nk--) {
                        newStrides[nk - 1] = newStrides[nk] * newShape[nk];
                    }
                }
                ni = nj++;
                oi = oj++;
            }

/*
 * Set strides corresponding to trailing 1s of the new shape.
 */
            if (ni >= 1) {
                last_stride = newStrides[ni - 1];
            } else {
                last_stride = stride[rank - 1];
            }
            if (isFOrder) {
                if (ni >= 1)
                    last_stride *= newShape[ni - 1];
            }
            for (nk = ni; nk < newShapeRank; nk++) {
                newStrides[nk] = last_stride;
            }
//returns the last element of the new stride array
            int ret = last_stride;
            delete[] newStrides;
            delete[] newShape;
            delete[] oldstrides;
            delete[] olddims;
            return ret;
        }


    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int computeElementWiseStride(int rank, int *shape, int *stride, int isFOrder,
                                           int *dimension, int dimensionLength) {
        if(dimensionLength == 1) {
            return stride[dimension[0]];
        }
        return -1;

    }

/**
 * Get the shape info buffer
 * for the given rank and shape.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBuffer(int rank, int *shape) {
        int *stride = shape::calcStrides(shape, rank);

        traceNew(11);

        shape::ShapeInformation * shapeInfo = new shape::ShapeInformation();
        shapeInfo->shape = shape;
        shapeInfo->stride = stride;
        shapeInfo->offset = 0;
        shapeInfo->rank = rank;
        int elementWiseStride = shape::computeElementWiseStride(rank, shape, stride,
                                                                0);
        shapeInfo->order = 'c';
        shapeInfo->elementWiseStride = elementWiseStride;
        int *shapeInfoBuffer = shape::toShapeBuffer(shapeInfo);
        delete[] stride;
        delete shapeInfo;
        return shapeInfoBuffer;
    }

    /**
     * This is special method, it returns ONLY 2D shapebuffer.
     *
     * This method is used only for SoftMax
     */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBuffer(int rank, int *shape, int *buffer) {
        int stride[MAX_RANK];
        shape::calcStrides(shape,rank, stride);


        shape::ShapeInformation shapeInfo;
        shapeInfo.shape = shape;
        shapeInfo.stride = stride;
        shapeInfo.offset = 0;
        shapeInfo.rank = rank;
        int elementWiseStride = shape::computeElementWiseStride(rank, shape, stride, 0);

        shapeInfo.order = 'c';
        shapeInfo.elementWiseStride = elementWiseStride;
        shape::toShapeBuffer(&shapeInfo, buffer);
        return buffer;
    }

/**
* Get the shape info buffer
* for the given rank and shape.
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferFortran(int rank, int *shape) {
        int *stride = shape::calcStridesFortran(shape,rank);

        traceNew(12);

        shape::ShapeInformation * shapeInfo = new shape::ShapeInformation();
        shapeInfo->shape = shape;
        shapeInfo->stride = stride;
        shapeInfo->offset = 0;
        shapeInfo->rank = rank;
        int elementWiseStride = shape::computeElementWiseStride(rank, shape, stride, 0);

        shapeInfo->order = 'f';
        shapeInfo->elementWiseStride = elementWiseStride;
        int *shapeInfoBuffer = shape::toShapeBuffer(shapeInfo);
        delete[] stride;
        delete shapeInfo;
        return shapeInfoBuffer;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferFortran(int rank, int *shape, int *output) {
        int stride[MAX_RANK];
        shape::calcStridesFortran(shape,rank, stride);


        shape::ShapeInformation shapeInfo;
        shapeInfo.shape = shape;
        shapeInfo.stride = stride;
        shapeInfo.offset = 0;
        shapeInfo.rank = rank;
        int elementWiseStride = shape::computeElementWiseStride(rank, shape, stride, 0);

        shapeInfo.order = 'f';
        shapeInfo.elementWiseStride = elementWiseStride;
        shape::toShapeBuffer(&shapeInfo, output);
        return output;
    }

/**
 * Compute the real linear indices for the given shape and stride
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF Nd4jIndex *computeIndices(int rank, int *shape,  int *stride) {
        Nd4jIndex length = shape::prodLong(shape,rank);

        traceNew(13);

        Nd4jIndex *ret = new Nd4jIndex[length];
        for(int i = 0; i < length; i++) {
            int *idx = shape::ind2sub(rank, shape, i);
            ret[i] = shape::getOffset(0, shape, stride, idx, rank);
            delete[] idx;
        }

        return ret;
    }

/**
* Compute the real linear indices for the given shape and stride
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF Nd4jIndex *computeIndices(int *shapeBuffer) {
        return computeIndices(shape::rank(shapeBuffer),shape::shapeOf(shapeBuffer),shape::stride(shapeBuffer));
    }


/**
* Convert the given index (such as 1,1)
* to a linear index
* @param shape the shape of the indexes to convert
* @param indices the index to convert
* @return the linear index given the shape
* and indices
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int sub2Ind(int rank, int *shape, int *indices) {
        int index = 0;
        int shift = 1;

        for(int i = 0; i < rank; i++) {
            index += shift * indices[i];
            shift *= shape[i];
        }
        return index;
    }

/**
 * Convert a linear index to
 * the equivalent nd index
 * @param shape the shape of the dimensions
 * @param index the index to map
 * @param numIndices the number of total indices (typically prod of shape(
 * @return the mapped indexes along each dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* ind2sub(int rank,  int *shape, int index,int numIndices) {

        traceNew(14);

        int *ret = new int[rank];
        ind2sub(rank, shape, index, numIndices, ret);
        return ret;
    }

/**
 * Convert a linear index to
 * the equivalent nd index.
 * Infers the number of indices from the specified shape.
 *
 * @param shape the shape of the dimensions
 * @param index the index to map
 * @return the mapped indexes along each dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* ind2sub(int rank,  int *shape, int index) {
        return ind2sub(rank,shape, index,shape::prodLong(shape,rank));
    }

/**
* Convert a linear index to
* the equivalent nd index
* @param shape the shape of the dimensions
* @param index the index to map
* @param numIndices the number of total indices (typically prod of shape(
* @return the mapped indexes along each dimension
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void  ind2sub(int rank, int *shape, int index, int numIndices, int *ret) {
        int denom = numIndices;

        for(int i = rank - 1; i >= 0; i--) {
            denom /= shape[i];
            ret[i] = index / denom;
            index %= denom;
        }
    }

/**
     * Convert a linear index to
     * the equivalent nd index.
     * Infers the number of indices from the specified shape.
     *
     * @param shape the shape of the dimensions
     * @param index the index to map
     * @return the mapped indexes along each dimension
     */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void ind2sub(int rank,int *shape,int index, int *out) {
        ind2sub(rank,shape, index,shape::prodLong(shape,rank),out);
    }

/**
 * Convert a linear index to
 * the equivalent nd index
 * @param shape the shape of the dimensions
 * @param index the index to map
 * @param numIndices the number of total indices (typically prod of shape(
 * @return the mapped indexes along each dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int * ind2subC(int rank, int *shape, int index, int numIndices) {

        traceNew(15);

        int *ret = new int[rank];
        ind2subC(rank, shape, index, numIndices, ret);
        return ret;
    }

/**
 * Convert a linear index to
 * the equivalent nd index.
 * Infers the number of indices from the specified shape.
 *
 * @param shape the shape of the dimensions
 * @param index the index to map
 * @return the mapped indexes along each dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *ind2subC(int rank, int *shape, int index) {
        return ind2subC(rank,shape, index, shape::prodLong(shape,rank));
    }

/**
 * Convert a linear index to
 * the equivalent nd index
 * @param shape the shape of the dimensions
 * @param index the index to map
 * @param numIndices the number of total indices (typically prod of shape(
 * @return the mapped indexes along each dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void ind2subC(int rank, int *shape, int index, int numIndices, int *ret) {
        int denom = numIndices;
        for(int i = 0; i < rank; i++) {
            denom /= shape[i];
            if(denom > 0) {
                ret[i] = index / denom;
                index %= denom;
            }
            else
                ret[i] = 0;


        }
    }

/**
     * Convert a linear index to
     * the equivalent nd index.
     * Infers the number of indices from the specified shape.
     *
     * @param shape the shape of the dimensions
     * @param index the index to map
     * @return the mapped indexes along each dimension
     */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void ind2subC(int rank, int *shape, int index, int *out) {
        ind2subC(rank,shape, index,shape::prodLong(shape,rank),out);
    }

/**
* Convert a linear index to
* the equivalent nd index
* @param shape the shape of the dimensions
* @param index the index to map
* @param numIndices the number of total indices (typically prod of shape(
* @return the mapped indexes along each dimension
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void  ind2subOrder(int *shapeInfo, int index, int numIndices,int *out) {
        if(shape::order(shapeInfo) == 'f') {
            shape::ind2sub(
                    shape::rank(shapeInfo),
                    shape::shapeOf(shapeInfo),
                    index,
                    numIndices,
                    out);
        }
        else {
            shape::ind2subC(
                    shape::rank(shapeInfo),
                    shape::shapeOf(shapeInfo),
                    index,
                    numIndices,
                    out);

        }
    }

/**
* Convert a linear index to
* the equivalent nd index
* @param shape the shape of the dimensions
* @param index the index to map
* @param numIndices the number of total indices (typically prod of shape(
* @return the mapped indexes along each dimension
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void ind2subOrder(int *shapeInfo, int index, int *out) {
        ind2subOrder(shapeInfo,index,shape::length(shapeInfo),out);
    }

/**
 * Convert a linear index to
 * the equivalent nd index
 * @param shape the shape of the dimensions
 * @param index the index to map
 * @param numIndices the number of total indices (typically prod of shape(
 * @return the mapped indexes along each dimension
 */



/**
 *
 * @param length
 * @param shape
 * @param rearrange
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *doPermuteSwap(int length, int *shape, int *rearrange) {
        traceNew(16);
        int *ret = new int[length];
        for (int i = 0; i < length; i++) {
            ret[i] = shape[rearrange[i]];
        }
        return ret;
    }

/**
 *
 * @param length
 * @param shape
 * @param rearrange
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void doPermuteSwap(int length, int **shape, int *rearrange) {
        bool inOrder = true;
        for(int i = 0; i < length - 1; i++) {
            inOrder = inOrder && rearrange[i] + 1 == rearrange[i + 1];

        }

        //all in order, nothing to do
        if(inOrder)
            return;


        int *shapeDeref = *shape;
        //we know they are just reversed, dimension length of 2
        if(length == 2) {
            int shapeFirst = shapeDeref[0];
            int shapeSecond = shapeDeref[1];
            shapeDeref[0] = shapeSecond;
            shapeDeref[1] = shapeFirst;
            return;
        }

        int *temp = new int[length];
        memcpy(temp,shapeDeref,sizeof(int) * length);
        for (int i = 0; i < length; i++) {
            shapeDeref[i] = temp[rearrange[i]];
        }

        delete[] temp;
    }


#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void permuteShapeBufferInPlace(int *shapeBuffer,int *rearrange,int *out) {
        if(shapeBuffer != out)
            memcpy(out,shapeBuffer,sizeof(int) * shape::shapeInfoLength(shape::rank(shapeBuffer)));
        doPermuteShapeBuffer(shape::rank(shapeBuffer),shapeBuffer,rearrange,out);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *permuteShapeBuffer(int *shapeBuffer,int *rearrange) {
        int len = shape::shapeInfoLength(shape::rank(shapeBuffer));
        int *copy = shape::copyOf(len,shapeBuffer);
        doPermuteShapeBuffer(copy,rearrange);
        return copy;
    }
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void doPermuteShapeBuffer(int *shapeBuffer,int *rearrange) {
        int *shapeRef = shapeBuffer;
        //rank of the rearrange array == rank of shape buffer
        int rearrageRank = shape::rank(shapeRef);
        int *shape = shape::shapeOf(shapeRef);
        int *stride = shape::stride(shapeRef);
        shape::doPermuteSwap(rearrageRank,&shape,rearrange);
        shape::doPermuteSwap(rearrageRank,&stride,rearrange);
        shapeRef[shapeInfoLength(rearrageRank) - 2] = -1;
        shapeRef[shape::shapeInfoLength(rearrageRank) - 1] = shape::getOrder(rearrageRank,shape,stride,1);

    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void doPermuteShapeBuffer(int *shapeBuffer,int *rearrange, int *tmpBuffer) {
        int *shapeRef = shapeBuffer;
        //rank of the rearrange array == rank of shape buffer
        int rearrageRank = shape::rank(shapeRef);
        int *shape = shape::shapeOf(shapeRef);
        int *stride = shape::stride(shapeRef);

        shape::copyOf(rearrageRank,rearrange, tmpBuffer);
        shape::doPermuteSwap(rearrageRank,&shape,tmpBuffer);

        shape::copyOf(rearrageRank,rearrange, tmpBuffer);
        shape::doPermuteSwap(rearrageRank,&stride,tmpBuffer);
        shapeRef[shapeInfoLength(rearrageRank) - 2] = -1;
        shapeRef[shape::shapeInfoLength(rearrageRank) - 1] = shape::getOrder(rearrageRank,shape,stride,1);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void doPermuteShapeBuffer(int rank,int *shapeBuffer,int *rearrange) {
        int *shapeRef = shapeBuffer;
        //rank of the rearrange array == rank of shape buffer
        int rearrageRank = rank;
        int *shape = shape::shapeOf(shapeRef);
        int *stride = shape::stride(shapeRef);
        int *rearrangeCopy1 = shape::copyOf(rearrageRank,rearrange);
        shape::doPermuteSwap(rearrageRank,&shape,rearrangeCopy1);
        delete[] rearrangeCopy1;
        int *rearrangeCopy2 = shape::copyOf(rearrageRank,rearrange);
        shape::doPermuteSwap(rearrageRank,&stride,rearrangeCopy2);
        shapeBuffer[shape::shapeInfoLength(rank) - 1] = shape::getOrder(rank,shape,stride,1);
        shapeBuffer[shape::shapeInfoLength(rank) - 2] = -1;
        delete[] rearrangeCopy2;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void doPermuteShapeBuffer(int rank,int *shapeBuffer,int *rearrange, int *tmpBuffer) {
        int *shapeRef = shapeBuffer;
        //rank of the rearrange array == rank of shape buffer
        int rearrageRank = rank;
        int *shape = shape::shapeOf(shapeRef);
        int *stride = shape::stride(shapeRef);
        if(shapeBuffer != tmpBuffer)
            shape::copyOf(rearrageRank,shapeBuffer, tmpBuffer);
        shape::doPermuteSwap(rearrageRank,&shape,rearrange);
        shape::doPermuteSwap(rearrageRank,&stride,rearrange);
        shapeRef[shapeInfoLength(rank) - 2] = -1;
        shapeRef[shape::shapeInfoLength(rank) - 1] = shape::getOrder(rank,shape,stride,1);
    }


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *createPermuteIndexes(int originalRank,int *dimension,int dimensionLength) {
        int delta = originalRank - dimensionLength;

        traceNew(17);

        int *ret = new int[originalRank];
        for(int i = 0; i < delta; i++) {
            ret[i] = i + dimensionLength;
        }

        for(int i = delta; i  < originalRank; i++) {
            ret[i] = i - delta;
        }

        return ret;
    }

/**
 * Get the ordering for the device
 * @param length
 * @param shape
 * @param stride
 * @param elementStride
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF char getOrder(int length, int *shape, int *stride, int elementStride) {
        int sd = -1;
        int dim = -1;
        int i = -1;
        int cContiguous = 1;
        int isFortran = 1;

        sd = 1;
        for (i = length - 1; i >= 0; --i) {
            dim = shape[i];

            if (stride[i] != sd) {
                cContiguous = 0;
                break;
            }
            /* contiguous, if it got this far */
            if (dim == 0) {
                break;
            }
            sd *= dim;

        }

        /* check if fortran contiguous */
        sd = elementStride;
        for (i = 0; i < length; ++i) {
            dim = shape[i];
            if (stride[i] != sd) {
                isFortran = 0;
            }
            if (dim == 0) {
                break;
            }
            sd *= dim;

        }

        if (isFortran && cContiguous)
            return 'a';
        else if (isFortran && !cContiguous)
            return 'f';
        else if (!isFortran && !cContiguous)
            return 'c';
        else
            return 'c';

    }





/**
 * Ensure that every value in the re arrange
 * array is unique
 * @param arr
 * @param shape
 * @param arrLength
 * @param shapeLength
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int checkArrangeArray(int *arr, int arrLength, int shapeLength) {
        if (arrLength != shapeLength)
            return -1;
        for (int i = 0; i < arrLength; i++) {
            if (arr[i] >= arrLength || arr[i] < 0)
                return -1;
        }

        for (int i = 0; i < arrLength; i++) {
            for (int j = 0; j < arrLength; j++) {
                if (i != j && arr[i] == arr[j])
                    return -1;
            }
        }

        return 1;
    }

/**
 * Permute the shape information
 * @param info the shape information to permute
 * @param rearrange the order to re arrange
 * @param rank the rank of the rearrange array
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void permute(ShapeInformation **info, int *rearrange, int rank) {
        ShapeInformation *infoDeref = *info;
        checkArrangeArray(rearrange, rank, rank);
        shape::doPermuteSwap(rank, &infoDeref->shape, rearrange);
        shape::doPermuteSwap(rank, &infoDeref->stride, rearrange);
        char order = getOrder(rank,
                              infoDeref->shape,
                              infoDeref->stride,
                              infoDeref->elementWiseStride);
        infoDeref->order = order;

    }

/**
 * Returns whether the
 * given shape is a vector or not
 * @param shape the shape of the array
 * @param rank the rank of the shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isVector(int *shape, int rank) {
        if (rank > 2)
            return 0;
        else if (rank <= 2) {
            if (shape[0] == 1 || shape[1] == 1)
                return 1;
        }
        return 0;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isVector(int *shapeInfo) {
        return isVector(shape::shapeOf(shapeInfo),shape::rank(shapeInfo));
    }


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool isRowVector(int *shapeInfo) {
        bool isVector = shape::isVector(shapeInfo) == 1;
        bool shapeFirstOne = shape::shapeOf(shapeInfo)[0] == 1;
        return isVector && shapeFirstOne;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF bool isColumnVector(int *shapeInfo) {
        bool isVector = shape::isVector(shapeInfo) == 1;
        bool shapeFirstOne = shape::shapeOf(shapeInfo)[0] == 1;
        return isVector && !shapeFirstOne;
    }
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int oneDimEqualToLength(int *shape, int rank) {
        for(int i = 0; i < rank; i++) {
            if(shape[i] == shape::prod(shape,rank))
                return 1;
        }

        return 0;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int oneDimEqualToLength(int *shapeInfo) {
        return oneDimEqualToLength(shape::shapeOf(shapeInfo),shape::rank(shapeInfo));
    }

/**
* Returns whether the
* given shape is a vector or not
* @param shape the shape of the array
* @param rank the rank of the shape
*/
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isMatrix(int *shape, int rank) {
        if (rank > 2)
            return 0;
        else if (rank <= 2) {
            if (shape[0] == 1 || shape[1] == 1)
                return 0;
        }

        return 1;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isMatrix(int *shapeInfo) {
        return isMatrix(shape::shapeOf(shapeInfo),shape::rank(shapeInfo));
    }

/**
 * Returns the shape portion of an information
 * buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *shapeOf(int *buffer) {
        return buffer + 1;
    }

/**
 * Return a copy of a buffer.
 * This buffer allocates memory
 * that must be freed elsewhere.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *copyOf(int length, int *toCopy) {
        traceNew(18);

        int *ret = new int[length];
        return copyOf(length, toCopy, ret);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *copyOf(int length, int *toCopy, int *ret) {
        memcpy(ret, toCopy, sizeof(int)*length);
        return ret;
    }

/**
* Return a copy of a buffer.
* This buffer allocates memory
* that must be freed elsewhere.
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void copyTo(int length, int *from, int *to) {
        memcpy(to, from, sizeof(int)*length);
    }

/**
* Return a copy of a buffer.
* This buffer allocates memory
* that must be freed elsewhere.
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void copyTo(int length, int *from, int *to, int *indexes) {
        for(int i = 0; i < length; i++) {
            to[i] = from[indexes[i]];
        }
    }

/**
 * Permute the given strides
 * in the given rearrange order
 * @param toPermute the buffer to permute
 * @param shapeRank the length of the buffer to permute
 * @param rearrange the rearrange order (must be 0 based indexes
 * and all must be filled in)
 * @return the rearranged array
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *permutedStrides(int *toPermute, int shapeRank, int *rearrange) {
        int *strideCopy = copyOf(shapeRank, toPermute);
        checkArrangeArray(rearrange, shapeRank, shapeRank);
        int *newStride = doPermuteSwap(shapeRank, strideCopy, rearrange);
        delete[] strideCopy;
        return newStride;
    }

/**
 * Return the slice (shape + 1 in pointer arithmetic)
 * @param shape the shape to take the slice of
 * @return the shape array - the first entry
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *slice(int *shape) {
        return shape + 1;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int slices(int *shapeBuffer) {
        return shape::shapeOf(shapeBuffer)[0];
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *sliceOfShapeBuffer(int sliceIdx,int *shapeBuffer) {
        int rank = shape::rank(shapeBuffer);
        int newRank = rank - 1;
        if(newRank < 2)
            newRank = 2;
        int *newShapeBuffer = new int[shape::shapeInfoLength(newRank)];
        newShapeBuffer[0] = newRank;
        int *currShape = shape::shapeOf(shapeBuffer);
        int *currStride = shape::stride(shapeBuffer);
        //initialize new shape and stride by taking the shape and stride + 1
        //and adding to the shape information
        //a slice is always just taking the existing shape and cutting the first index off
        //of the shape and stride
        int *newShape = shape::shapeOf(newShapeBuffer);
        int *newStride = shape::stride(newShapeBuffer);
        if(shape::isVector(shapeBuffer)) {
            int *currShape = shape::shapeOf(shapeBuffer);
            //row vector: slice index 0 is a valid index, just copy the whole thing
            if(currShape[0] == 1) {
                if(sliceIdx == 0) {
                    memcpy(newShapeBuffer,shapeBuffer,shape::shapeInfoLength(shape::rank(shapeBuffer) * sizeof(int)));
                    return newShapeBuffer;
                }
            }
                //column vector: this will be a scalar
            else {
                delete[] newShapeBuffer;
                int *scalar = shape::createScalarShapeInfo();
                int offset = shape::offset(shapeBuffer);
                scalar[shape::shapeInfoLength(2) - 3] = offset + sliceIdx;
                return scalar;

            }

        }
        else if(shape::isMatrix(shapeBuffer)) {
            newShape[0] = 1;
            newShape[1] = currShape[1];
            newStride[0] = 1;
            newStride[1] = currStride[1];
        }
        else {
            for(int i = 0; i < newRank; i++) {
                newShape[i] = currShape[i + 1];
                newStride[i] = currStride[i + 1];
            }
        }


        int *indices = new int[rank];
        memset((void *) indices,0,rank * sizeof(int));
        indices[0] = sliceIdx;
        Nd4jIndex offset = shape::getOffset(0,newShape,newStride,indices,rank);
        newShapeBuffer[shape::shapeInfoLength(newRank) - 3] = offset;
        if(shape::isMatrix(shapeBuffer)) {
            newShapeBuffer[shape::shapeInfoLength(newRank) - 2] = currStride[1];
        }
        else {
            newShapeBuffer[shape::shapeInfoLength(newRank) - 2] = shape::elementWiseStride(shapeBuffer);
        }
        newShapeBuffer[shape::shapeInfoLength(newRank) - 1] = shape::getOrder(newRank,newShape,newStride,1);


        delete[] indices;

        return newShapeBuffer;
    }

/**
 * Returns the length of the
 * shape information buffer:
 * rank * 2 + 3
 * @param rank the rank to get the shape
 * info length for
 * @return rank * 2 + 4
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int shapeInfoLength(int rank) {
        //FIXME magic numbers
        return rank * 2 + 4;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int shapeInfoLength(int* shape) {
        return shapeInfoLength(shape[0]);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF size_t shapeInfoByteLength(int rank) {
        //FIXME magic numbers
        return (rank * 2 + 4) * sizeof(int);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF size_t shapeInfoByteLength(int* shapeInfo) {
        //FIXME magic numbers
        return shapeInfoByteLength(shapeInfo[0]);
    }

/**
 * Returns the rank portion of
 * an information buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int rank( int *buffer) {
        return buffer[0];
    }

/**
 * Converts a raw int buffer of the layout:
 * rank
 * shape
 * stride
 * offset
 * elementWiseStride
 *
 * where shape and stride are both straight int pointers
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF ShapeInformation *infoFromBuffer(int *buffer) {

        traceNew(19);

        ShapeInformation *info = new ShapeInformation;
        int length = shapeInfoLength(rank(buffer));
        int rank = buffer[0];

        //start after rank
        info->shape = buffer + 1;
        info->stride = buffer + (1 + rank);
        info->rank = rank;
        info->offset = buffer[length - 3];
        info->elementWiseStride = buffer[length - 2];
        int *stride = buffer + 1 + rank;
        info->stride = stride;
        info->order = (char) buffer[length - 1];
        return info;
    }

/**
 * Returns the stride portion of an information
 * buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF  int *stride( int *buffer) {
        return buffer + (1 + rank(buffer));
    }



/**
 * Compute the length of the given shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF Nd4jIndex length(int *shapeInfo) {
        return shape::prodLong(shape::shapeOf(shapeInfo), shape::rank(shapeInfo));
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF Nd4jIndex length(std::initializer_list<int>& shape) {
        Nd4jIndex ret = 0;
        for (auto v : shape) {
            ret *= v;
        }
        return ret;
    }

/***
 * Returns the offset
 * portion of an information buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int offset(int *buffer) {
        return buffer[shape::shapeInfoLength(shape::rank(buffer)) - 3];
    }

/**
 * Returns the ordering
 * for this shape information buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF char order(int *buffer) {
        //FIXME magic numbers
        return (char) buffer[(buffer[0] * 2 + 4) - 1];
    }

/**
 * Returns the element wise stride for this information
 * buffer
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int elementWiseStride(int *buffer) {
        return buffer[shapeInfoLength(buffer[0]) - 2];
    }

/**
* Returns the element wise stride for this information
* buffer relative to a dimension and reduction index
*/
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int reductionIndexElementWiseStride(int *buffer, int *dimension, int dimensionLength) {
        if(dimensionLength > 1) {
            if(shape::order(buffer) == 'f') {
                /**
                        * The element wise stride belongs to a reduction index.
                        * When used out of order, we can get rid of the data
                        * dependencies and rely on using the max dimension
                        * specified for stride instead.
                        * Say we take the sum(0,1) along arr
                        * we can use arr.stride(1) as a representation
                        * along which to iterate.
                        */
                if(shape::shapeOf(buffer)[dimension[dimensionLength - 1]] != 1) {
                    //int tadElementWiseStride = shape::stride(buffer)[dimension[dimensionLength - 1]];
                    //return tadElementWiseStride;
                    int tadElementWiseStride = shape::stride(buffer)[dimension[0]];
                    return tadElementWiseStride;
                }

                return 1;

            }
            else {
                /**
                        * The element wise stride belongs to a reduction index.
                        * When used out of order, we can get rid of the data
                        * dependencies and rely on using the max dimension
                        * specified for stride instead.
                        * Say we take the sum(0,1) along arr
                        * we can use arr.stride(1) as a representation
                        * along which to iterate.
                        */
                if(shape::shapeOf(buffer)[dimension[dimensionLength - 1]] != 1) {
                    int tadElementWiseStride = shape::stride(buffer)[dimension[dimensionLength - 1]];
                    return tadElementWiseStride;
                }

                return 1;
            }
        }
        else {
            if(shape::order(buffer) == 'f') {
                /**
                        * The element wise stride belongs to a reduction index.
                        * When used out of order, we can get rid of the data
                        * dependencies and rely on using the max dimension
                        * specified for stride instead.
                        * Say we take the sum(0,1) along arr
                        * we can use arr.stride(1) as a representation
                        * along which to iterate.
                        */
                int tadElementWiseStride = shape::stride(buffer)[dimension[0]];
                return tadElementWiseStride;
            }
            else {
                /**
                        * The element wise stride belongs to a reduction index.
                        * When used out of order, we can get rid of the data
                        * dependencies and rely on using the max dimension
                        * specified for stride instead.
                        * Say we take the sum(0,1) along arr
                        * we can use arr.stride(1) as a representation
                        * along which to iterate.
                        */
                int tadElementWiseStride = shape::stride(buffer)[dimension[dimensionLength - 1]];
                return tadElementWiseStride;
            }
        }

    }

/**
 * Returns whether
 * the given shape info buffer
 * represents a scalar shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isScalar(int *info) {
        if (shape::rank(info) > 2)
            return 0;
        if (shape::rank(info) == 1)
            return shape::shapeOf(info)[0] == 1;
        else if (rank(info) == 2) {
            return shape::shapeOf(info)[0] == 1 && shape::shapeOf(info)[1] == 1;
        }
        return 0;
    }

/**
 * Returns whether
 * the given shape information
 * represents a scalar
 * shape or not
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int isScalar(volatile ShapeInformation *info) {
        if (info->rank > 2)
            return 0;
        if (info->rank == 1)
            return info->shape[0] == 1;
        else if (info->rank == 2) {
            return info->shape[0] == 1 && info->shape[1] == 1;
        }
        return 0;
    }

/**
 * Return a copy of this array with the
 * given index omitted
 *
 * @param data  the data to copy
 * @param indexes the index of the item to remove
 * @param dataLength the length of the data array
 * @param indexesLength the length of the data array
 * @return the new array with the omitted
 *
 * item
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void removeIndex(int *data, int *indexes, int dataLength, int indexesLength,
                               int *ret) {
        int count = 0;
        int absLength = dataLength - indexesLength;
        for (int i = 0; i < dataLength && count < absLength; i++) {
            int contains = 0;
            for (int j = 0; j < indexesLength; j++) {
                if (i == indexes[j]) {
                    contains = 1;
                    break;
                }
            }

            if (!contains) {
                ret[count] = data[i];
                count++;
            }
        }
    }

    /**
 * Return a copy of this array with the
 * given index omitted
 *
 * @param data  the data to copy
 * @param indexes the index of the item to remove
 * @param dataLength the length of the data array
 * @param indexesLength the length of the data array
 * @return the new array with the omitted
 *
 * item
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int * removeIndex(int *data, int *indexes, int dataLength, int indexesLength) {
        int lengthOfArr = dataLength - indexesLength;
        if(lengthOfArr < 0) {
            printf("Remove index call created a <= 0 length array. This was likely not intended.");
        }
        int *ret = new int[lengthOfArr];
        removeIndex(data,indexes,dataLength,indexesLength,ret);
        return ret;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* everyIndexBut(int *indexes,int indexesLength,int begin,int end) {
        int len = end - indexesLength;

        traceNew(20);

        int *ret = new int[len];
        int retIdx = 0;
        //not here that we do 0 based indexing for end - this assumes things like:
        //0 to 4 are specified
        for(int i = begin; i < end ; i++) {
            bool found = false;
            for(int j = 0; j < indexesLength; j++) {
                if(indexes[j] == i) {
                    found = true;
                    break;
                }
            }

            if(!found) {
                ret[retIdx++] = i;
            }

        }

        return ret;

    }

/**
 * Computes the offset for accessing
 * a global element given the shape information
 * and the offset to be read.
 */
#ifdef __CUDACC__
    __device__ int tadOffset(ShapeInformation *xInfo, int offset) {
    return offset + threadIdx.x * xInfo->elementWiseStride;

}

#endif

/**
 * Returns a shape
 * forces the given length to be 2.
 * @param shape the shape to modify
 * @param dimension the dimension (row or column)
 * for the shape to be returned as
 * @return the new shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *ensureVectorShape(int *shape, int dimension) {
        traceNew(21);

        int *ret = new int[2];

        if (dimension == 0) {
            ret[0] = 1;
            ret[1] = shape[0];
        } else {
            ret[0] = shape[0];
            ret[1] = 1;
        }

        return ret;
    }

/**
 * Returns a shape
 * forces the given length to be 2.
 * @param shape the shape to modify
 * @param dimension the dimension (row or column)
 * for the shape to be returned as
 * @return the new shape
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *ensureVectorShape(int *shape) {
        return ensureVectorShape(shape, 0);
    }

    /**
     * This method does STRICT comparison for two shape buffers
     *
     * @param shape
     * @return
     */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF bool equalsStrict(int *shapeA, int *shapeB) {
        if (shapeA[0] != shapeB[0])
            return false;

        // we do full comparison here
        int length = shape::shapeInfoLength(shapeA[0]);

        for (int e = 1; e < length; e++)
            if (shapeA[e] != shapeB[e])
                return false;

        return true;
    }

    /**
     * This method does SOFT comparison for two shape buffers, we compare only rank & shapes
     *
     * @param shape
     * @return
     */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF bool equalsSoft(int *shapeA, int *shapeB) {
        if (shapeA[0] != shapeB[0])
            return false;

        // we compare only shapes, and ignoring stride & ews
        int length = shapeA[0];

        for (int e = 1; e <= length; e++)
            if (shapeA[e] != shapeB[e])
                return false;

        return true;
    }

/**
 * Generate an int buffer
 * up to the given length
 * at the specified increment
 *
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *range(int from, int to, int increment) {
        int diff = nd4j::math::nd4j_abs<int>(from - to);
        int retLength = diff / increment;
        int *ret;

        traceNew(22);

        if(diff / increment < 1)
            ret = new int[1];
        else
            ret = new int[diff / increment];
        if (from < to) {
            int count = 0;
            for (int i = from; i < to; i += increment) {
                if (count >= retLength)
                    break;
                ret[count++] = i;
            }
        } else if (from > to) {
            int count = 0;
            for (int i = from - 1; i >= to; i -= increment) {
                if (count >= retLength)
                    break;
                ret[count++] = i;
            }
        }

        return ret;
    }

/**
 * Generate a range
 * beginning at from and ending at to
 * incrementing by 1
 * @param from the start
 * @param to the end
 * @return the int array starting at from and ending at to
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *range(int from, int to) {
        return range(from, to, 1);
    }

/**
 * Keep the given indexes in the data
 * @param data
 * @param index
 * @param indexLength
 * @param dataLength
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *keep(volatile int *data, int *index, int indexLength, int dataLength) {

        traceNew(23);

        int *ret = new int[indexLength];
        int count = 0;
        for (int i = 0; i < dataLength; i++) {
            int contains = 0;
            for (int j = 0; j < indexLength; j++) {
                if (i == index[j]) {
                    contains = 1;
                    break;
                }
            }

            if (contains)
                ret[count++] = data[i];
        }
        return ret;
    }

/**
 * Generate a reverse
 * copy of the data
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *reverseCopy(int *data, int length) {
        if (length < 1)
            return nullptr;

        traceNew(24);

        int *copy = new int[length];
        for (int i = 0; i <= length / 2; i++) {
            int temp = data[i];
            copy[i] = data[length - i - 1];
            copy[length - i - 1] = temp;
        }
        return copy;
    }


#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void reverseCopyTo(int *from, int *to, int length) {
        if (length < 1)
            return;
        for (int i = 0; i <= length / 2; i++) {
            int temp = from[i];
            to[i] = from[length - i - 1];
            to[length - i - 1] = temp;
        }
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF void reverseCopyTo(int *from, int *to, int *indexes, int length) {
        if (length < 1)
            return;

        for (int i = 0; i <= length / 2; i++) {
            int temp = from[indexes[i]];
            to[i] = from[indexes[length - i - 1]];
            to[length - i - 1] = temp;
        }

    }

/**
 *
 * @param arr1
 * @param arr1Length
 * @param arr2
 * @param arr2Length
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *concat(int *arr1, int arr1Length, int *arr2, int arr2Length) {

        traceNew(25);

        int *ret = new int[arr1Length + arr2Length];
        std::memcpy(ret, arr1, arr1Length * sizeof(int));
        std::memcpy(ret + arr1Length, arr2, arr2Length * sizeof(int));
        return ret;
    }

/**
 *
 * @param numArrays
 * @param numTotalElements
 * @param arr
 * @param lengths
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *concat(int numArrays, int numTotalElements, int **arr, int *lengths) {

        int *ret = new int[numTotalElements];
        int count = 0;

        for (int i = 0; i < numArrays; i++) {
            for (int j = 0; j < lengths[i]; j++) {
                ret[count++] = arr[i][j];
            }
        }

        return ret;
    }

/**
 * Get the length per slice of the
 * given shape and the dimension
 * @param rank the rank of the shape
 * @param shape the shape of to get
 * the length per slice for
 * @param dimension the dimension to
 * get the length per slice for
 * @param dimensionLength the length of the dimension array
 * @return the length per slice of the given shape
 * along the given dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int lengthPerSlice(int rank, int *shape, int *dimension, int dimensionLength) {
        if(shape::isVector(shape,rank)) {
            //return total length for row vectors
            if(dimensionLength == 1 && shape[0] == 1) {
                return shape::prod(shape,rank);
            }
        }
        else if(rank == dimensionLength)
            return shape::prod(shape,rank);
        int absSelta = nd4j::math::nd4j_abs<int>(rank - dimensionLength);
        traceNew(27);
        int *ret2 = shape::removeIndex(shape,dimension,rank,dimensionLength);
        int ret = prod(ret2, absSelta);
        delete[] ret2;
        return ret;
    }

/**
 * calculates the offset for a tensor
 * @param index
 * @param arr
 * @param tensorShape
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int sliceOffsetForTensor(int rank, int index, int *shape, int *tensorShape,
                                       int tensorShapeLength, int *dimension, int dimensionLength) {
        int tensorLength = prodLong(tensorShape, tensorShapeLength);
        int lengthPerSlice2 = lengthPerSlice(rank, shape, dimension,
                                             dimensionLength);
        if (lengthPerSlice2 <= 0) {
            return 0;
        }

        int offset = index * tensorLength / lengthPerSlice2;
        return offset;
    }

    /**
 * calculates the offset for a tensor
 * @param index
 * @param arr
 * @param tensorShape
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int sliceOffsetForTensor(int index,int tensorLength,int lengthPerSlice2) {
        int offset = index * tensorLength / lengthPerSlice2;
        return offset;
    }


#ifdef __CUDACC__

    /**
* Computes the offset for accessing
* a global element given the shape information
* and the offset to be read.
*/
__device__ int tadOffset(int *xInfo, int offset) {
    return offset + threadIdx.x * elementWiseStride(xInfo);

}
#endif





/**
 * Computes the number
 * of tensors along
 * a given dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tensorsAlongDimension(volatile int rank, volatile int length,
                                        volatile int *shape, int *dimension, int dimensionLength) {
        int *tensorShape = shape::keep(shape, dimension, dimensionLength, rank);
        int ret = length / shape::prodLong(tensorShape, dimensionLength);
        delete[] tensorShape;
        return ret;
    }

/**
 * Computes the number
 * of tensors along
 * a given dimension
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tensorsAlongDimension(int *shapeInfo, int *dimension, int dimensionLength) {
        int *keepShape = shape::shapeOf(shapeInfo);
        int *tensorShape = shape::keep(keepShape, dimension, dimensionLength,
                                       rank(shapeInfo));
        int ret = shape::length(shapeInfo)
                  / shape::prodLong(tensorShape, dimensionLength);
        delete[] tensorShape;
        return ret;
    }




/**
* Get an offset for retrieval
* from a data buffer
* based on the given
* shape stride and given indices
* @param baseOffset the offset to start from
* @param shape the shape of the array
* @param stride the stride of the array
* @param indices the indices to iterate over
* @return the double at the specified index
*/
#ifdef __CUDACC__
    __host__ __device__
#endif
    Nd4jIndex getOffset(Nd4jIndex baseOffset,  int *shape,  int *stride,  int *indices, int rank) {
        Nd4jIndex offset = baseOffset;
        for(int i = 0; i < rank; i++) {
            if(indices[i] >= shape[i] && shape[i] != 1) {
                printf("Index %d [%d] must not be >= shape[%d].\n", i,indices[i],shape[i]);
                return -1;
            }

            if(shape[i] != 1) {
                offset += (Nd4jIndex) indices[i] * (Nd4jIndex) stride[i];
            }
        }

        return offset;
    }




/**
 * Returns the tensor along dimension
 * for the given block index
 * @param blockSize
 * @param blockIdx
 * @param i
 * @return
 */
#ifdef __CUDACC__
    __device__ __host__
#endif
    INLINEDEF int tadForBlockIndex(int blockSize, int blockIdx, int i) {
        return blockIdx + i * blockSize;
    }

/**
 * Computes the number of tads per block
 *
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tadsPerBlock(int blockSize, int tads) {
        return nd4j::math::nd4j_ceil<double>(tads / (double) blockSize);
    }

/**
 * Returns a shape buffer
 * for the shape information metadata.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *toShapeBuffer( ShapeInformation *info) {

        traceNew(29);

        int *ret = new int[shapeInfoLength(info->rank)];
        int count = 1;
        int rank = info->rank;

        ret[0] = info->rank;

        for (int i = 0; i < rank; i++) {
            ret[count++] = info->shape[i];
        }

        for (int i = 0; i < rank; i++) {
            ret[count++] = info->stride[i];
        }

        ret[count++] = info->offset;
        ret[count++] = info->elementWiseStride;
        ret[count++] = info->order;

        return ret;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int *toShapeBuffer( ShapeInformation *info, int* ret) {

        int count = 1;
        int rank = info->rank;

        ret[0] = info->rank;

        for (int i = 0; i < rank; i++) {
            ret[count++] = info->shape[i];
        }

        for (int i = 0; i < rank; i++) {
            ret[count++] = info->stride[i];
        }

        ret[count++] = info->offset;
        ret[count++] = info->elementWiseStride;
        ret[count++] = info->order;

        return ret;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    void printIntArray(int *arr,int length) {
        for(int i = 0; i < length; i++) {
            printf(" %d ",arr[i]);
        }

        printf("\n");
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    void printShapeInfo(int *shapeInfo) {
        int rank = shape::rank(shapeInfo);
        int *shape = shape::shapeOf(shapeInfo);
        printf("Rank %d\n",rank);
        printf("Shape:\n");
        for(int i = 0; i < rank; i++) {
            printf(" %d ",shape[i]);
        }

        printf("\n");

        int *stride = shape::stride(shapeInfo);
        printf("Stride:\n");
        for(int i = 0; i < rank; i++) {
            printf(" %d ",stride[i]);
        }

        printf("\n");

        printf("Order %c\n",shape::order(shapeInfo));
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    void printShapeInfoLinear(int *shapeInfo) {
        int rank = shape::rank(shapeInfo);
        printf("ShapeInfo: [");
        for (int i = 0; i < rank * 2 + 4; i++) {
            printf("%i, ", shapeInfo[i]);
        }
        printf("]\n");
#ifndef __CUDACC__
        fflush(stdout);
#endif
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF void printArray(float *arr,int length) {
        printf("Array: [");
        for (int i = 0; i < length; i ++) {
            printf("%f", arr[i]);
            if (i + 1 < length) printf(", ");
        }
        printf("]\n");
    }
/**
 * Given an linear index, element wise stride
 * and the length of each tad
 * map a linear index to a tad
 * @param i the index to map
 * @param the element wise stride for the tads
 * @param numElementsPerTad the number of elements
 * per tad
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tadIndex(int i, int elementWiseStride, int numElementsPerTad) {
        return i / (numElementsPerTad * elementWiseStride);
    }

/**
 * Map a tad to a
 * reduction index.
 * @param tadIndexForOriginal the original tad index for the
 * split up problem (eg: split is dimension 3 mapping to a 2,3 problem)
 * @param tadsForReduced the number of tads for the shrunk down problem (eg: 2,3)
 * @param tadsForOriginal the number of tads for the smaller problem (eg: 3)
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int reductionIndexForTad(int tadIndexForOriginal, int tadsForReduced,
                                       int tadsForOriginal) {
        if (tadIndexForOriginal == 0)
            return 0;
        return tadIndexForOriginal / (tadsForOriginal / tadsForReduced);
    }

/**
 * Tad index for linear
 * @param linearIndex
 * @param tadLength
 * @return
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tadIndexForLinear(int linearIndex, int tadLength) {
        return linearIndex % tadLength;
    }

/**
 * Computes the number of tads
 * per reduce index for the
 * reduction tad.
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int tadsPerReduceIndex(int tadsForReduce, int tadsForOriginal) {
        return tadsForOriginal / tadsForReduce;
    }

/**
 * Maps a linear index to a reduction index
 * @param i the linear index to map
 * @param elementWiseStride the element wise stride
 * for the multiple problem
 * @param tadNum the number of tads for the shrunken problem
 * @param originalTadNum the tad number for the reduced version of the problem
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int reductionIndexForLinear(int i, int elementWiseStride, int numElementsPerTad,
                                          int tadNum, int originalTadNum) {
        int tad = tadIndex(i, elementWiseStride, numElementsPerTad);
        return reductionIndexForTad(tad, tadNum, originalTadNum);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* createScalarShapeInfo() {

        traceNew(30);

        int *shape = new int[2];
        shape[0] = 1;
        shape[1] = 1;
        int *stride = new int[2];
        stride[0] = 1;
        stride[1] = 1;
        ShapeInformation *shapeInformation2 = new ShapeInformation();
        shapeInformation2->rank = 2;
        shapeInformation2->offset = 0;
        shapeInformation2->stride = stride;
        shapeInformation2->shape = shape;
        shapeInformation2->elementWiseStride = 1;
        shapeInformation2->order = 97;
        int *ret = shape::toShapeBuffer(shapeInformation2);
        delete shapeInformation2;
        delete[] shape;
        delete[] stride;
        return ret;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int* createScalarShapeInfo(int *ret) {
        ret[0] = 2;
        ret[1] = 1;
        ret[2] = 1;
        ret[3] = 1;
        ret[4] = 1;
        ret[5] = 0;
        ret[6] = 1;
        ret[7] = 99;

        return ret;
    }

/**
 * Returns the prod of the data
 * up to the given length
 */
#ifdef __CUDACC__
    __host__ __device__
#endif

    INLINEDEF int prod(int *data, int length) {
        int prod = 1;
        for (int i = 0; i < length; i++) {
            prod *= data[i];
        }

        return prod;
    }

/**
 * Returns the prod of the data
 * up to the given length
 */
#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF Nd4jIndex prodLong( int *data, int length) {
        Nd4jIndex prod = 1;
        for (int i = 0; i < length; i++) {
            prod *= data[i];
        }

        return prod;
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int rearMostLeftOverItem(int *data, int *dimension,int dimensionLength) {
        int *stride = shape::stride(data);
        //corner case: return the final item when its greater than the max, since its guaranteed to be left over
        //note here that strides are interpreted in reverse for tad
        //start from the front rather than the back

        int rank = shape::rank(data);


        if(shape::order(data) == 'f') {
            int dimIdx = dimensionLength - 1;
            for(int i = rank - 1; i >= 0; i--) {
                /**
                 * Needs to find an algorithm such that:
                 * looping backwards will find the highest dimension left
                 * that isn't included in the dimension index list.
                 *
                 * This can also be thought of as the last item of the first index
                 * of the difference between the full list of indices and
                 * the dimension indices.
                 *
                 * We should avoid excessive object creation by only looping backwards.
                 */
                if(dimension[dimIdx--] != i) {
                    int ret = stride[i];
                    return ret;
                }
            }
        }

        else {
            int dimIdx = dimensionLength - 1;

            for(int i = rank - 1; i >= 0; i--) {
                /**
                 * Needs to find an algorithm such that:
                 * looping backwards will find the highest dimension left
                 * that isn't included in the dimension index list.
                 *
                 * This can also be thought of as the last item of the first index
                 * of the difference between the full list of indices and
                 * the dimension indices.
                 *
                 * We should avoid excessive object creation by only looping backwards.
                 */
                if(dimension[dimIdx--] != i) {
                    int ret = stride[i];
                    return ret;
                }
            }
        }




        int ret = stride[0];
        return ret;
    }

#ifdef __CUDACC__
    __device__ INLINEDEF void sweepShapeInfoBuffer(int *shapeInfoBuffer, int *targetBuffer) {
    // we read first element, to find out length of our shapeInfoBuffer
    int rank = shapeInfoBuffer[0];
    int len = shape::shapeInfoLength(rank);
    for (int i = threadIdx.x; i < len; i += blockDim.x)
        targetBuffer[i] = shapeInfoBuffer[i];
}
#endif



#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferOfNpy(cnpy::NpyArray arr) {
      return shape::shapeBufferOfNpy(arr.shape.size(),(unsigned int* )arr.shape.data(),arr.fortranOrder);
    }





#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferOfNpyBuffer(char *buffer) {
        unsigned int *shape;
        unsigned int ndims, wordSize;
        bool fortranOrder;
        cnpy::parseNpyHeaderStr(std::string(buffer),wordSize,shape,ndims,fortranOrder);
        int * ret =  shape::shapeBufferOfNpy(ndims,shape,fortranOrder);
        delete[] shape;
        return ret;
    }


#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF int *shapeBufferOfNpy(int rank, unsigned int *shape,bool fortranOrder) {
        if(fortranOrder) {
            int *shapeBufferRet = shape::shapeBufferFortran(rank,(int *) shape);
            return shapeBufferRet;
        }
        else {
            int *newShape = new int[rank];
            for(int i = 0; i < rank; i++) {
                newShape[i] = shape[i];
            }

            int *shapeBufferRet = shape::shapeBuffer(rank,newShape);
            delete[] newShape;
            return shapeBufferRet;

        }
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    INLINEDEF bool strideDescendingCAscendingF(int *shapeBuffer) {
        int rank = shape::rank(shapeBuffer);
        int *strides = shape::stride(shapeBuffer);
        char order = shape::order(shapeBuffer);

        if (shape::isRowVector(shapeBuffer) && strides[0] == 1 && strides[1] == 1)
            return true;

        if (order == 'c') {
            for (int i = 1; i < rank; i++)
                if (strides[i-1] <= strides[i])
                    return false;
            return true;
        } else if (order == 'f') {
            for (int i = 1; i < rank; i++)
                if (strides[i-1] >= strides[i])
                    return false;
            return true;
        } else {
            printf("Unknown order for array!\n");
            return false;
        }
    }


#ifdef __CUDACC__
    __host__
#endif
    INLINEDEF bool reshapeCF(const int oldRank, int* oldShape, const int newRank, int* newShapeOf, bool isFOrder, int* target) {
        int oldnd;
        int* olddims = shape::copyOf(oldRank, shape::shapeOf(oldShape));
        int* oldstrides = shape::copyOf(oldRank, shape::stride(oldShape));
        int np, op, last_stride;
        int oi, oj, ok, ni, nj, nk;
        int* newStrides = new int[newRank];
        oldnd = 0;

        /*
         * Remove axes with dimension 1 from the old array. They have no effect
         * but would need special cases since their strides do not matter.
         */
        for (oi = 0; oi < oldRank; oi++) {
            if (shape::shapeOf(oldShape)[oi] != 1) {
                olddims[oldnd] = shape::shapeOf(oldShape)[oi];
                oldstrides[oldnd] = shape::stride(oldShape)[oi];
                oldnd++;
            }
        }

        np = 1;
        for (ni = 0; ni < newRank; ni++) {
            np *= newShapeOf[ni];
        }
        op = 1;
        for (oi = 0; oi < oldnd; oi++) {
            op *= olddims[oi];
        }
        if (np != op) {
            /* different total sizes; no hope */
            return false;
        }

        if (np == 0) {
            /* the current code does not handle 0-sized arrays, so give up */
            return false;
        }

        /* oi to oj and ni to nj give the axis ranges currently worked with */
        oi = 0;
        oj = 1;
        ni = 0;
        nj = 1;

        while (ni < newRank && oi < oldnd) {
            np = newShapeOf[ni];
            op = olddims[oi];

            while (np != op) {
                if (np < op) {
                    /* Misses trailing 1s, these are handled later */
                    np *= newShapeOf[nj++];
                } else {
                    op *= olddims[oj++];
                }
            }

            /* Check whether the original axes can be combined */
            for (ok = oi; ok < oj - 1; ok++) {
                if (isFOrder) {
                    if (oldstrides[ok + 1] != olddims[ok] * oldstrides[ok]) {
                        /* not contiguous enough */
                        return false;
                    }
                } else {
                    /* C order */
                    if (oldstrides[ok] != olddims[ok + 1] * oldstrides[ok + 1]) {
                        /* not contiguous enough */
                        return false;
                    }
                }
            }

            /* Calculate new strides for all axes currently worked with */
            if (isFOrder) {
                newStrides[ni] = oldstrides[oi];
                for (nk = ni + 1; nk < nj; nk++) {
                    newStrides[nk] = newStrides[nk - 1] * newShapeOf[nk - 1];
                }
            } else {
                /* C order */
                newStrides[nj - 1] = oldstrides[oj - 1];
                for (nk = nj - 1; nk > ni; nk--) {
                    newStrides[nk - 1] = newStrides[nk] * newShapeOf[nk];
                }
            }
            ni = nj++;
            oi = oj++;
        }

        if (ni >= 1) {
            last_stride = newStrides[ni - 1];
        } else {
            last_stride = shape::elementWiseStride(oldShape);
        }
        if (isFOrder && ni >= 1) {
            last_stride *= newShapeOf[ni - 1];
        }
        for (nk = ni; nk < newRank; nk++) {
            newStrides[nk] = last_stride;
        }

        target[0] = newRank;
        int cnt = 1;
        for (int e = 0; e < newRank; e++)
            target[cnt++] = newShapeOf[e];

        for (int e = 0; e < newRank; e++)
            target[cnt++] = newStrides[e];

        target[shape::shapeInfoLength(newRank) - 3] = 0;
        target[shape::shapeInfoLength(newRank) - 2] = -1;
        target[shape::shapeInfoLength(newRank) - 1] = isFOrder ? 102 : 99;

        return true;
    }

#ifdef __CUDACC__
    __host__
#endif
    INLINEDEF bool canReshape(const int oldRank, int* oldShape, const int newRank, int* newShapeOf, bool isFOrder) {
        int oldnd;
        int* olddims = shape::copyOf(oldRank, shape::shapeOf(oldShape));
        int* oldstrides = shape::copyOf(oldRank, shape::stride(oldShape));
        int np, op, last_stride;
        int oi, oj, ok, ni, nj, nk;
        int* newStrides = new int[newRank];
        oldnd = 0;

        /*
         * Remove axes with dimension 1 from the old array. They have no effect
         * but would need special cases since their strides do not matter.
         */
        for (oi = 0; oi < oldRank; oi++) {
            if (shape::shapeOf(oldShape)[oi] != 1) {
                olddims[oldnd] = shape::shapeOf(oldShape)[oi];
                oldstrides[oldnd] = shape::stride(oldShape)[oi];
                oldnd++;
            }
        }

        np = 1;
        for (ni = 0; ni < newRank; ni++) {
            np *= newShapeOf[ni];
        }
        op = 1;
        for (oi = 0; oi < oldnd; oi++) {
            op *= olddims[oi];
        }
        if (np != op) {
            /* different total sizes; no hope */
            return false;
        }

        if (np == 0) {
            /* the current code does not handle 0-sized arrays, so give up */
            return false;
        }

        /* oi to oj and ni to nj give the axis ranges currently worked with */
        oi = 0;
        oj = 1;
        ni = 0;
        nj = 1;

        while (ni < newRank && oi < oldnd) {
            np = newShapeOf[ni];
            op = olddims[oi];

            while (np != op) {
                if (np < op) {
                    /* Misses trailing 1s, these are handled later */
                    np *= newShapeOf[nj++];
                } else {
                    op *= olddims[oj++];
                }
            }

            /* Check whether the original axes can be combined */
            for (ok = oi; ok < oj - 1; ok++) {
                if (isFOrder) {
                    if (oldstrides[ok + 1] != olddims[ok] * oldstrides[ok]) {
                        /* not contiguous enough */
                        return false;
                    }
                } else {
                    /* C order */
                    if (oldstrides[ok] != olddims[ok + 1] * oldstrides[ok + 1]) {
                        /* not contiguous enough */
                        return false;
                    }
                }
            }

            /* Calculate new strides for all axes currently worked with */
            if (isFOrder) {
                newStrides[ni] = oldstrides[oi];
                for (nk = ni + 1; nk < nj; nk++) {
                    newStrides[nk] = newStrides[nk - 1] * newShapeOf[nk - 1];
                }
            } else {
                /* C order */
                newStrides[nj - 1] = oldstrides[oj - 1];
                for (nk = nj - 1; nk > ni; nk--) {
                    newStrides[nk - 1] = newStrides[nk] * newShapeOf[nk];
                }
            }
            ni = nj++;
            oi = oj++;
        }


        return true;
    }

#ifdef __CUDACC__
    __host__
#endif
    // this function checks the consistence of dimensions with array rank (negative dimensions, too large dimensions, too big number of dimensions)    
    // also sort input array of dimensions, this operation is also necessary for creating TAD object 
    INLINEDEF void checkDimensions(const int rank, std::vector<int>& dimensions) {

        int dimSize = dimensions.size();
        if(dimSize == 0)
            throw "shape::checkDimensions method: array of dimensions is empty!";        
        // check presence of negative dimensions and if they are present transform them to positive ones -dim -> rank - |dim|                
        for(auto& dim : dimensions)
            if(dim < 0)
                dim += rank;
		// sort input array of dimensions, this operation is also necessary for creating TAD object in external methods
        if (dimSize > 1) {
            std::sort(dimensions.begin(), dimensions.end());		
            // remove duplicates if they are present
            dimensions.erase(std::unique(dimensions.begin(), dimensions.end()), dimensions.end());
        }
        // check whether number of dimensions is to big (>rank)
        dimSize = dimensions.size();
        if(dimSize > rank)
            throw "shape::checkDimensions method: number of input dimensions is too big ( > rank of array) !";
        // check if min dimension is still negative and whether max dimension is bigger then rank-1
        if(dimensions[0] < 0 || dimensions.back() > (rank-1))
            throw "shape::checkDimensions method: the negative dimension is still present in input array after transform or the too big dimension is present ( > rank of array) !";

        return;
    }





}





#endif /* SHAPE_H_ */
