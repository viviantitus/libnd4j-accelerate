//
// @author raver119@gmail.com
//

#ifndef LIBND4J_SPECIAL_ACCUMULATION_OPS_H
#define LIBND4J_SPECIAL_ACCUMULATION_OPS_H

#include <templatemath.h>
//#include <ops/ops.h>
//#include <loops/reduce.h>

namespace simdOps {

    template<typename T>
    class LogSumExp {
    public:
        static const bool requiresSpecialAccumulation = true;


        op_def static T startingValue(const T *input) {
            return (T) 0.0f;
        }

        op_def static T merge(T old, T opOutput, T *extraParams) {
            return opOutput + old;
        }

        op_def static T update(T old, T opOutput, T *extraParams) {
            return opOutput + old;
        }

        op_def static T op(T d1, T d2) {
            return nd4j::math::nd4j_exp<T>(d1 - d2);
        }

        op_def static T op(T d1, T* extraParams) {
            return nd4j::math::nd4j_exp<T>(d1 - extraParams[0]);
        }

        op_def static T postProcess(T reduction, Nd4jIndex n, T *extraParams) {
            return extraParams[0] + nd4j::math::nd4j_log<T>(reduction);
        }

#ifdef __CUDACC__
        __device__ static inline void aggregatePartials(T *sPartials, int tid, int numItems, T *extraParams) {
            // start the shared memory loop on the next power of 2 less
            // than the block size.  If block size is not a power of 2,
            // accumulate the intermediate sums in the remainder range.
            int floorPow2 = numItems;

            if (floorPow2 & (floorPow2 - 1)) {
                while (floorPow2 & (floorPow2 - 1)) {
                    floorPow2 &= floorPow2 - 1;
                }
                if (tid >= floorPow2) {
                    sPartials[tid - floorPow2] = update(sPartials[tid - floorPow2], sPartials[tid], extraParams);
                }

                __syncthreads();
            }


            for (int activeThreads = floorPow2 >> 1; activeThreads; activeThreads >>= 1) {
                if (tid < activeThreads && tid + activeThreads < numItems) {
                    sPartials[tid] = update(sPartials[tid], sPartials[tid + activeThreads], extraParams);
                }
                __syncthreads();
            }
        }

        static inline __device__ void execSpecialCuda(
				T *dx,
				int *xShapeInfo,
				T *extraParams,
				T *result,
				int *resultShapeInfo,
				int *dimension,
				int dimensionLength,
				T *reductionBuffer,
				UnifiedSharedMemory *manager,
				int *tadOnlyShapeInfo,
				Nd4jIndex *tadOffsets) {

				// we assume that RESULT already holds max values

				//shared memory space for storing intermediate results
				__shared__ T *sPartials;

				//                __shared__ shape::TAD *tad;
				__shared__ int tadLength;
				__shared__ int tadRank;
				__shared__ int numTads;
				__shared__ int *tadShape;
				__shared__ int *tadStride;
				if (threadIdx.x == 0) {
				    extern __shared__ unsigned char shmem[];
				    sPartials = (T *) shmem;
					tadLength = shape::tadLength(xShapeInfo, dimension, dimensionLength);
					tadRank = shape::rank(tadOnlyShapeInfo);
					numTads = shape::length(xShapeInfo) / tadLength;

					tadShape = shape::shapeOf(tadOnlyShapeInfo);
					tadStride = shape::stride(tadOnlyShapeInfo);
				}
				__syncthreads();

				int xCoord[MAX_RANK];

				for (int r = blockIdx.x; r < numTads; r += gridDim.x) {
					Nd4jIndex tadOffsetForBlock = tadOffsets[r];

					sPartials[threadIdx.x] = startingValue(dx + tadOffsetForBlock);

					for (int i = threadIdx.x; i < tadLength; i += blockDim.x) {
						shape::ind2subC(tadRank, tadShape, i, xCoord);
						Nd4jIndex xOffset = shape::getOffset(tadOffsetForBlock, tadShape, tadStride, xCoord, tadRank);

						sPartials[threadIdx.x] = update(sPartials[threadIdx.x], op(dx[xOffset], result[r]), extraParams);
					}
					__syncthreads();

					// aggregate. do NOT reduce for elements > tadLength
					aggregatePartials(sPartials, threadIdx.x, nd4j::math::nd4j_min<int>(blockDim.x, tadLength), &result[r]);


					__syncthreads();
					if (threadIdx.x == 0)
						result[r] = postProcess(sPartials[threadIdx.x], tadLength, &result[r]);
				}
			}
#endif

        static void execSpecial(T *x,
                         int *xShapeInfo,
                         T *extraParams,
                         T *result,
                         int *resultShapeInfoBuffer,
                         int *dimension,
                         int dimensionLength,
                         int *tadShapeInfo,
                         Nd4jIndex *tadOffset) {
            int resultLength = shape::length(resultShapeInfoBuffer);

            int *tadOnlyShapeInfo = tadShapeInfo;
            Nd4jIndex *tadOffsets = tadOffset;
            shape::TAD *tad = nullptr;

            if (tadOnlyShapeInfo == nullptr || tadOffsets == nullptr) {
                tad = new shape::TAD(xShapeInfo, dimension, dimensionLength);
                tad->createTadOnlyShapeInfo();
                tad->createOffsets();

                if (tad->dimensionLength < 1) {
                    delete tad;
                    return;
                }

                tadOnlyShapeInfo = tad->tadOnlyShapeInfo;
                tadOffsets = tad->tadOffsets;
            }


            const int tadLength = shape::tadLength(xShapeInfo, dimension, dimensionLength);
            int numTads = shape::length(xShapeInfo) / tadLength;
            int tadEWS = shape::elementWiseStride(tadOnlyShapeInfo);

            int tadsPerThread = resultLength / TAD_THRESHOLD;
            int num_threads = nd4j::math::nd4j_max<int>(1, tadsPerThread);
            num_threads = nd4j::math::nd4j_min<int>(num_threads, omp_get_max_threads());

            if (tadEWS > 0 && (numTads == 1 || shape::isVector(tadOnlyShapeInfo) || shape::isScalar(tadOnlyShapeInfo))) {

#pragma omp parallel for schedule(guided) num_threads(num_threads) if (num_threads > 1) proc_bind(AFFINITY) default(shared)
                for (int i = 0; i < resultLength; i++) {

                    T *iter = x + tadOffsets[i];
                    T start = startingValue(iter);
                    if (tadEWS == 1) {
                        for (int j = 0; j < tadLength; j++) {
                            start = update(start, op(iter[j], result[i]), extraParams);

                        }
                    }
                    else {
                        for (int j = 0; j < tadLength; j++) {
                            start = update(start, op(iter[j * tadEWS], result[i]), extraParams);
                        }
                    }
                    result[i] = postProcess(start, tadLength, &result[i]);
                }
            }
            else {
                int *tadShape = shape::shapeOf(tadOnlyShapeInfo);
                int *tadStride = shape::stride(tadOnlyShapeInfo);
                int tadRank = shape::rank(tadOnlyShapeInfo);

#pragma omp  parallel for schedule(guided) num_threads(num_threads) if (num_threads > 1) proc_bind(AFFINITY) default(shared)
                for (int i = 0; i < resultLength; i++) {

                    Nd4jIndex offset = tadOffsets[i];
                    int xCoord[MAX_RANK];

                    T start = startingValue(x + offset);

                    for (int j = 0; j < tadLength; j++) {
                        shape::ind2subC(tadRank, tadShape, j, xCoord);
                        Nd4jIndex xOffset = shape::getOffset(offset, tadShape, tadStride, xCoord, tadRank);

                        printf("C I: %i; V: %f; op: %f\n", i, x[xOffset], op(x[xOffset], result[i]));

                        start = update(start, op(x[xOffset], result[i]), extraParams);
                    }

                    result[i] = postProcess(start, tadLength, &result[i]);;
                }
            }

            if (tad != nullptr)
                delete tad;

        }
    };
}

#endif //LIBND4J_SPECIAL_ACCUMULATION_OPS_H
