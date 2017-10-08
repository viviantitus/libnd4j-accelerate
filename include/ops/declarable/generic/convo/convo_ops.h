//
// 3D convolutions are based on pytorch - https://github.com/pytorch/pytorch
//

#ifndef LIBND4J_CONVO_OPS_H
#define LIBND4J_CONVO_OPS_H

#include <memory>
#include <NDArray.h>
#include <NDArrayFactory.h>
#include <op_boilerplate.h>
#include <declarable/declarable_ops.h>
#include <declarable/generic/helpers/convolutions.h>
#include <helpers/ArrayUtils.h>

namespace nd4j {
    namespace ops {
        //////////////////////////////////////////////////////////////////////////
        DECLARE_CUSTOM_OP(conv2d, 2, 1, false, 0, 9) {
            // basically im2col + gemm
            NDArray<T>* input = block.getVariables().at(0)->getNDArray();
            NDArray<T>* weights = block.getVariables().at(1)->getNDArray();
            NDArray<T>* bias = nullptr;

            if (block.getVariables().size() == 3)
                bias = block.getVariables().at(2)->getNDArray();

            const int kY = block.getIArguments()->at(0);
            const int kX = block.getIArguments()->at(1);
            const int sY = block.getIArguments()->at(2);
            const int sX = block.getIArguments()->at(3);
            int pY = block.getIArguments()->at(4);
            int pX = block.getIArguments()->at(5);
            const int dY = block.getIArguments()->at(6);
            const int dX = block.getIArguments()->at(7);
            const bool isSameMode = block.getIArguments()->at(8) != 0;

            int oY = 0;
            int oX = 0;

            const int batchSize = input->shapeOf()[0];
            const int outDepth = weights->shapeOf()[0];
            const int inDepth = weights->shapeOf()[1];
            const int inY = input->shapeOf()[2];
            const int inX = input->shapeOf()[3];

            REQUIRE_TRUE(weights->shapeOf()[2] == kY && weights->shapeOf()[3] == kX, 0, "Kernels should have dimensions of [%i, %i], but got [%i, %i] instead", kY, kX, weights->sizeAt(2), weights->sizeAt(3));

            nd4j::ops::calcOutHWpool2D(oY, oX, kY, kX, sY, sX, pY, pX, dY, dX, inY, inX, isSameMode);

            if (isSameMode) {
                nd4j::ops::_calcPadding2D(pY, pX, oY, oX, inY, inX, kY, kX, sY, sX, dY, dX);
            }

            NDArray<T>* output = this->getZ(block);

            Nd4jIndex prod = batchSize * outDepth * oY * oX;
            REQUIRE_TRUE(output->lengthOf() == prod, 0, "Z should have total length of %i, but got %i instead", prod, output->lengthOf());

            //INDArray col = Nd4j.createUninitialized(new int[] {miniBatch, outH, outW, inDepth, kH, kW}, 'c');
            std::unique_ptr<NDArray<T>> col(new NDArray<T>('c', {batchSize, oY, oX, inDepth, kY, kX}));
            std::unique_ptr<NDArray<T>> col2(col.get()->permute({0, 3, 4, 5, 1, 2}));

//            std::unique_ptr<NDArray<T>> col2(new NDArray<T>('c', {batchSize, inDepth, kY, kX, oY, oX }));

            std::unique_ptr<T> extrasIm2Col(new T[9]{(T) kY, (T) kX, (T) sY, (T) sX, (T) pY, (T) pX, (T) dY, (T) dX, isSameMode ? (T) 1.0f : (T) 0.0f});

            input->template applyTransform<simdOps::Im2col<T>>(col2.get(), extrasIm2Col.get());

            std::unique_ptr<NDArray<T>> im2col2d(col->reshape('c', {batchSize * oY * oX, inDepth * kY * kX}));
            std::unique_ptr<NDArray<T>> permutedW(weights->permute({3, 2, 1, 0}));
            std::unique_ptr<NDArray<T>> reshapedW(permutedW.get()->reshape('f', {kX * kY * inDepth, outDepth}));

            output->reshapei('f', {im2col2d.get()->rows(), reshapedW.get()->columns()});

            auto res = NDArrayFactory::mmulHelper<T>(im2col2d.get(), reshapedW.get(), output, 1.0, 0.0);

            // bias addition is optional
            if (bias != nullptr)
                output->addiRowVector(bias);

            output->reshapei('f', {oX, oY, input->sizeAt(0),outDepth});
            output->permutei({2, 3, 1, 0});

            if (debug && verbose)
                output->printShapeInfo("Conv2D result shape");

            STORE_RESULT(*output);

            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(conv2d) {
            auto inShape = inputShape->at(0);
            auto wShape = inputShape->at(1);

            const int kY = block.getIArguments()->at(0);
            const int kX = block.getIArguments()->at(1);
            const int sY = block.getIArguments()->at(2);
            const int sX = block.getIArguments()->at(3);
            int pY = block.getIArguments()->at(4);
            int pX = block.getIArguments()->at(5);
            const int dY = block.getIArguments()->at(6);
            const int dX = block.getIArguments()->at(7);
            const bool isSameMode = block.getIArguments()->at(8) != 0;

            int oY = 0;
            int oX = 0;

            const int batchSize = inShape[1];
            const int outDepth = wShape[1];
            const int inY = inShape[3];
            const int inX = inShape[4];

            nd4j::ops::calcOutHWpool2D(oY, oX, kY, kX, sY, sX, pY, pX, dY, dX, inY, inX, isSameMode);

            if (isSameMode) {
                nd4j::ops::_calcPadding2D(pY, pX, oY, oX, inY, inX, kY, kX, sY, sX, dY, dX);
            }

            //z = Shape.newShapeNoCopy(z, new int[] {outW, outH, miniBatch, outDepth}, true);
            int *newShape;
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(4), int);
            std::vector<int> shape({batchSize, outDepth, oY, oX});
            shape::shapeBuffer(4, shape.data(), newShape);

            return new ShapeList(newShape);
        }

        /**
         * Depthwise convolution2d
         */
//////////////////////////////////////////////////////////////////////////
        DECLARE_CUSTOM_OP(sconv2d, 2, 1, false, 0, 9) {
            NDArray<T> *input = block.getVariables().at(0)->getNDArray();
            NDArray<T> *weights = block.getVariables().at(1)->getNDArray();
            NDArray<T> *bias = nullptr;
            if (block.getVariables().size() == 3)
                bias = block.getVariables().at(2)->getNDArray();

            NDArray<T> *z = this->getZ(block);

            const int kY = block.getIArguments()->at(0);
            const int kX = block.getIArguments()->at(1);
            const int sY = block.getIArguments()->at(2);
            const int sX = block.getIArguments()->at(3);
            int pY = block.getIArguments()->at(4);
            int pX = block.getIArguments()->at(5);
            const int dY = block.getIArguments()->at(6);
            const int dX = block.getIArguments()->at(7);
            const bool isSameMode = block.getIArguments()->at(8) != 0;

            int oY = 0;
            int oX = 0;

            const int batchSize = input->shapeOf()[0];
            const int outDepth = weights->shapeOf()[0];
            const int inDepth = weights->shapeOf()[1];
            const int inY = input->shapeOf()[2];
            const int inX = input->shapeOf()[3];

            REQUIRE_TRUE(weights->shapeOf()[2] == kY && weights->shapeOf()[3] == kX, 0, "Kernels should have dimensions of [%i, %i], but got [%i, %i] instead", kY, kX, weights->sizeAt(2), weights->sizeAt(3));

            if (input->sizeAt(1) == 1) {
                nd4j_debug("Separable conv2d for 1 channel equals to standard conv2d\n","");
                nd4j::ops::conv2d<T> c2d;
                return c2d.execute(&block);
            }

            nd4j::ops::calcOutHWpool2D(oY, oX, kY, kX, sY, sX, pY, pX, dY, dX, inY, inX, isSameMode);

            if (isSameMode) {
                nd4j::ops::_calcPadding2D(pY, pX, oY, oX, inY, inX, kY, kX, sY, sX, dY, dX);
            }

            std::unique_ptr<NDArray<T>> col2(new NDArray<T>('c', {batchSize, inDepth, kY, kX, oY, oX}));

            // col2d now has shape of [bS, inDepth, kY, kX, oY, oX]
            std::unique_ptr<T> extrasIm2Col(new T[9]{(T) kY, (T) kX, (T) sY, (T) sX, (T) pY, (T) pX, (T) dY, (T) dX, isSameMode ? (T) 1.0f : (T) 0.0f});

            input->template applyTransform<simdOps::Im2col<T>>(col2.get(), extrasIm2Col.get());

            NDArray<T>* c_ = col2.get()->permute({1, 0, 4, 5, 2, 3});
            NDArray<T>* w_ = weights->permute({1, 2, 3, 0});

            c_->reshapei('c', {inDepth, batchSize * oY * oX, kY * kX});
            w_->reshapei('c', {inDepth, kY * kX, outDepth});

            // matmul here
            z->reshapei('c', {inDepth, batchSize * oY * oX, outDepth});
            NDArrayFactory::mmulHelper<T>(c_, w_, z);

            if (bias != nullptr) {
                z->reshapei('c', {-1, (int) bias->lengthOf()});
                z->addiRowVector(bias);
            }

            z->reshapei('c', {input->sizeAt(0),outDepth * inDepth, oY, oX });


            STORE_RESULT(*z);

            delete c_;
            delete w_;

            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(sconv2d) {
            auto inShape = inputShape->at(0);
            auto wShape = inputShape->at(1);

            const int kY = block.getIArguments()->at(0);
            const int kX = block.getIArguments()->at(1);
            const int sY = block.getIArguments()->at(2);
            const int sX = block.getIArguments()->at(3);
            int pY = block.getIArguments()->at(4);
            int pX = block.getIArguments()->at(5);
            const int dY = block.getIArguments()->at(6);
            const int dX = block.getIArguments()->at(7);
            const bool isSameMode = block.getIArguments()->at(8) != 0;

            int oY = 0;
            int oX = 0;

            const int batchSize = inShape[1];
            const int inDepth = inShape[2];
            const int outDepth = wShape[1];
            const int inY = inShape[3];
            const int inX = inShape[4];

            nd4j::ops::calcOutHWpool2D(oY, oX, kY, kX, sY, sX, pY, pX, dY, dX, inY, inX, isSameMode);

            if (isSameMode) {
                nd4j::ops::_calcPadding2D(pY, pX, oY, oX, inY, inX, kY, kX, sY, sX, dY, dX);
            }

            //z = Shape.newShapeNoCopy(z, new int[] {outW, outH, miniBatch, outDepth}, true);
            int *newShape;
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(4), int);
            std::vector<int> shape({batchSize, outDepth * inDepth, oY, oX});
            shape::shapeBuffer(4, shape.data(), newShape);

            return new ShapeList(newShape);
        }

        /**
         *
         *
         */
        DECLARE_CUSTOM_OP(sconv2d_bp, 4, 2, false, 0, 9) {
            NDArray<T> *input = INPUT_VARIABLE(0);
            NDArray<T> *weights = INPUT_VARIABLE(1);
            NDArray<T> *epsilonNext = INPUT_VARIABLE(2);
            NDArray<T> *bias = nullptr;

            // bias is still optional
            if (block.getVariables().size() > 3)
                bias = INPUT_VARIABLE(3);

            //epsilonNext->rankOf() == 4 && weights->rankOf() == 4
            REQUIRE_TRUE(input->rankOf() == 4, 0, "Input should be 4D, but got %iD instead", input->rankOf());
            REQUIRE_TRUE(weights->rankOf() == 4, 0, "Weights should be 4D, but got %iD instead", weights->rankOf());
            REQUIRE_TRUE(epsilonNext->rankOf() == 4, 0, "Epsilon should be 4D, but got %iD instead", epsilonNext->rankOf());

            NDArray<T> * epsilon = this->getZ(block);
            NDArray<T> * gradW = this->getZ(block, 1);
            NDArray<T> * gradB = nullptr;
            if (bias != nullptr)
                gradB = this->getZ(block, 2);

            const int kY = block.getIArguments()->at(0);
            const int kX = block.getIArguments()->at(1);
            const int sY = block.getIArguments()->at(2);
            const int sX = block.getIArguments()->at(3);
            int pY = block.getIArguments()->at(4);
            int pX = block.getIArguments()->at(5);
            const int dY = block.getIArguments()->at(6);
            const int dX = block.getIArguments()->at(7);
            const bool isSameMode = block.getIArguments()->at(8) != 0;

            int oY = epsilonNext->sizeAt(2);
            int oX = epsilonNext->sizeAt(3);

            const int batchSize = input->shapeOf()[0];
            const int outDepth = weights->shapeOf()[0];
            const int inDepth = weights->shapeOf()[1];
            const int inY = input->shapeOf()[2];
            const int inX = input->shapeOf()[3];

            bool hasCol = CHECK_STASH("im2col");
            NDArray<T> *col = nullptr;
            if (hasCol)
                col = UNSTASH("im2col")
            else {
                std::unique_ptr<NDArray<T>> col2(new NDArray<T>('c', {batchSize, inDepth, kY, kX, oY, oX}));

                // col2d now has shape of [bS, inDepth, kY, kX, oY, oX]
                std::unique_ptr<T> extrasIm2Col(new T[9]{(T) kY, (T) kX, (T) sY, (T) sX, (T) pY, (T) pX, (T) dY, (T) dX, isSameMode ? (T) 1.0f : (T) 0.0f});

                input->template applyTransform<simdOps::Im2col<T>>(col2.get(), extrasIm2Col.get());
            }

//            epsilonNext->printShapeInfo("eps next");

            /*
             gy_ = gy.reshape((B, C, D, IY * IX)).transpose(1, 2, 0, 3).reshape((C, D, B * IY * IX))
             */
            auto eN_ = epsilonNext->reshape('c', {batchSize, inDepth, outDepth, oY * oX});
            eN_->permutei({1, 2, 0, 3});
            eN_->reshapei('c', {inDepth, outDepth, batchSize * oY * oX});

            auto col_ = col->permute({1, 0, 4, 5, 2, 3});
            col_->reshapei('c', {inDepth, batchSize * oY * oX, kY * kX});

            /*
             # (C, D, B*IY*IX), (C, B*IY*IX, KY*KX) -> (C, D, KY*KX)
                gW_ = _matmul(gy_, c_, xp)
             */

            // calculating wieghts gradients here
            //auto gW_ = gradW->reshape('c', {inDepth, outDepth, kY * kX});
            auto gW_ = NDArrayFactory::mmulHelper(eN_, col_);

            gW_->reshapei('c',{inDepth, outDepth, kY, kX});
            gW_->permutei({1, 0, 2, 3});
            gradW->assign(gW_);

            delete gW_;
            delete col_;
            if (!hasCol)
                delete col;

            // calculating epsilon here
            auto w_ = weights->permute({1, 2, 3, 0});
            w_->reshapei('c', {inDepth, kY * kX, outDepth});

            auto gcol = NDArrayFactory::mmulHelper(w_, eN_);
            gcol->reshapei('c', {inDepth, kY, kX, batchSize, oY, oX});
            gcol->permutei({3, 0, 1, 2, 4, 5});

            std::unique_ptr<T> extrasCol2Im(new T[9]{(T) sY, (T) sX, (T) pY, (T) pX, (T) inY, (T) inX, (T) dY, (T) dX, isSameMode ? (T) 1.0f : (T) 0.0f});

            // we're sure that col2im result will have the same size as original image
            //auto rCol = new NDArray<T>('c', {batchSize, inDepth, inY, inX});
            gcol->template applyTransform<simdOps::Col2Im<T>>(epsilon, extrasCol2Im.get());


            delete eN_;
            delete gcol;
            delete w_;



            if (bias != nullptr) {
                // calculating gradB, if defined
                auto eN_ = epsilonNext->permute({0, 2, 3, 1});
                auto sum = eN_->template reduceAlongDimension<simdOps::Sum<T>>({0, 1, 2});
                gradB->assign(sum);
                delete sum;

                STORE_3_RESULTS(*epsilon, *gradW, *gradB);
            } else {
                STORE_2_RESULTS(*epsilon, *gradW);
            }

            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(sconv2d_bp) {
            auto inShape = inputShape->at(0);
            auto wShape = inputShape->at(1);
            auto eShape = inputShape->at(2);
            int *bShape = nullptr;

            // bias is optional thing, and might be absent
            if (inputShape->size() == 4)
                bShape = inputShape->at(3);

            int *newInShape;
            int *newWShape;
            ALLOCATE(newInShape, block.getWorkspace(), shape::shapeInfoLength(inShape), int);
            ALLOCATE(newWShape, block.getWorkspace(), shape::shapeInfoLength(wShape), int);

            memcpy(newInShape, inShape, shape::shapeInfoByteLength(inShape));
            memcpy(newWShape, wShape, shape::shapeInfoByteLength(wShape));

            auto shapes = new ShapeList({newInShape, newWShape});

            if (bShape != nullptr) {
                int *newBShape;
                ALLOCATE(newBShape, block.getWorkspace(), shape::shapeInfoLength(bShape), int);
                memcpy(newBShape, bShape, shape::shapeInfoByteLength(bShape));

                shapes->push_back(newBShape);
            }

            return shapes;
        }

//////////////////////////////////////////////////////////////////////////
        DECLARE_CUSTOM_OP(deconv2d, 2, 1, false, 0, 9) {
            NDArray<T>* input = INPUT_VARIABLE(0);
            NDArray<T>* weights = INPUT_VARIABLE(1);
            NDArray<T>* bias = nullptr;
            if (block.getVariables().size() > 2)
                bias = INPUT_VARIABLE(2);

            REQUIRE_TRUE(input->rankOf() == 4, 0, "Input should be 4D, but got %iD instead", input->rankOf());
            REQUIRE_TRUE(weights->rankOf() == 4, 0, "Weights should be 4D, but got %iD instead", weights->rankOf());

            int oD = weights->sizeAt(0);

            if (bias != nullptr) {
                REQUIRE_TRUE(bias->isVector(), 0, "Bias should be vector");
                REQUIRE_TRUE(bias->lengthOf() == oD, 0, "Bias length be equal to outpuDepth, but got %i instead", bias->lengthOf());
            }

            int iY = input->sizeAt(2);
            int iX = input->sizeAt(3);

            int kY = block.getIArguments()->at(0);
            int kX = block.getIArguments()->at(1);
            int sY = block.getIArguments()->at(2);
            int sX = block.getIArguments()->at(3);
            int pY = block.getIArguments()->at(4);
            int pX = block.getIArguments()->at(5);
            int dY = block.getIArguments()->at(6);
            int dX = block.getIArguments()->at(7);
            const bool isSameMode = block.getIArguments()->at(8) != 0;

            NDArray<T> *z = this->getZ(block);

            int oY = z->sizeAt(2);
            int oX = z->sizeAt(3);

            auto gcol = nd4j::NDArrayFactory::tensorDot<T>(weights, input, nullptr, {0}, {1});
            gcol->permutei({3, 0, 1, 2, 4, 5});

            std::unique_ptr<T> extrasCol2Im(new T[9]{(T) sY, (T) sX, (T) pY, (T) pX, (T) oY, (T) oX, (T) dY, (T) dX, isSameMode ? (T) 1.0f : (T) 0.0f});

            gcol->template applyTransform<simdOps::Col2Im<T>>(z, extrasCol2Im.get());

            delete gcol;

            if (bias != nullptr) {
                z->template applyBroadcast<simdOps::Add<double>>({1}, bias);
            }

            STORE_RESULT(*z);

            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(deconv2d) {
            auto inShape = inputShape->at(0);
            auto wShape = inputShape->at(1);

            int B = shape::shapeOf(inShape)[0];
            int iC = shape::shapeOf(inShape)[1];
            int iY = shape::shapeOf(inShape)[2];
            int iX = shape::shapeOf(inShape)[3];

            int oC = shape::shapeOf(wShape)[0];
            int kY = block.getIArguments()->at(0);
            int kX = block.getIArguments()->at(1);
            int sY = block.getIArguments()->at(2);
            int sX = block.getIArguments()->at(3);
            int pY = block.getIArguments()->at(4);
            int pX = block.getIArguments()->at(5);

            int oY = sY * (iY - 1) + kY - 2 * pY;
            int oX = sX * (iX - 1) + kX - 2 * pX;

            int *newShape;
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(4), int);
            std::vector<int> shape({B, oC, oY, oX});
            shape::shapeBuffer(4, shape.data(), newShape);

            return new ShapeList(newShape);
        }


        DECLARE_CUSTOM_OP(deconv2d_bp, 4, 2, false, 0, 9) {
            NDArray<T> *input = INPUT_VARIABLE(0);
            NDArray<T> *weights = INPUT_VARIABLE(1);
            NDArray<T> *epsilonNext = INPUT_VARIABLE(2);
            NDArray<T> *bias = nullptr;

            // bias is still optional
            if (block.getVariables().size() > 3)
                bias = INPUT_VARIABLE(3);

            //epsilonNext->rankOf() == 4 && weights->rankOf() == 4
            REQUIRE_TRUE(input->rankOf() == 4, 0, "Input should be 4D, but got %iD instead", input->rankOf());
            REQUIRE_TRUE(weights->rankOf() == 4, 0, "Weights should be 4D, but got %iD instead", weights->rankOf());
            REQUIRE_TRUE(epsilonNext->rankOf() == 4, 0, "Epsilon should be 4D, but got %iD instead", epsilonNext->rankOf());

            int kY = block.getIArguments()->at(0);
            int kX = block.getIArguments()->at(1);
            int sY = block.getIArguments()->at(2);
            int sX = block.getIArguments()->at(3);
            int pY = block.getIArguments()->at(4);
            int pX = block.getIArguments()->at(5);
            int dY = block.getIArguments()->at(6);
            int dX = block.getIArguments()->at(7);
            const bool isSameMode = block.getIArguments()->at(8) != 0;

            NDArray<T>* epsilon = this->getZ(block);
            NDArray<T>* gradW = this->getZ(block, 1);
            NDArray<T>* gradB = nullptr;

            if (bias != nullptr)
                gradB = this->getZ(block, 2);

            // epsilon for deconv2d is FF conv pass

            nd4j::ops::conv2d<T> op;
            Nd4jStatus r1 = op.execute({input, weights}, {epsilon}, {}, {kY, kX, sY, sX, pY, pX, dY, dX, block.getIArguments()->at(8)});
            if (r1 != ND4J_STATUS_OK)
                return r1;

            // gradW is im2col + tensorDot
            /*
              col = conv.im2col_cpu(
            x, self.kh, self.kw, self.sy, self.sx, self.ph, self.pw,
            cover_all=self.cover_all)
             gW = numpy.tensordot(gy, col, ((0, 2, 3), (0, 4, 5))).
             */

            int oY = 0;
            int oX = 0;
            int inY = epsilonNext->sizeAt(2);
            int inX = epsilonNext->sizeAt(3);

            nd4j::ops::calcOutHWpool2D(oY, oX, kY, kX, sY, sX, pY, pX, dY, dX, inY, inX, isSameMode);

            if (isSameMode) {
                nd4j::ops::_calcPadding2D(pY, pX, oY, oX, inY, inX, kY, kX, sY, sX, dY, dX);
            }

            std::unique_ptr<T> extrasIm2Col(new T[9]{(T) kY, (T) kX, (T) sY, (T) sX, (T) pY, (T) pX, (T) dY, (T) dX, isSameMode ? (T) 1.0f : (T) 0.0f});
            auto gcol = new NDArray<T>('c', {input->sizeAt(0), input->sizeAt(1), kY, kX, oY, oX });
            epsilonNext->template applyTransform<simdOps::Im2col<T>>(gcol, extrasIm2Col.get());

            /*
            gW = numpy.tensordot(
                    gy, col, ((0, 2, 3), (0, 4, 5))).
            */

            auto gW = NDArrayFactory::tensorDot<T>(input, gcol, nullptr, {0, 2, 3}, {0, 4, 5});
            gradW->assign(gW);

            delete gW;
            delete gcol;

            if (gradB != nullptr) {
                auto sum = epsilon->template reduceAlongDimension<simdOps::Sum<T>>({0, 2, 3});
                gradB->assign(sum);
                delete sum;

                STORE_3_RESULTS(*epsilon, *gradW, *gradB);
            } else {
                STORE_2_RESULTS(*epsilon, *gradW);
            }



            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(deconv2d_bp) {
            auto inShape = inputShape->at(0);
            auto wShape = inputShape->at(1);
            auto eShape = inputShape->at(2);
            int *bShape = nullptr;

            // bias is optional thing, and might be absent
            if (inputShape->size() == 4)
                bShape = inputShape->at(3);

            int *newInShape;
            int *newWShape;
            ALLOCATE(newInShape, block.getWorkspace(), shape::shapeInfoLength(inShape), int);
            ALLOCATE(newWShape, block.getWorkspace(), shape::shapeInfoLength(wShape), int);

            memcpy(newInShape, inShape, shape::shapeInfoByteLength(inShape));
            memcpy(newWShape, wShape, shape::shapeInfoByteLength(wShape));

            auto shapes = new ShapeList({newInShape, newWShape});

            if (bShape != nullptr) {
                int *newBShape;
                ALLOCATE(newBShape, block.getWorkspace(), shape::shapeInfoLength(bShape), int);
                memcpy(newBShape, bShape, shape::shapeInfoByteLength(bShape));

                shapes->push_back(newBShape);
            }

            return shapes;
        }

//////////////////////////////////////////////////////////////////////////
        DECLARE_CONFIGURABLE_OP(conv3d, 2, 1, false, 0, 7) {
            // cubic convo

            NDArray<T> *input = block.getVariables().at(0)->getNDArray();
            NDArray<T> *weights = block.getVariables().at(1)->getNDArray();
            NDArray<T> *bias = nullptr;
            if (block.getVariables().size() == 3)
                bias = block.getVariables().at(2)->getNDArray();

            if (input->rankOf() != 5)
                return ND4J_STATUS_BAD_DIMENSIONS;

            NDArray<T> *output = this->getZ(block);

            bool biasUsed = block.getIArguments()->at(0) != 0 && bias != nullptr;
            int dT = block.getIArguments()->at(1);
            int dW = block.getIArguments()->at(2);
            int dH = block.getIArguments()->at(3);
            int pT = block.getIArguments()->at(4);
            int pW = block.getIArguments()->at(5);
            int pH = block.getIArguments()->at(6);


            if (pT != 0 || pW != 0 || pH != 0) {
                nd4j_printf("Padding isn't supported on CPU backend O_o","");
                return ND4J_STATUS_BAD_PARAMS;
            }

            // we always expect 5d
            int dimt = 2;
            int dimh = 3;
            int dimw = 4;

            Nd4jIndex nOutputPlane = weights->sizeAt(0);
            Nd4jIndex kT           = weights->sizeAt(2);
            Nd4jIndex kH           = weights->sizeAt(3);
            Nd4jIndex kW           = weights->sizeAt(4);
            Nd4jIndex inputDepth   = input->sizeAt(dimt);
            Nd4jIndex inputHeight  = input->sizeAt(dimh);
            Nd4jIndex inputWidth   = input->sizeAt(dimw);
            Nd4jIndex outputDepth  = (inputDepth - kT) / dT + 1;
            Nd4jIndex outputWidth  = (inputWidth - kW) / dW + 1;
            Nd4jIndex outputHeight = (inputHeight - kH) / dH + 1;


            REQUIRE_TRUE(output->sizeAt(0) == input->sizeAt(0) && output->sizeAt(1) == nOutputPlane && output->sizeAt(2) == outputDepth && output->sizeAt(3) == outputHeight && output->sizeAt(4) == outputWidth, 0,
                         "Expected output shape: [%i, %i, %i, %i, %i] but got [%i, %i, %i, %i, %i] instead", input->sizeAt(0), nOutputPlane, outputDepth, outputHeight, outputWidth, output->sizeAt(0), output->sizeAt(1), output->sizeAt(2), output->sizeAt(3), output->sizeAt(4));

            std::unique_ptr<ArrayList<T>> batchIn(NDArrayFactory::allExamples<T>(input));
            std::unique_ptr<ArrayList<T>> batchOut(NDArrayFactory::allExamples<T>(output));

            // TODO: eventually we want OMP being used here
            for (int e = 0; e < batchIn->size(); e++) {
                auto tadIn = batchIn->at(e);
                auto tadOut = batchOut->at(e);

                if (biasUsed) {
                    std::unique_ptr<ArrayList<T>> outputBlock(NDArrayFactory::allExamples<T>(tadOut));
                    for (int i = 0; i < bias->lengthOf(); i++) {
                        auto oB = outputBlock->at(i);
                        oB->assign(bias->getScalar(i));
                    }
                } else
                    output->assign(0.0);

                Nd4jStatus  res = conv3Dmv(tadOut, (T) 1.0f, (T) 1.0f, tadIn, weights, dT, dH, dW, "V", "X");
                if (res != ND4J_STATUS_OK)
                    throw "Boom";
            }

            STORE_RESULT(*output);

            return ND4J_STATUS_OK;
        }

        DECLARE_CONFIGURABLE_OP(conv3d_bp, 3, 1, false, 0, 7) {

            return ND4J_STATUS_OK;
        }

//////////////////////////////////////////////////////////////////////////
        /**
         * Upsampling implementation, based on pytorch
         *
         * IArgs map:
         * IArgs[0] - scale factor
         */
        DECLARE_CONFIGURABLE_OP(upsampling2d, 1, 1, false, 0, 1) {
            NDArray<T>* input = block.getVariables().at(0)->getNDArray();
            NDArray<T>* output = this->getZ(block);
            int scale_factor = block.getIArguments()->at(0);

//            int inputHeight = input->sizeAt(2);
//            int inputWidth  = input->sizeAt(3);

            int dW = scale_factor;
            int dH = scale_factor;
//            int outputHeight = inputHeight * scale_factor;
//            int outputWidth = inputWidth * scale_factor;
            int xDim = input->rankOf() - 2;
            int yDim = input->rankOf() - 1;

            int osz0 = output->sizeAt(0);
            int osz1 = output->sizeAt(1);
            int osz2 = output->sizeAt(2);
            int osz3 = output->sizeAt(3);

            int i0, i1, i2, i3, isrc, idst;
            int iout[4];  // Output indices
            int iin[4];  // Input indices

            for (i0 = 0; i0 < osz0; i0++) {
                iout[0] = i0;
                iin[0] = i0;
                for (i1 = 0; i1 < osz1; i1++) {
                    iout[1] = i1;
                    iin[1] = i1;
                    for (i2 = 0; i2 < osz2; i2++) {
                        iout[2] = i2;
                        iin[2] = i2;
                        for (i3 = 0; i3 < osz3; i3++) {
                            iout[3] = i3;
                            iin[3] = i3;

                            // set the indices for the upsampled dimensions
                            iin[xDim] = iout[xDim] / dW;
                            iin[yDim] = iout[yDim] / dH;

                            idst = i0 * output->stridesOf()[0] + i1 * output->stridesOf()[1] + i2 * output->stridesOf()[2];
                            isrc = iin[0] * input->stridesOf()[0] + iin[1] * input->stridesOf()[1] + iin[2] * input->stridesOf()[2];

                            // in our case rank of input is always 4
                            idst += i3 * output->stridesOf()[3];
                            isrc += iin[3]* input->stridesOf()[3];


                            output->getBuffer()[idst] = input->getBuffer()[isrc];
                        }
                    }
                }
            }

            STORE_RESULT(*output);

            return ND4J_STATUS_OK;
        }
        DECLARE_SYN(upsampling, upsampling2d);

//////////////////////////////////////////////////////////////////////////
        /**
         * Upsampling backprop implementation, based on pytorch
         *
         * Input[0] - preoutput result
         * Input[1] - gradients from next node/layer
         *
         * Output[0] - gradient for this node
         *
         * IArgs map:
         * IArgs[0] - scale factor
         */
        DECLARE_CONFIGURABLE_OP(upsampling2d_bp, 2, 1, false, 0, 1) {
            //NDArray<T>* input = block.getVariables().at(0)->getNDArray();
            NDArray<T>* gradientNext = block.getVariables().at(1)->getNDArray();
            NDArray<T>* output = this->getZ(block);
            int scale_factor = block.getIArguments()->at(0);


            int dW = scale_factor;
            int dH = scale_factor;
            int xDim = output->rankOf() - 2;
            int yDim = output->rankOf() - 1;

            // dims
            int idim = output->rankOf();  // Guaranteed to be between 3 and 5
            int isz0 = output->sizeAt(0);
            int isz1 = output->sizeAt(1);
            int isz2 = output->sizeAt(2);
            int isz3 = 1;
            if (idim > 3) {
                isz3 = output->sizeAt(3);
            }

            output->assign(0.0);

            // perform the upsampling
            int i0, i1, i2, i3, isrc, idst, x, y;
            int iin[4];  // Input indices
            int iout[4];  // Output indices

            for (i0 = 0; i0 < isz0; i0++) {
                iin[0] = i0;
                iout[0] = i0;
                for (i1 = 0; i1 < isz1; i1++) {
                    iin[1] = i1;
                    iout[1] = i1;
                    for (i2 = 0; i2 < isz2; i2++) {
                        iin[2] = i2;
                        iout[2] = i2;
                        for (i3 = 0; i3 < isz3; i3++) {
                            iin[3] = i3;
                            iout[3] = i3;

                            idst = i0 * output->stridesOf()[0] + i1 * output->stridesOf()[1] + i2 * output->stridesOf()[2];
                            if (idim > 3) {
                                idst += i3 * output->stridesOf()[3];
                            }

                            // Now accumulate the gradients from gradOutput
                            for (y = 0; y < dH; y++) {
                                for (x = 0; x < dW; x++) {
                                    iout[xDim] = dW * iin[xDim] + x;
                                    iout[yDim] = dH * iin[yDim] + y;
                                    isrc = iout[0] * gradientNext->stridesOf()[0] + iout[1] * gradientNext->stridesOf()[1] + iout[2] * gradientNext->stridesOf()[2];
                                    if (idim > 3) {
                                        isrc += iout[3] * gradientNext->stridesOf()[3];
                                    }
                                    output->getBuffer()[idst] += gradientNext->getBuffer()[isrc];
                                }
                            }
                        }
                    }
                }
            }

            STORE_RESULT(*output);

            return ND4J_STATUS_OK;
        }
        DECLARE_SYN(upsampling_bp, upsampling2d_bp);

//////////////////////////////////////////////////////////////////////////
        // maxpool2d corresponds to poolingMode=0
        DECLARE_CUSTOM_OP(maxpool2d, 1, 1, false, 0, 9) {

            NDArray<T> *x = block.getVariables().at(0)->getNDArray();

            REQUIRE_TRUE(x->rankOf() == 4, 0, "Input should have rank of 4, but got %i instead", x->rankOf());

            const int bSize = x->sizeAt(0);
            const int inD = x->sizeAt(1);
            const int inY = x->sizeAt(2);
            const int inX = x->sizeAt(3);

            std::vector<int> argI = *(block.getIArguments());
            auto z = this->getZ(block);

            int pY = argI[4];
            int pX = argI[5];

            const bool isSameMode = block.getIArguments()->at(8) > 0;
            if (isSameMode)
                nd4j::ops::_calcPadding2D(pY, pX, z->sizeAt(2), z->sizeAt(3), inY, inX, argI[0], argI[1], argI[2], argI[3], argI[6], argI[7]);

            // 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8 - same mode;


            // 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8,9 - poolingMode; 10 - divisor;
            std::vector<T> argT = {(T)argI[0], (T)argI[1], (T)argI[2], (T)argI[3], (T) pY, (T) pX, (T)argI[6], (T)argI[7], (T)0.f, (T)0.f, (T)1.f};

            x->template applyTransform<simdOps::Pooling2D<T>>(z, argT.data());

            STORE_RESULT(*z);            

            z->printShapeInfo("MaxPool2D result shape");

            return ND4J_STATUS_OK;
        }
        DECLARE_SYN(MaxPool2D, maxpool2d);
        DECLARE_SYN(MaxPool, maxpool2d);
        DECLARE_SYN(maxpool, maxpool2d);
        //////////////////////////////////////////////////////////////////////////
        DECLARE_SHAPE_FN(maxpool2d) {
            //NDArray<T> *x = block.getVariables().at(0)->getNDArray();
            int* inShape = inputShape->at(0);
            int* shapeOf = shape::shapeOf(inShape);
            // 0 - number of dimensions; 1,2 - kernel Height/Width; 3,4 - stride Height/Width; 5,6 - pad Height/Width; 7,8 - dilation Height/Width; 9,10 - input Height/Width; 11 - batch size; 12 - input depth; 13 - same mode;
            std::vector<int> argI = *(block.getIArguments());
            int kH = argI[0];
            int kW = argI[1];
            int sH = argI[2];
            int sW = argI[3];
            int pH = argI[4];
            int pW = argI[5];
            int dH = argI[6];
            int dW = argI[7];
            int isSameMode = argI[8];

            int bS = shapeOf[0];
            int iD = shapeOf[1];
            int iH = shapeOf[2];
            int iW = shapeOf[3];

            char order = shape::order(inShape); // output order must be equal to input order

            // calculate output Height/Width
            int oH, oW;
            nd4j::ops::calcOutHWpool2D(oH, oW, kH, kW, sH, sW, pH, pW, dH, dW, iH, iW, isSameMode);

            const bool bisSameMode = block.getIArguments()->at(8) > 0;
            if (bisSameMode)
                nd4j::ops::_calcPadding2D(pH, pW, oH, oW, iH, iW, argI[0], argI[1], argI[2], argI[3], argI[6], argI[7]);

            // allocate memory for new shape
            int* newShapeInfo = nullptr;
            ALLOCATE(newShapeInfo, block.getWorkspace(), 12, int);
            newShapeInfo[0] = 4;		// rank
            newShapeInfo[1] = bS;
            newShapeInfo[2] = iD;
            newShapeInfo[3] = oH;
            newShapeInfo[4] = oW;
            shape::updateStrides(newShapeInfo, order);

            return new ShapeList(newShapeInfo);
        }

//////////////////////////////////////////////////////////////////////////
        // avgpool2d corresponds to poolingMode=1
        DECLARE_CUSTOM_OP(avgpool2d, 1, 1, false, 0, 9) {

            NDArray<T> *x = block.getVariables().at(0)->getNDArray();

            REQUIRE_TRUE(x->rankOf() == 4, 0, "Input should have rank of 4, but got %i instead", x->rankOf());

            const int inY = x->sizeAt(2);
            const int inX = x->sizeAt(3);

            // 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8 - same mode;
            std::vector<int> argI = *(block.getIArguments());
            auto z = this->getZ(block);

            int pY = argI[4];
            int pX = argI[5];

            const bool isSameMode = block.getIArguments()->at(8) > 0;
            if (isSameMode)
                nd4j::ops::_calcPadding2D(pY, pX, z->sizeAt(2), z->sizeAt(3), inY, inX, argI[0], argI[1], argI[2], argI[3], argI[6], argI[7]);

            // 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8,9 - poolingMode; 10 - divisor;
            std::vector<T> argT = {(T) argI[0], (T) argI[1], (T) argI[2], (T) argI[3], (T) argI[4], (T) argI[5], (T)argI[6], (T)argI[7], (T)1.f, (T)1.f, (T)1.f};


            x->template applyTransform<simdOps::Pooling2D<T>>(z, argT.data());

            STORE_RESULT(*z);

            return ND4J_STATUS_OK;
        }

		DECLARE_SYN(AvgPool2D, avgpool2d);
        DECLARE_SYN(AvgPool, avgpool2d);
        DECLARE_SYN(avgpool, avgpool2d);
        //////////////////////////////////////////////////////////////////////////
        DECLARE_SHAPE_FN(avgpool2d) {
            int* inShape = inputShape->at(0);
            int* shapeOf = shape::shapeOf(inShape);

            // 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8 - same mode;
            std::vector<int> argI = *(block.getIArguments());
            int kH = argI[0];
            int kW = argI[1];
            int sH = argI[2];
            int sW = argI[3];
            int pH = argI[4];
            int pW = argI[5];
            int dH = argI[6];
            int dW = argI[7];
            int isSameMode = argI[8];

            int bS = shapeOf[0];
            int iD = shapeOf[1];
            int iH = shapeOf[2];
            int iW = shapeOf[3];


            char order = shape::order(inShape); // output order must be equal to input order

            // calculate output Height/Width
            int oH, oW;
            nd4j::ops::calcOutHWpool2D(oH, oW, kH, kW, sH, sW, pH, pW, dH, dW, iH, iW, isSameMode);

            const bool bisSameMode = block.getIArguments()->at(8) > 0;
            if (bisSameMode)
                nd4j::ops::_calcPadding2D(pH, pW, oH, oW, iH, iW, argI[0], argI[1], argI[2], argI[3], argI[6], argI[7]);


            // allocate memory for new shape
            int* newShapeInfo = nullptr;
            ALLOCATE(newShapeInfo, block.getWorkspace(), 12, int);
            newShapeInfo[0] = 4;		// rank
            newShapeInfo[1] = bS;
            newShapeInfo[2] = iD;
            newShapeInfo[3] = oH;
            newShapeInfo[4] = oW;
            shape::updateStrides(newShapeInfo, order);

            return new ShapeList(newShapeInfo);
        }

//////////////////////////////////////////////////////////////////////////
		// pnormpool2d corresponds to poolingMode=2	
        DECLARE_CUSTOM_OP(pnormpool2d, 1, 1, false, 0, 10) {

            REQUIRE_OK(this->validateInputLengthMatch(block));
            REQUIRE_OK(this->validateInputDimensionsMatch(block));
            NDArray<T> *x = block.getVariables().at(0)->getNDArray();
			REQUIRE_TRUE(x->rankOf() == 4, 0, "Input should have rank of 4, but got %i instead", x->rankOf());

            std::vector<int> argI = *(block.getIArguments()); // 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8 - same mode; 9 - extraParam0 for pnorm case;
            std::vector<T> argT = {(T) argI[1], (T) argI[2], (T) argI[3], (T) argI[4], (T) argI[5], (T) argI[6], (T) argI[7], (T) argI[8], (T)2.f, (T)2.f, (T)argI[9]};  // 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8,9 - poolingMode; 10 - extraParam0 for pnorm case;

            auto z = this->getZ(block);
            x->template applyTransform<simdOps::Pooling2D<T>>(z, argT.data());

            STORE_RESULT(*z);

            return ND4J_STATUS_OK;
        }
		DECLARE_SYN(PnormPool2D, pnormpool2d);
        DECLARE_SYN(PnormPool, pnormpool2d);
        DECLARE_SYN(pnormpool, pnormpool2d);
	
		//////////////////////////////////////////////////////////////////////////
		DECLARE_SHAPE_FN(pnormpool2d) {
			int* inShape = inputShape->at(0);
            // 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8 - same mode;
            std::vector<int> argI = *(block.getIArguments());
            int kH = argI[0];
            int kW = argI[1];
            int sH = argI[2];
            int sW = argI[3];
            int pH = argI[4];
            int pW = argI[5];
            int dH = argI[6];
            int dW = argI[7];
            int isSameMode = argI[8];

            int bS = inShape[1];
            int iD = inShape[2];
            int iH = inShape[3];
            int iW = inShape[4];

            char order = shape::order(inShape); // output order must be equal to input order

			// calculate output Height/Width
			int oH, oW;
			nd4j::ops::calcOutHWpool2D(oH, oW, kH, kW, sH, sW, pH, pW, dH, dW, iH, iW, isSameMode);
			// allocate memory for new shape
            int* newShapeInfo = nullptr;
            ALLOCATE(newShapeInfo, block.getWorkspace(), 12, int);
			newShapeInfo[0] = 4;		// rank
			newShapeInfo[1] = bS;
			newShapeInfo[2] = iD;
			newShapeInfo[3] = oH;
			newShapeInfo[4] = oW;
            shape::updateStrides(newShapeInfo, order);

			return new ShapeList(newShapeInfo);
		}	

//////////////////////////////////////////////////////////////////////////
        DECLARE_CONFIGURABLE_OP(maxpool3d, 1, 2, true, 0, 13) {

            NDArray<T> *input = block.getVariables().at(0)->getNDArray();

            NDArray<T> *output = this->getZ(block);
            NDArray<T> *indices = this->getZ(block, 1);

            REQUIRE_TRUE(input->sizeOfT() > 2, 0, "MaxPool3D can't be used in HALF precision")
            REQUIRE_TRUE(input->rankOf() == 5, 0, "Input should be 5D, got rank %i instead", input->rankOf());
            REQUIRE_TRUE(output->rankOf() == 5, 0, "Output should be 5D, got rank %i instead", output->rankOf());

            int kT = block.getIArguments()->at(0);
            int kW = block.getIArguments()->at(1);
            int kH = block.getIArguments()->at(2);
            int dT = block.getIArguments()->at(3);
            int dW = block.getIArguments()->at(4);
            int dH = block.getIArguments()->at(5);
            int pT = block.getIArguments()->at(6);
            int pW = block.getIArguments()->at(7);
            int pH = block.getIArguments()->at(8);
            int dilationT = block.getIArguments()->at(9);
            int dilationW = block.getIArguments()->at(10);
            int dilationH = block.getIArguments()->at(11);
            bool ceilMode = block.getIArguments()->at(12) != 0;


            REQUIRE_TRUE(kT > 0 && kW > 0 && kH > 0, 0,
                         "Kernel size should be greater than zero, but got kT: %d kH: %d kW: %d",
                         kT, kH, kW);

            REQUIRE_TRUE(dT > 0 && dW > 0 && dH > 0, 8,
                         "stride should be greater than zero, but got dT: %d dH: %d dW: %d",
                         dT, dH, dW);

            REQUIRE_TRUE(dilationT > 0 && dilationW > 0 && dilationH > 0, 14,
                         "dilation should be greater than 0, but got dilationT: %d dilationH: %d dilationW: %d",
                         dilationT, dilationH, dilationW);

            REQUIRE_TRUE(kT/2 >= pT && kW/2 >= pW && kH/2 >= pH, 2,
                         "pad should be smaller than half of kernel size, but got "
                                 "kT: %d kW: %d, kH: %d, padT: %d, padW: %d, padH: %d",
                         kT, kW, kH, pT, pW, pH);

            Nd4jIndex nslices;
            Nd4jIndex itime;
            Nd4jIndex iheight;
            Nd4jIndex iwidth;
            Nd4jIndex otime;
            Nd4jIndex oheight;
            Nd4jIndex owidth;
            T *input_data;
            T *output_data;

            ////////////
            T *indices_data;


            int dimN = 1;
            int dimt = 2;
            int dimh = 3;
            int dimw = 4;


            nslices = input->sizeAt(dimN);
            itime   = input->sizeAt(dimt);
            iheight = input->sizeAt(dimh);
            iwidth  = input->sizeAt(dimw);

            if (ceilMode) {
                otime = (int)(nd4j::math::nd4j_ceil<T>((T)(itime - (dilationT * (kT - 1) + 1) + 2*pT) / dT)) + 1;
                oheight = (int)(nd4j::math::nd4j_ceil<T>((T)(iheight - (dilationH * (kH - 1) + 1) + 2*pH) / dH)) + 1;
                owidth  = (int)(nd4j::math::nd4j_ceil<T>((T)(iwidth  - (dilationW * (kW - 1) + 1) + 2*pW) / dW)) + 1;
            } else {
                otime = (int)(nd4j::math::nd4j_floor<T>((T)(itime - (dilationT * (kT - 1) + 1) + 2*pT) / dT)) + 1;
                oheight = (int)(nd4j::math::nd4j_floor<T>((T)(iheight - (dilationH * (kH - 1) + 1) + 2*pH) / dH)) + 1;
                owidth  = (int)(nd4j::math::nd4j_floor<T>((T)(iwidth  - (dilationW * (kW - 1) + 1) + 2*pW) / dW)) + 1;
            }

            if (pT > 0 || pW > 0 || pH > 0) {
                // ensure that the last pooling starts inside the image
                if ((otime - 1)*dT >= itime + pT)
                    --otime;
                if ((oheight - 1)*dH >= iheight + pH)
                    --oheight;
                if ((owidth  - 1)*dW >= iwidth  + pW)
                    --owidth;
            }


            REQUIRE_TRUE(otime >= 1 && owidth >= 1 && oheight >= 1, 0, "Output size is too small: [%i, %i, %i]", otime, oheight, owidth);

            NDArray<T>* _input;
            if (!input->isContiguous())
                _input = input->dup(input->ordering());
            else
                _input = input;

            Nd4jIndex istride = nslices * itime * iwidth * iheight;
            Nd4jIndex ostride = nslices * otime * owidth * oheight;

            REQUIRE_TRUE(output->sizeAt(0) == input->sizeAt(0) && output->sizeAt(1) == nslices && output->sizeAt(2) == otime && output->sizeAt(3) == oheight && output->sizeAt(4) == owidth, 0,
                         "Output shape expected to be [%i, %i, %i, %i, %i], but got [%i, %i, %i, %i, %i] instead", input->sizeAt(0), nslices, otime, oheight, owidth, output->sizeAt(0), output->sizeAt(1), output->sizeAt(2), output->sizeAt(3), output->sizeAt(4));

            REQUIRE_TRUE(indices->isSameShape(output), 0, "Output and Indices shapes should be equal");

            input_data = _input->getBuffer();
            output_data = output->getBuffer();
            indices_data = indices->getBuffer();

            for (int n = 0; n < input->sizeAt(0); n++) {
                nd4j::ops::_dilatedMaxPool3D(
                        input_data   + n * istride,
                        output_data  + n * ostride,
                        indices_data + n * ostride,
                        nslices,
                        itime, iwidth, iheight,
                        otime, owidth, oheight,
                        kT, kW, kH,
                        dT, dW, dH,
                        pT, pW, pH,
                        dilationT, dilationW, dilationH);
            }

            if (_input != input)
                delete _input;

            STORE_RESULT(*output);

            return ND4J_STATUS_OK;
        }
        DECLARE_SYN(MaxPool3D, maxpool3d);
        DECLARE_SYN(MaxPool3d, maxpool3d);

//////////////////////////////////////////////////////////////////////////
        DECLARE_CUSTOM_OP(maxpool3d_bp, 3, 1, true, 0, 13) {

            NDArray<T> *input = block.getVariables().at(0)->getNDArray();
            NDArray<T> *gradNext = block.getVariables().at(1)->getNDArray();
            NDArray<T> *indices = block.getVariables().at(2)->getNDArray();

            NDArray<T> *output = this->getZ(block);

            REQUIRE_TRUE(input->rankOf() == 5, 0, "Input should be 5D, got %i instead", input->rankOf());
            REQUIRE_TRUE(indices->isSameShape(input), 1, "Indices should have the same dimensionality as input");
            REQUIRE_TRUE(output->isSameShape(input), 1, "Output gradient should have the same dimensionality as input");


            int kT = block.getIArguments()->at(0);
            int kW = block.getIArguments()->at(1);
            int kH = block.getIArguments()->at(2);
            int dT = block.getIArguments()->at(3);
            int dW = block.getIArguments()->at(4);
            int dH = block.getIArguments()->at(5);
            int pT = block.getIArguments()->at(6);
            int pW = block.getIArguments()->at(7);
            int pH = block.getIArguments()->at(8);
            int dilationT = block.getIArguments()->at(9);
            int dilationW = block.getIArguments()->at(10);
            int dilationH = block.getIArguments()->at(11);
            bool ceilMode = block.getIArguments()->at(12) != 0;


            REQUIRE_TRUE(kT > 0 && kW > 0 && kH > 0, 0,
                         "Kernel size should be greater than zero, but got kT: %d kH: %d kW: %d",
                         kT, kH, kW);

            REQUIRE_TRUE(dT > 0 && dW > 0 && dH > 0, 8,
                         "stride should be greater than zero, but got dT: %d dH: %d dW: %d",
                         dT, dH, dW);

            REQUIRE_TRUE(dilationT > 0 && dilationW > 0 && dilationH > 0, 14,
                         "dilation should be greater than 0, but got dilationT: %d dilationH: %d dilationW: %d",
                         dilationT, dilationH, dilationW);

            REQUIRE_TRUE(kT/2 >= pT && kW/2 >= pW && kH/2 >= pH, 2,
                         "pad should be smaller than half of kernel size, but got "
                                 "kT: %d kW: %d, kH: %d, padT: %d, padW: %d, padH: %d",
                         kT, kW, kH, pT, pW, pH);


            int nslices;
            int itime;
            int iheight;
            int iwidth;
            int otime;
            int oheight;
            int owidth;
            T *gradInput_data;
            T *gradOutput_data;
            T *indices_data;

            int dimN = 1;
            int dimt = 2;
            int dimh = 3;
            int dimw = 4;

            /* sizes */
            nslices = input->sizeAt(dimN);
            itime = input->sizeAt(dimt);
            iheight = input->sizeAt(dimh);
            iwidth = input->sizeAt(dimw);
            otime = gradNext->sizeAt(dimt);
            oheight = gradNext->sizeAt(dimh);
            owidth = gradNext->sizeAt(dimw);

            /* get raw pointers */
            gradInput_data = output->getBuffer();
            gradOutput_data = gradNext->getBuffer();
            indices_data = indices->getBuffer();

            int nBatch = input->sizeAt(0);

            Nd4jIndex istride = nslices * itime * iwidth * iheight;
            Nd4jIndex ostride = nslices * otime * owidth * oheight;

            for (int p = 0; p < nBatch; p++) {
                nd4j::ops::_dilatedMaxPool3D_bp(
                        gradInput_data + p * istride,
                        gradOutput_data + p * ostride,
                        indices_data + p * ostride,
                        nslices,
                        itime, iwidth, iheight,
                        otime, owidth, oheight,
                        dT, dW, dH,
                        pT, pW, pH,
                        dilationT, dilationW, dilationH
                );
            }

            STORE_RESULT(*output);

            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(maxpool3d_bp) {
            // output shape equals to input shape, all out of sudden
            int* newShape;
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(inputShape->at(0)), int);
            memcpy(newShape, inputShape->at(0), shape::shapeInfoByteLength(inputShape->at(0)));
            return new ShapeList(newShape);
        }

//////////////////////////////////////////////////////////////////////////
        DECLARE_CUSTOM_OP(avgpool3d, 1, 1, true, 0, 11) {

            NDArray<T> *input = block.getVariables().at(0)->getNDArray();
            NDArray<T> *output = this->getZ(block);

            REQUIRE_TRUE(input->rankOf() == 5, 0, "Input should be 5D, got %i instead", input->rankOf());

            int kT = block.getIArguments()->at(0);
            int kW = block.getIArguments()->at(1);
            int kH = block.getIArguments()->at(2);
            int dT = block.getIArguments()->at(3);
            int dW = block.getIArguments()->at(4);
            int dH = block.getIArguments()->at(5);
            int padT = block.getIArguments()->at(6);
            int padW = block.getIArguments()->at(7);
            int padH = block.getIArguments()->at(8);
            bool ceil_mode = block.getIArguments()->at(9) != 0;
            bool count_include_pad  = block.getIArguments()->at(10) != 0;


            Nd4jIndex nslices;
            Nd4jIndex itime;
            Nd4jIndex iheight;
            Nd4jIndex iwidth;
            Nd4jIndex otime;
            Nd4jIndex oheight;
            Nd4jIndex owidth;
            T *input_data;
            T *output_data;

            int dimN = 1;
            int dimt = 2;
            int dimh = 3;
            int dimw = 4;

            nslices = input->sizeAt(dimN);
            itime   = input->sizeAt(dimt);
            iheight = input->sizeAt(dimh);
            iwidth  = input->sizeAt(dimw);

            if (ceil_mode)
            {
                otime   = (Nd4jIndex)(nd4j::math::nd4j_ceil<T>((T)(itime   - kT + 2*padT) / dT)) + 1;
                oheight = (Nd4jIndex)(nd4j::math::nd4j_ceil<T>((T)(iheight - kH + 2*padH) / dH)) + 1;
                owidth  = (Nd4jIndex)(nd4j::math::nd4j_ceil<T>((T)(iwidth  - kW + 2*padW) / dW)) + 1;
            }
            else
            {
                otime   = (Nd4jIndex)(nd4j::math::nd4j_floor<T>((T)(itime   - kT + 2*padT) / dT)) + 1;
                oheight = (Nd4jIndex)(nd4j::math::nd4j_floor<T>((T)(iheight - kH + 2*padH) / dH)) + 1;
                owidth  = (Nd4jIndex)(nd4j::math::nd4j_floor<T>((T)(iwidth  - kW + 2*padW) / dW)) + 1;
            }
            if (padT || padH || padW)
            {
                // ensure that the last pooling starts inside the image
                // needed to avoid problems in ceil mode
                if ((otime   - 1)*dT >= itime   + padT)
                    --otime;
                if ((oheight - 1)*dH >= iheight + padH)
                    --oheight;
                if ((owidth  - 1)*dW >= iwidth  + padW)
                    --owidth;
            }

            int nBatch = input->sizeAt(0);

            Nd4jIndex istride = nslices * itime * iwidth * iheight;
            Nd4jIndex ostride = nslices * otime * owidth * oheight;

            REQUIRE_TRUE(output->isSameShape({nBatch, (int) nslices, (int)otime, (int)oheight, (int)owidth}), 0, "Output should have shape of [%i, %i, %i, %i, %i], but got [%i, %i, %i, %i, %i] instead", nBatch, nslices, otime, oheight, owidth, output->sizeAt(0), output->sizeAt(1), output->sizeAt(2), output->sizeAt(3), output->sizeAt(4));

            input_data = input->getBuffer();
            output_data = output->getBuffer();

            for (int p=0; p < nBatch; p++)
            {

                nd4j::ops::_avgPool3D(
                        input_data + p * istride, output_data + p * ostride, nslices,
                        itime, iwidth, iheight,
                        otime, owidth, oheight,
                        kT, kW, kH,
                        dT, dW, dH,
                        padT, padW, padH,
                        count_include_pad
                );

            }

            STORE_RESULT(*output);

            return ND4J_STATUS_OK;
        }

        DECLARE_SHAPE_FN(avgpool3d) {
            int* input = inputShape->at(0);

            int kT = block.getIArguments()->at(0);
            int kW = block.getIArguments()->at(1);
            int kH = block.getIArguments()->at(2);
            int dT = block.getIArguments()->at(3);
            int dW = block.getIArguments()->at(4);
            int dH = block.getIArguments()->at(5);
            int padT = block.getIArguments()->at(6);
            int padW = block.getIArguments()->at(7);
            int padH = block.getIArguments()->at(8);
            bool ceil_mode = block.getIArguments()->at(9) != 0;

            Nd4jIndex nslices;
            Nd4jIndex itime;
            Nd4jIndex iheight;
            Nd4jIndex iwidth;
            Nd4jIndex otime;
            Nd4jIndex oheight;
            Nd4jIndex owidth;

            int dimN = 2;
            int dimt = 3;
            int dimh = 4;
            int dimw = 5;

            int nBatch = input[1];
            nslices = input[dimN];
            itime   = input[dimt];
            iheight = input[dimh];
            iwidth  = input[dimw];

            if (ceil_mode)
            {
                otime   = (Nd4jIndex)(nd4j::math::nd4j_ceil<T>((T)(itime   - kT + 2*padT) / dT)) + 1;
                oheight = (Nd4jIndex)(nd4j::math::nd4j_ceil<T>((T)(iheight - kH + 2*padH) / dH)) + 1;
                owidth  = (Nd4jIndex)(nd4j::math::nd4j_ceil<T>((T)(iwidth  - kW + 2*padW) / dW)) + 1;
            }
            else
            {
                otime   = (Nd4jIndex)(nd4j::math::nd4j_floor<T>((T)(itime   - kT + 2*padT) / dT)) + 1;
                oheight = (Nd4jIndex)(nd4j::math::nd4j_floor<T>((T)(iheight - kH + 2*padH) / dH)) + 1;
                owidth  = (Nd4jIndex)(nd4j::math::nd4j_floor<T>((T)(iwidth  - kW + 2*padW) / dW)) + 1;
            }
            if (padT || padH || padW)
            {
                // ensure that the last pooling starts inside the image
                // needed to avoid problems in ceil mode
                if ((otime   - 1)*dT >= itime   + padT)
                    --otime;
                if ((oheight - 1)*dH >= iheight + padH)
                    --oheight;
                if ((owidth  - 1)*dW >= iwidth  + padW)
                    --owidth;
            }

            int *shapeOf;
            int *newShape;
            ALLOCATE(shapeOf, block.getWorkspace(), 5, int);
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(5), int);

            nd4j::ArrayUtils::toIntPtr({nBatch, (int) nslices, (int)otime, (int)oheight, (int)owidth}, shapeOf);

            shape::shapeBuffer(5, shapeOf, newShape);

            RELEASE(shapeOf, block.getWorkspace());
            return new ShapeList(newShape);
        }


//////////////////////////////////////////////////////////////////////////
        DECLARE_CUSTOM_OP(avgpool3d_bp, 2, 1, true, 0, 11) {
            NDArray<T> *input = block.getVariables().at(0)->getNDArray();
            NDArray<T> *gradNext = block.getVariables().at(1)->getNDArray();

            NDArray<T> *output = this->getZ(block);

            REQUIRE_TRUE(input->rankOf() == 5, 0, "Input should be 5D, got %i instead", input->rankOf());

            Nd4jIndex nslices;
            Nd4jIndex itime;
            Nd4jIndex iheight;
            Nd4jIndex iwidth;
            Nd4jIndex otime;
            Nd4jIndex oheight;
            Nd4jIndex owidth;
            T *gradInput_data;
            T *gradOutput_data;
            int kT = block.getIArguments()->at(0);
            int kW = block.getIArguments()->at(1);
            int kH = block.getIArguments()->at(2);
            int dT = block.getIArguments()->at(3);
            int dW = block.getIArguments()->at(4);
            int dH = block.getIArguments()->at(5);
            int padT = block.getIArguments()->at(6);
            int padW = block.getIArguments()->at(7);
            int padH = block.getIArguments()->at(8);
            bool ceil_mode = block.getIArguments()->at(9) != 0;
            bool count_include_pad  = block.getIArguments()->at(10) != 0;

            REQUIRE_TRUE(output->isSameShape(input), 0, "Output gradients should have the same dimensionality as input");

            int dimN = 1;
            int dimt = 2;
            int dimh = 3;
            int dimw = 4;

            output->assign(0.0);

            nslices = input->sizeAt(dimN);
            itime = input->shapeOf()[dimt];
            iheight = input->shapeOf()[dimh];
            iwidth = input->shapeOf()[dimw];
            otime = gradNext->shapeOf()[dimt];
            oheight = gradNext->shapeOf()[dimh];
            owidth = gradNext->shapeOf()[dimw];


            gradInput_data = output->getBuffer();
            gradOutput_data = gradNext->getBuffer();

            long nBatch = input->sizeAt(0);

            long istride = nslices * itime * iwidth * iheight;
            long ostride = nslices * otime * owidth * oheight;

            for (int p = 0; p < nBatch; p++)
            {
                nd4j::ops::_avgPool3D_bp(
                        gradInput_data  + p * istride,
                        gradOutput_data + p * ostride,
                        nslices,
                        itime, iwidth, iheight,
                        otime, owidth, oheight,
                        kT, kW, kH,
                        dT, dW, dH,
                        padT, padW, padH,
                        count_include_pad
                );
            }

            STORE_RESULT(*output);

            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(avgpool3d_bp) {
            // output shape equals to input shape, all out of sudden
            int* newShape;
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(inputShape->at(0)), int);
            memcpy(newShape, inputShape->at(0), shape::shapeInfoByteLength(inputShape->at(0)));
            return new ShapeList(newShape);
        }


//////////////////////////////////////////////////////////////////////////
        DECLARE_CUSTOM_OP(fullconv3d, 5, 1, false, 0, 13) {

            NDArray<T> *input = block.getVariables().at(0)->getNDArray();
            NDArray<T> *weights = block.getVariables().at(1)->getNDArray();
            NDArray<T> *bias = block.getVariables().at(2)->getNDArray();
            NDArray<T> *columns = block.getVariables().at(3)->getNDArray();
            NDArray<T> *ones = block.getVariables().at(4)->getNDArray();

            REQUIRE_TRUE(weights->rankOf() == 5, 0, "Weights should be 5D, got %i instead", weights->rankOf());
            REQUIRE_TRUE(input->rankOf() == 5, 0, "Input should be 5D, got %i instead", input->rankOf());

            int dT = block.getIArguments()->at(0);
            int dW = block.getIArguments()->at(1);
            int dH = block.getIArguments()->at(2);
            int pT = block.getIArguments()->at(3);
            int pW = block.getIArguments()->at(4);
            int pH = block.getIArguments()->at(5);
            int dilationT = block.getIArguments()->at(6);
            int dilationW = block.getIArguments()->at(7);
            int dilationH = block.getIArguments()->at(8);
            int aT = block.getIArguments()->at(9);
            int aW = block.getIArguments()->at(10);
            int aH = block.getIArguments()->at(11);
            bool biasUsed = block.getIArguments()->at(12) != 0;


            REQUIRE_TRUE(dT > 0 && dW > 0 && dH > 0, 11,
                       "stride should be greater than zero, but got dT: %d dH: %d dW: %d", dT, dH, dW);
            REQUIRE_TRUE(dilationT > 0 && dilationW > 0 && dilationH > 0, 15,
                       "dilation should be greater than zero, but got dilationT: %d, dilationH: %d, dilationW: %d",
                       dilationT, dilationH, dilationW);
            REQUIRE_TRUE((aT < dT || aT < dilationT)
                       && (aW < dW || aW < dilationW)
                       && (aH < dH || aH < dilationH), 15,
                       "output padding must be smaller than either stride or dilation,"
                               " but got aT: %d aH: %d aW: %d dT: %d dH: %d dW: %d "
                               "dilationT: %d dilationH: %d dilationW: %d",
                       aT, aH, aW, dT, dH, dW, dilationT, dilationH, dilationW);

            NDArray<T> *output = this->getZ(block);

            const int nInputPlane  = weights->shapeOf()[0];
            const int nOutputPlane = weights->shapeOf()[1];
            const int kT           = weights->shapeOf()[2];
            const int kH           = weights->shapeOf()[3];
            const int kW           = weights->shapeOf()[4];

            const Nd4jIndex inputWidth   = input->shapeOf()[4];
            const Nd4jIndex inputHeight  = input->shapeOf()[3];
            const Nd4jIndex inputDepth   = input->shapeOf()[2];
            const Nd4jIndex outputDepth  = (inputDepth - 1) * dT - 2*pT + (dilationT * (kT - 1) + 1) + aT;
            const Nd4jIndex outputHeight = (inputHeight - 1) * dH - 2*pH + (dilationH * (kH - 1) + 1) + aH;
            const Nd4jIndex outputWidth  = (inputWidth - 1) * dW - 2*pW + (dilationW * (kW - 1) + 1) + aW;

            const Nd4jIndex batchSize = input->shapeOf()[0];

            REQUIRE_TRUE(output->isSameShape({ (int) batchSize, (int)nOutputPlane, (int)outputDepth, (int)outputHeight, (int)outputWidth}), 0, "Output should have shape of [%i, %i, %i, %i, %i], but got [%i, %i, %i, %i, %i] instead", (int) batchSize, (int)nOutputPlane, (int)outputDepth, (int)outputHeight, (int)outputWidth, output->sizeAt(0), output->sizeAt(1), output->sizeAt(2), output->sizeAt(3), output->sizeAt(4));

            std::unique_ptr<ArrayList<T>> inputs(NDArrayFactory::allExamples(input));
            std::unique_ptr<ArrayList<T>> outputs(NDArrayFactory::allExamples(output));
            for (int e = 0; e < batchSize; e++) {
                auto tadIn = inputs->at(e);
                auto tadOut = outputs->at(e);

                const int m = weights->shapeOf()[1] * weights->shapeOf()[2] * weights->shapeOf()[3] * weights->shapeOf()[4];
                const int n = columns->shapeOf()[1];
                const int k = weights->shapeOf()[0];

                nd4j::blas::GEMM<T>::op('c', 'n', 't', m, n, k,
                                        1.0,
                                        tadIn->getBuffer(), n,
                                        weights->getBuffer(), m,
                                        0.0,
                                        columns->getBuffer(), n);

                nd4j::ops::_col2vol(columns->getBuffer(),
                        nOutputPlane, outputDepth, outputHeight, outputWidth,
                        inputDepth, inputHeight, inputWidth,
                        kT, kH, kW,
                        pT, pH, pW,
                        dT, dH, dW,
                        dilationT,  dilationH,  dilationW,
                        tadOut->getBuffer());


                const int m_ = nOutputPlane;
                const int n_ = outputDepth * outputHeight * outputWidth;
                const int k_ = 1;

                if (biasUsed) {
                    nd4j::blas::GEMM<T>::op('c', 't', 'n', n_, m_, k_,
                            1.0,
                            ones->getBuffer(), k_,
                            bias->getBuffer(), k_,
                            1.0,
                            tadOut->getBuffer(), n_);
                }
            }

            STORE_RESULT(*output);

            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(fullconv3d) {

            int* input = inputShape->at(0);
            int* weights = inputShape->at(1);

            int dT = block.getIArguments()->at(0);
            int dW = block.getIArguments()->at(1);
            int dH = block.getIArguments()->at(2);
            int pT = block.getIArguments()->at(3);
            int pW = block.getIArguments()->at(4);
            int pH = block.getIArguments()->at(5);
            int dilationT = block.getIArguments()->at(6);
            int dilationW = block.getIArguments()->at(7);
            int dilationH = block.getIArguments()->at(8);
            int aT = block.getIArguments()->at(9);
            int aW = block.getIArguments()->at(10);
            int aH = block.getIArguments()->at(11);
            bool biasUsed = block.getIArguments()->at(12) != 0;

            int *shapeOf;
            int *newShape;
            ALLOCATE(shapeOf, block.getWorkspace(), 5, int);
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(5), int);

            const int nInputPlane  = weights[1];
            const int nOutputPlane = weights[2];
            const int kT           = weights[3];
            const int kH           = weights[4];
            const int kW           = weights[5];

            const int batchSize          = input[1];
            const Nd4jIndex inputWidth   = input[5];
            const Nd4jIndex inputHeight  = input[4];
            const Nd4jIndex inputDepth   = input[3];
            const Nd4jIndex outputDepth  = (inputDepth - 1) * dT - 2*pT + (dilationT * (kT - 1) + 1) + aT;
            const Nd4jIndex outputHeight = (inputHeight - 1) * dH - 2*pH + (dilationH * (kH - 1) + 1) + aH;
            const Nd4jIndex outputWidth  = (inputWidth - 1) * dW - 2*pW + (dilationW * (kW - 1) + 1) + aW;

            nd4j::ArrayUtils::toIntPtr({(int) batchSize, (int)nOutputPlane, (int)outputDepth, (int)outputHeight, (int)outputWidth}, shapeOf);

            shape::shapeBuffer(5, shapeOf, newShape);

            RELEASE(shapeOf, block.getWorkspace());

            return new ShapeList(newShape);
        }

//////////////////////////////////////////////////////////////////////////
        DECLARE_CUSTOM_OP(fullconv3d_bp, 5, 1, false, 0, 13) {

            NDArray<T> *input = block.getVariables().at(0)->getNDArray();
            NDArray<T> *gradNext = block.getVariables().at(1)->getNDArray();
            NDArray<T> *weights = block.getVariables().at(2)->getNDArray();
            NDArray<T> *finput = block.getVariables().at(3)->getNDArray();

            // not used
            NDArray<T> *fgradInput = block.getVariables().at(4)->getNDArray();


            REQUIRE_TRUE(weights->rankOf() == 5, 0, "Weights should be 5D, got %i instead", weights->rankOf());
            REQUIRE_TRUE(input->rankOf() == 5, 0, "Input should be 5D, got %i instead", input->rankOf());

            NDArray<T> *output = this->getZ(block);

            int dT = block.getIArguments()->at(0);
            int dW = block.getIArguments()->at(1);
            int dH = block.getIArguments()->at(2);
            int pT = block.getIArguments()->at(3);
            int pW = block.getIArguments()->at(4);
            int pH = block.getIArguments()->at(5);
            int dilationT = block.getIArguments()->at(6);
            int dilationW = block.getIArguments()->at(7);
            int dilationH = block.getIArguments()->at(8);
            int aT = block.getIArguments()->at(9);
            int aW = block.getIArguments()->at(10);
            int aH = block.getIArguments()->at(11);
            bool biasUsed = block.getIArguments()->at(12) != 0;

            const int nInputPlane  = (int)weights->shapeOf()[0];
            const int nOutputPlane = (int)weights->shapeOf()[1];
            const int kT           = (int)weights->shapeOf()[2];
            const int kH           = (int)weights->shapeOf()[3];
            const int kW           = (int)weights->shapeOf()[4];

            const Nd4jIndex inputWidth   = input->shapeOf()[4];
            const Nd4jIndex inputHeight  = input->shapeOf()[3];
            const Nd4jIndex inputDepth   = input->shapeOf()[2];
            const Nd4jIndex outputDepth  = (inputDepth - 1) * dT - 2*pT + (dilationT * (kT - 1) + 1) + aT;
            const Nd4jIndex outputHeight = (inputHeight - 1) * dH - 2*pH + (dilationH * (kH - 1) + 1) + aH;
            const Nd4jIndex outputWidth  = (inputWidth - 1) * dW - 2*pW + (dilationW * (kW - 1) + 1) + aW;

            const Nd4jIndex batchSize = input->shapeOf()[0];


            REQUIRE_TRUE(output->isSameShape({(int) batchSize, (int) nInputPlane, (int) inputDepth, (int) inputHeight, (int) inputWidth}) ,0, "Output should have shape of [%i, %i, %i, %i, %i], but got [%i, %i, %i, %i, %i] instead", (int) batchSize, (int) nInputPlane, (int) inputDepth, (int) inputHeight, (int) inputWidth, output->sizeAt(0), output->sizeAt(1), output->sizeAt(2), output->sizeAt(3), output->sizeAt(4));

            output->assign(0.0);

            // FIXME: non-inplace reshape!!!!
            NDArray<T> *gradColumns;
            //auto gradColumns = finput->reshape('c', {nOutputPlane*kW*kH*kT, inputDepth*inputHeight*inputWidth });

            std::unique_ptr<ArrayList<T>> tadsNext(NDArrayFactory::allExamples<T>(gradNext));
            std::unique_ptr<ArrayList<T>> tadsOutput(NDArrayFactory::allExamples<T>(output));
            for (int e = 0; e < tadsNext->size(); e++) {
                auto tadNext = tadsNext->at(e);
                auto tadOutput = tadsOutput->at(e);

                nd4j::ops::_vol2col<T>(
                        tadNext->getBuffer(),
                        nOutputPlane, outputDepth, outputHeight, outputWidth,
                        kT, kH, kW,
                        pT, pH, pW,
                        dT, dH, dW,
                        dilationT,  dilationH,  dilationW,
                        gradColumns->getBuffer());

                const long m = weights->shapeOf()[0];
                const long n = gradColumns->shapeOf()[1];
                const long k = weights->shapeOf()[1] * weights->shapeOf()[2] * weights->shapeOf()[3] * weights->shapeOf()[4];

                nd4j::blas::GEMM<T>::op('f', 'n', 'n',
                        n, m, k,
                        1.0f,
                        gradColumns->getBuffer(), n,
                        weights->getBuffer(), k,
                        0,
                        tadOutput->getBuffer(), n
                );
            }


            STORE_RESULT(*output);

            delete gradColumns;
            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(fullconv3d_bp) {
            // output shape equals to input shape, all out of sudden
            int* newShape;
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(inputShape->at(0)), int);
            memcpy(newShape, inputShape->at(0), shape::shapeInfoByteLength(inputShape->at(0)));
            return new ShapeList(newShape);
        }

//////////////////////////////////////////////////////////////////////////
        DECLARE_CUSTOM_OP(fullconv3d_grad, 4, 2, false, 1, 13) {

            NDArray<T> *input = block.getVariables().at(0)->getNDArray();
            NDArray<T> *epsilon = block.getVariables().at(1)->getNDArray();
            NDArray<T> *columns = block.getVariables().at(2)->getNDArray();
            NDArray<T> *ones = block.getVariables().at(3)->getNDArray();

            REQUIRE_TRUE(input->rankOf() == epsilon->rankOf(), 0, "Rank of input (%i) & epsilon (%i) should be equal", input->rankOf(), epsilon->rankOf());
            REQUIRE_TRUE(input->sizeAt(0) == epsilon->sizeAt(0), 1, "Batch size should be equal for input and epsilon");

            NDArray<T> *gradWeight = this->getZ(block);
            NDArray<T> *gradBias = this->getZ(block, 1);

            REQUIRE_TRUE(gradBias->sizeAt(0) == gradWeight->sizeAt(1), 0, "Bias shape mismatch");

            int dT = block.getIArguments()->at(0);
            int dW = block.getIArguments()->at(1);
            int dH = block.getIArguments()->at(2);
            int pT = block.getIArguments()->at(3);
            int pW = block.getIArguments()->at(4);
            int pH = block.getIArguments()->at(5);
            int dilationT = block.getIArguments()->at(6);
            int dilationW = block.getIArguments()->at(7);
            int dilationH = block.getIArguments()->at(8);
            int aT = block.getIArguments()->at(9);
            int aW = block.getIArguments()->at(10);
            int aH = block.getIArguments()->at(11);
            bool biasUsed = block.getIArguments()->at(12) != 0;

            T scale = block.getTArguments()->at(0);

            int nInputPlane  = (int)gradWeight->shapeOf()[0];
            int nOutputPlane = (int)gradWeight->shapeOf()[1];
            int kT           = (int)gradWeight->shapeOf()[2];
            int kH           = (int)gradWeight->shapeOf()[3];
            int kW           = (int)gradWeight->shapeOf()[4];


            const Nd4jIndex inputWidth   = input->shapeOf()[4];
            const Nd4jIndex inputHeight  = input->shapeOf()[3];
            const Nd4jIndex inputDepth   = input->shapeOf()[2];
            const Nd4jIndex outputDepth  = (inputDepth - 1) * dT - 2*pT + (dilationT * (kT - 1) + 1) + aT;
            const Nd4jIndex outputHeight = (inputHeight - 1) * dH - 2*pH + (dilationH * (kH - 1) + 1) + aH;
            const Nd4jIndex outputWidth  = (inputWidth - 1) * dW - 2*pW + (dilationW * (kW - 1) + 1) + aW;


            REQUIRE_TRUE(gradWeight->isContiguous(), 0, "gradWight should be continuous");
            REQUIRE_TRUE(gradBias->isContiguous(), 0, "gradBias should be continuous");
            REQUIRE_TRUE(ones->rankOf() == 3, 0, "Ones should have rank 3, got %i instead", ones->rankOf());

            REQUIRE_TRUE(ones->isSameShape({outputDepth, outputHeight, outputWidth}), 0, "");

            ones->assign(1.0);

            std::unique_ptr<ArrayList<T>> tadsInput(NDArrayFactory::allExamples<T>(input));
            std::unique_ptr<ArrayList<T>> tadsEpsilon(NDArrayFactory::allExamples<T>(epsilon));

            for (int e = 0; e < tadsInput->size(); e++) {
                auto tadInput = tadsInput->at(e);
                auto tadEpsilon = tadsEpsilon->at(e);

                nd4j::ops::_vol2col<T>(
                        tadEpsilon->getBuffer(), nOutputPlane,
                        outputDepth, outputHeight, outputWidth,
                        kT, kH, kW,
                        pT, pH, pW,
                        dT, dH, dW,
                        dilationT,  dilationH,  dilationW,
                        columns->getBuffer()
                );

                const Nd4jIndex n = columns->shapeOf()[0];   // nOutputPlane * kt * kh * kw
                const Nd4jIndex m = tadInput->shapeOf()[0];   // nInputPlane
                const Nd4jIndex k = columns->shapeOf()[1];

                nd4j::blas::GEMM<T>::op('f', 't', 'n',
                        n, m, k,
                        scale,
                        columns->getBuffer(), k,
                        tadInput->getBuffer(), k,
                        1,
                        gradWeight->getBuffer(), n);

                const Nd4jIndex m_ = nOutputPlane;
                const Nd4jIndex k_ = outputDepth * outputHeight * outputWidth;


                if (gradBias) {
                    nd4j::blas::GEMV<T>::op('t',
                            k_, m_,
                            scale,
                            tadEpsilon->getBuffer(), k_,
                            ones->getBuffer(), 1, (T)1.0f,
                            gradBias->getBuffer(), 1);
                }
            }


            STORE_2_RESULTS(*gradWeight, *gradBias);

            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(fullconv3d_grad) {
            auto list = new ShapeList();

            // _grad ops MUST have output arrays provided

            return list;
        }
		
		//////////////////////////////////////////////////////////////////////////
		DECLARE_CUSTOM_OP(maxpool2d_bp, 2, 1, false, 0, 9) {

            NDArray<T>* input = block.getVariables().at(0)->getNDArray();
            REQUIRE_TRUE(input->rankOf() == 4, 0, "Input should have rank of 4, but got %i instead", input->rankOf());
			NDArray<T>* epsilon = block.getVariables().at(1)->getNDArray();
			NDArray<T>* outEpsilon = this->getZ(block);
			// 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8 - same mode;
			std::vector<int> argI = *(block.getIArguments());			

			int kH = argI[0];
			int kW = argI[1];
			int sH = argI[2];
			int sW = argI[3];
			int pH = argI[4];
			int pW = argI[5];
			int dH = argI[6];
			int dW = argI[7];
			int isSameMode = argI[8];

			int bS = input->getShapeInfo()[1];
			int iD = input->getShapeInfo()[2];
			int iH = input->getShapeInfo()[3];
			int iW = input->getShapeInfo()[4];
			
			// calculate output Height/Width
			int oH, oW;
			nd4j::ops::calcOutHWpool2D(oH, oW, kH, kW, sH, sW, pH, pW, dH, dW, iH, iW, isSameMode);			

			bool cOrderStrides = false;
			bool isEpsilonDup = false;
			if (epsilon->ordering() != 'c') {
				epsilon = epsilon->dup('c');
				cOrderStrides = true;
				isEpsilonDup = true;
			}

			int strideToCompare[] = {oH*oW, iD*oH*oW, oW, 1};
			if (!cOrderStrides && shape::strideDescendingCAscendingF(epsilon->getShapeInfo())) {
				cOrderStrides = true;
			}
			else if (!shape::strideEquals(strideToCompare, 4, epsilon->stridesOf(), epsilon->rankOf())) {
				epsilon = epsilon->dup('c');
				cOrderStrides = true;
				isEpsilonDup = true;
			}

			NDArray<T>* col6d = nullptr;
			NDArray<T>* col6dPermuted = nullptr;
			NDArray<T>* epsilon1d = nullptr;

			if (cOrderStrides) {
				col6d = new NDArray<T>('c', {bS, iD, oH, oW, kH, kW}, block.getWorkspace());
				col6dPermuted = col6d->permute({0, 1, 4, 5, 2, 3});
				epsilon1d = epsilon->reshape('c', {(int) epsilon->lengthOf(), 1}); //zero copy reshape
			}
			else {
				col6d = new NDArray<T>('c', {iD, bS, oH, oW, kH, kW}, block.getWorkspace());
				col6dPermuted = col6d->permute({1, 0, 4, 5, 2, 3});
				NDArray<T>* epsilonTemp = epsilon->permute({1, 0, 2, 3});
				epsilon1d = epsilonTemp->reshape('c', {(int) epsilon->lengthOf(), 1}); //Should be a zero-copy reshape always
				delete epsilonTemp;
			}

			// NDArray<T>* col2d = col6d->reshape('c', {bS*iD*oH*oW, kH*kW}, block.getWorkspace());

			T extraParams1[] = {kW, kH, sW, sH, pW, pH, dW, dH};
			input->template applyTransform<simdOps::Im2col<T>>(col6dPermuted, extraParams1);

			//FIXME: this op should be moved to CustomOps
			// T extraParams2[] = {(T)1.f, (T)1.f};
			// col2d->template applyTransform<simdOps::IsMax<T>>(extraParams2);
			// col2d->muliColumnVector(epsilon1d);		

			// NDArray<T>* tempEpsilon = new NDArray<T>('c', {iD, bS, iH, iW}, block.getWorkspace());
			// NDArray<T>* outEpsilon = tempEpsilon.permute({1, 0, 2, 3});
			T extraParams3[] = {sW, sH, pW, pH, iH, iW, dW, dH};   			// ??? zeros
			col6dPermuted->template applyTransform<simdOps::Col2Im<T>>(outEpsilon, extraParams3);
            
			STORE_RESULT(*outEpsilon);		// ???

			if(isEpsilonDup)
				delete epsilon;
			delete col6d;
			delete col6dPermuted;
			delete epsilon1d;
            // delete col2d;

			return ND4J_STATUS_OK;
        }
        DECLARE_SYN(MaxPool2D_bp, maxpool2d_bp);
        DECLARE_SYN(MaxPool_bp, maxpool2d_bp);

		//////////////////////////////////////////////////////////////////////////
		DECLARE_SHAPE_FN(maxpool2d_bp) {
			int* inShape = inputShape->at(0);
			int bS = inShape[1];
			int iD = inShape[2];
			int iH = inShape[3];
			int iW = inShape[4];
			// calculate output Height/Width
            int* newShapeInfo = nullptr;
            ALLOCATE(newShapeInfo, block.getWorkspace(), 12, int);
			newShapeInfo[0] = 4;		// rank
			newShapeInfo[1] = iD;
			newShapeInfo[2] = bS;
			newShapeInfo[3] = iH;
			newShapeInfo[4] = iW;
            shape::updateStrides(newShapeInfo, 'c');
			int dimensions[] = {1, 0, 2, 3};
			shape::doPermuteShapeBuffer(4, newShapeInfo, dimensions);	
			return new ShapeList(newShapeInfo);
		}	

		//////////////////////////////////////////////////////////////////////////
		DECLARE_CONFIGURABLE_OP(ismax, 1, 1, false, 0, -1) {			

			REQUIRE_OK(this->validateInputLengthMatch(block));
            REQUIRE_OK(this->validateInputDimensionsMatch(block));

			NDArray<T>* x = block.getVariables().at(0)->getNDArray();			
			NDArray<T>* z = this->getZ(block);
			std::vector<int> dimensions = *(block.getIArguments());			// argI

			if (x->isVector()) {
				int dimensionsLength = dimensions.size();
				int length = x->lengthOf();
				if ((x->shapeOf())[dimensions[0]] == 1) {
					for (int i = 0; i < length; i++)
						z->putScalar(i, 1.f);
				}
				else {
					int eleStride = shape::elementWiseStride(x->getShapeInfo());
					if (eleStride == 1) {
						int maxIdx = 0;
						T currMax = x->getScalar(0);
						if (length < ELEMENT_THRESHOLD) {

//#pragma omp simd reduction(max:maxIdx,currMax)
							for (int i = 0; i < length; i++) {
								if (currMax < x->getScalar(i)) {
									currMax = x->getScalar(i);
									maxIdx = i;
								}
								x->putScalar(i, 0.f);
							}
						}
						else {
#pragma omp parallel proc_bind(AFFINITY) default(shared)
{
							int maxIdxLocal = maxIdx;
							T currMaxLocal = currMax;
//#pragma omp simd reduction(max:maxIdxLocal,currMaxLocal)
							for (int i = 0; i < length; i++) {
								if (currMaxLocal < x->getScalar(i)) {
									currMaxLocal = x->getScalar(i);
									maxIdxLocal = i;
								}
								z->putScalar(i, 0.f);
							}
#pragma omp critical
                            {
							    if (currMax < currMaxLocal) {
								    currMax = currMaxLocal;
								    maxIdx = maxIdxLocal;
							    }
                            }
}
						}
						z->putScalar(maxIdx, 1.f);
					}
					else {
						int maxIdx = 0;
						T currMax = x->getScalar(0);
						if (length < ELEMENT_THRESHOLD) {
//#pragma omp parallel for reduction(max:maxIdx,currMax) proc_bind(AFFINITY)
							for (int i = 0; i < length; i++) {
								if (currMax < x->getScalar(i*eleStride)) {
									currMax = x->getScalar(i*eleStride);
									maxIdx = i;
								}
								z->putScalar(i, 0.f);
							}
						}
						else {
#pragma omp parallel proc_bind(AFFINITY) default(shared)
{
							int maxIdxLocal = maxIdx;
							T currMaxLocal = currMax;
//#pragma omp parallel for reduction(max:maxIdx,currMax)  proc_bind(AFFINITY)
							for (int i = 0; i < length; i++) {
								if (currMaxLocal < x->getScalar(i*eleStride)) {
									currMaxLocal = x->getScalar(i*eleStride);
									maxIdxLocal = i;
								}
								z->putScalar(i, 0.f);
							}
#pragma omp critical
{
							if (currMax < currMaxLocal) {
								currMax = currMaxLocal;
								maxIdx = maxIdxLocal;
							}
}
}
						}
						z->putScalar(maxIdx, 1.f);
					}
				}
			}
			else {
                int dimensionsLength = dimensions.size();
//                int tads = tad.numTads;
                //decompose in to several sub tads after
                //moving all dimensions (in sorted order)
                //to the back.
                //permuted version of the x shape info for setting up the tad problem
                shape::TAD tad(x->getShapeInfo(), dimensions.data(), dimensionsLength);
                tad.createTadOnlyShapeInfo();
                tad.createOffsets();
						
                int *tadShapeShapeInfo = tad.tadOnlyShapeInfo;
				Nd4jIndex* tadOffsets = tad.tadOffsets;

                int tadLength = shape::tadLength(x->getShapeInfo(), dimensions.data(), dimensionsLength);
                int tads = x->lengthOf() / tadLength;

                int tadsPerThread = tads / TAD_THRESHOLD;
                int num_threads = nd4j::math::nd4j_max<int>(1, tadsPerThread);
                num_threads = nd4j::math::nd4j_min<int>(num_threads, omp_get_max_threads());

                int tadEWS = shape::elementWiseStride(tadShapeShapeInfo);
                int zEWS = tadEWS;

                int span = (tads / num_threads) + 8;

#pragma omp parallel num_threads(num_threads) if (num_threads>1) proc_bind(AFFINITY)
                {
                    int tid = omp_get_thread_num();
                    int start = span * tid;
                    int end = span * (tid + 1);
                    if (end > tads) end = tads;

                    for (int r = start; r < end; r++) {
                        if (tadEWS > 0 && zEWS > 0 && dimensionsLength == 1) {
                            T *rX = x->getBuffer() + tadOffsets[r];
                            T *rZ = z->getBuffer() + tadOffsets[r];

                            T maxValue = rX[0];
                            int maxIdx = 0;
                            if (tadEWS == 1 && zEWS == 1) {
//#pragma omp simd reduction(max:maxValue,maxIdx)
                                for (int i = 0; i < tadLength; i++) {
                                    if (rX[i] > maxValue) {
                                        maxIdx = i;
                                        maxValue = rX[i];
                                    }
                                }

#pragma omp simd
                                for (int i = 0; i < tadLength; i++) {
                                    rZ[i] = maxIdx == i ? (T) 1.0 : (T) 0.0;
                                }

                            } else {

//#pragma omp parallel for reduction(max:maxValue,maxIdx) default(shared)
                                for (int i = 0; i < tadLength; i++) {
                                    if (rX[i * tadEWS] > maxValue) {
                                        maxIdx = i;
                                        maxValue = rX[i * tadEWS];
                                    }
                                }

#pragma omp simd
                                for (int i = 0; i < tadLength; i++) {
                                    rZ[i * zEWS] = maxIdx == i ? (T) 1.0 : (T) 0.0;
                                }
                            }
                        } else {
                            int tadsPerThread = tads / TAD_THRESHOLD;
                            int num_threads = nd4j::math::nd4j_max<int>(1, tadsPerThread);
                            num_threads = nd4j::math::nd4j_min<int>(num_threads, omp_get_max_threads());

                            int offset = tadOffsets[r];
                            int shapeIter[MAX_RANK];
                            int coord[MAX_RANK];
                            int dim;
                            int xStridesIter[MAX_RANK];
                            int resultStridesIter[MAX_RANK];
                            int *xShape = shape::shapeOf(tadShapeShapeInfo);
                            int *xStride = shape::stride(tadShapeShapeInfo);
                            int *resultStride = shape::stride(tadShapeShapeInfo);
                            int rank = shape::rank(tadShapeShapeInfo);
                            T *xPointer = x->getBuffer() + offset;
                            T *resultPointer = z->getBuffer() + offset;
                            T maxValue = xPointer[0];

                            T *maxCursor = resultPointer;
                            Nd4jPointer maxCursorLong = reinterpret_cast<Nd4jPointer>(maxCursor);
                            if (PrepareTwoRawArrayIter<T>(rank,
                                                             xShape,
                                                             xPointer,
                                                             xStride,
                                                             resultPointer,
                                                             resultStride,
                                                             &rank,
                                                             shapeIter,
                                                             &xPointer,
                                                             xStridesIter,
                                                             &resultPointer,
                                                             resultStridesIter) >= 0) {
                                   ND4J_RAW_ITER_START(dim, rank, coord, shapeIter); {
                                       if (maxValue < xPointer[0]) {
                                           maxCursor = resultPointer;
                                           maxCursorLong = reinterpret_cast<Nd4jPointer>(resultPointer);
                                           maxValue = xPointer[0];
                                       }

                                       resultPointer[0] = 0.0;
                                   }
                                   ND4J_RAW_ITER_TWO_NEXT(dim,
                                                          rank,
                                                          coord,
                                                          shapeIter,
                                                          xPointer,
                                                          xStridesIter,
                                                          resultPointer,
                                                          resultStridesIter);
                                   maxCursor = reinterpret_cast<T *>(maxCursorLong);
                                   maxCursor[0] = 1.0;
                            }
                        }
                    }
                }
            }
			return ND4J_STATUS_OK;
		}
        DECLARE_SYN(IsMax, ismax);        

		//////////////////////////////////////////////////////////////////////////        
        DECLARE_CUSTOM_OP(pooling2d, 1, 1, false, 0, 11) {

			NDArray<T> *x = block.getVariables().at(0)->getNDArray();			
			REQUIRE_TRUE(x->rankOf() == 4, 0, "Input should have rank of 4, but got %i instead", x->rankOf());            
            std::vector<int> argI = *(block.getIArguments());				// 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8 - same mode; 9 - pooling mode; 10 - divisor extraParam0 for pnorm case			
            auto z = this->getZ(block);
			
			int kH = argI[0];
			int kW = argI[1];
			int sH = argI[2];
			int sW = argI[3];
			int pH = argI[4];
			int pW = argI[5];
			int dH = argI[6];			//Dilation, height dimension
			int dW = argI[7];			//Dilation, width dimension
			int poolingMode = argI[9];
			T extraParam0 = (int)argI[10];

			int kSize = kW * kH;

			int *inShape = shape::shapeOf(x->getShapeInfo());
			int *inStride = shape::stride(x->getShapeInfo());

			int samples = inShape[0];
			int depth = inShape[1];
			int height = inShape[2];
			int width = inShape[3];

			int strideex = inStride[0];
			int stridech = inStride[1];
			int strideh = inStride[2];
			int stridew = inStride[3];

			int outH = (z->getShapeInfo())[3];
			int outW = (z->getShapeInfo())[4];			
            int *im2colShapeInfo = new int[16] {6, samples, depth, kH, kW, outH, outW, depth*kH*kW*outH*outW, kH*kW*outH*outW, kW*outH*outW, outH*outW, outW, 1, 0, 1, 99};

            int *outShape = shape::shapeOf(im2colShapeInfo);
            int *outStride = shape::stride(im2colShapeInfo);

			int height_col = outShape[4];
			int width_col = outShape[5];

			int n = samples * depth * height_col * width_col;

			int _threads = omp_get_max_threads();
			int span = (n / _threads) + 1;


#pragma omp parallel num_threads(_threads) proc_bind(close)
            {
				int tid = omp_get_thread_num();
				int start = span * tid;
				int end = span * (tid + 1);
				if (end > n) end = n;
                T res;

                for (int index = start; index < end; index++) {
                    int h_index = index / width_col;
                    int h_col = h_index % height_col;
                    int w_col = index % width_col;

                    int c_im = h_index / height_col;
                    int c_col = c_im * kSize;

                    int depth_im = c_im % depth;
                    int num_im = c_im / depth;
                    int h_offset = h_col * sH - pH;
                    int w_offset = w_col * sW - pW;

                    T *data_col_ptr = z->getBuffer();

                    int i_c = (c_col * height_col + h_col) * width_col + w_col;
                    data_col_ptr += (c_col * height_col + h_col) * width_col + w_col;

                    T *data_im_ptr = x->getBuffer();

                    data_im_ptr += num_im * strideex + depth_im * stridech + h_offset * strideh + w_offset * stridew;
                    res = poolingMode == 0 ? (T) -MAX_FLOAT : (T) 0.0f;

                    for (int i = 0; i < kH; ++i) {
                        for (int j = 0; j < kW; ++j) {
                            int h_im = h_offset + i * dH;
                            int w_im = w_offset + j * dW;
                            int i_f = 0;
                            int i_c_temp = i_c;
                            for (int dim = 5; dim >= 0; dim--) {
                                i_f += (i_c_temp % outShape[dim]) * outStride[dim];
                                i_c_temp = i_c_temp / outShape[dim];
                            }

                            T val;
                            if (h_im >= 0 && w_im >= 0 && h_im < height && w_im < width)
                                val = data_im_ptr[i * dH * strideh + j * dW * stridew];
                            else
                                val = (T) 0.0f;

                            //kernel[i * kH + j] = val;
                            // max
                            if (poolingMode == 0) {
                                if (res < val)
                                    res = val;
                            // avg
                            } else if (poolingMode == 1) {
                                res += val;

                            // phorm
                            } else if (poolingMode == 2) {
                                res += nd4j::math::nd4j_pow<T>(nd4j::math::nd4j_abs<T>(val), extraParam0);
                            }

                            //result[i_f] = (h_im >= 0 && w_im >= 0 && h_im < height && w_im < width) ? data_im_ptr[i * strideh + j*stridew] : 0;
                            data_col_ptr += height_col * width_col;
                            i_c += height_col * width_col;
                        }
                    }

                    // avg final step
                    if (poolingMode == 1) {
                        res /= kSize;

                    // pnorm final step
                    } else if (poolingMode == 2) {
                        res = nd4j::math::nd4j_pow<T>(res, (T) 1.0f /  extraParam0);
                    }

                    z->putScalar(index,res);
                }
            }
			delete im2colShapeInfo;
			return ND4J_STATUS_OK;
		}
		DECLARE_SYN(Pooling2D, pooling2d);
		
		//////////////////////////////////////////////////////////////////////////
		DECLARE_SHAPE_FN(pooling2d) {
			int* inShape = inputShape->at(0);            
			// 0,1 - kernel Height/Width; 2,3 - stride Height/Width; 4,5 - pad Height/Width; 6,7 - dilation Height/Width; 8 - same mode; 9 - pooling mode; 
			std::vector<int> argI = *(block.getIArguments());			
			int kH = argI[0];
			int kW = argI[1];
			int sH = argI[2];
			int sW = argI[3];
			int pH = argI[4];
			int pW = argI[5];
			int dH = argI[6];
			int dW = argI[7];
			int isSameMode = argI[8];

            int bS = inShape[1];
            int iD = inShape[2];
            int iH = inShape[3];
            int iW = inShape[4];
			
			char order = shape::order(inShape); // output order must be equal to input order

			// calculate output Height/Width
			int oH, oW;
			nd4j::ops::calcOutHWpool2D(oH, oW, kH, kW, sH, sW, pH, pW, dH, dW, iH, iW, isSameMode);
			// allocate memory for new shape
            int* newShapeInfo = nullptr;
            ALLOCATE(newShapeInfo, block.getWorkspace(), 12, int);
			newShapeInfo[0] = 4;		// rank
			newShapeInfo[1] = bS;
			newShapeInfo[2] = iD;
			newShapeInfo[3] = oH;
			newShapeInfo[4] = oW;
            shape::updateStrides(newShapeInfo, order);

			return new ShapeList(newShapeInfo);
		}
		
		//////////////////////////////////////////////////////////////////////////
		DECLARE_CUSTOM_OP(avgpool2d_bp, 2, 1, false, 0, 9) {
			
            NDArray<T>* input = block.getVariables().at(0)->getNDArray();
			REQUIRE_TRUE(input->rankOf() == 4, 0, "Input should have rank of 4, but got %i instead", input->rankOf());
			NDArray<T>* epsilon = block.getVariables().at(1)->getNDArray();
			NDArray<T>* outEpsilon = this->getZ(block);
			std::vector<int> argI = *(block.getIArguments());
			
			int kH = argI[0];
			int kW = argI[1];
			int sH = argI[2];
			int sW = argI[3];
			int pH = argI[4];
			int pW = argI[5];
			int dH = argI[6];
			int dW = argI[7];
			int isSameMode = argI[8];
			
			int bS = input->getShapeInfo()[1];
			int iD = input->getShapeInfo()[2];
			int iH = input->getShapeInfo()[3];
			int iW = input->getShapeInfo()[4];
						
			// calculate output Height/Width
			int oH, oW;
			nd4j::ops::calcOutHWpool2D(oH, oW, kH, kW, sH, sW, pH, pW, dH, dW, iH, iW, isSameMode);			

			bool cOrderStrides = false;
			bool isEpsilonDup = false;
			if (epsilon->ordering() != 'c') {
				epsilon = epsilon->dup('c');
				cOrderStrides = true;
				isEpsilonDup = true;
			}

			int strideToCompare[] = {oH*oW, iD*oH*oW, oW, 1};
			if (!cOrderStrides && shape::strideDescendingCAscendingF(epsilon->getShapeInfo())) {
				cOrderStrides = true;
			}
			else if (!shape::strideEquals(strideToCompare, 4, epsilon->stridesOf(), epsilon->rankOf())) {
				epsilon = epsilon->dup('c');
				cOrderStrides = true;
				isEpsilonDup = true;
			}

			NDArray<T>* col6d = nullptr;
			NDArray<T>* col6dPermuted = nullptr;
			NDArray<T>* epsilon1d = nullptr;

			if (cOrderStrides) {
				col6d = new NDArray<T>('c', {bS, iD, oH, oW, kH, kW}, block.getWorkspace());
				col6dPermuted = col6d->permute({0, 1, 4, 5, 2, 3});
				epsilon1d = epsilon->reshape('c', {(int) epsilon->lengthOf(), 1}); //zero copy reshape
			}
			else {
				col6d = new NDArray<T>('c', {iD, bS, oH, oW, kH, kW}, block.getWorkspace());
				col6dPermuted = col6d->permute({1, 0, 4, 5, 2, 3});
				NDArray<T>* epsilonTemp = epsilon->permute({1, 0, 2, 3});
				epsilon1d = epsilonTemp->reshape('c', {(int) epsilon->lengthOf(), 1}); //Should be a zero-copy reshape always
				delete epsilonTemp;
			}

			// NDArray<T>* col2d = col6d->reshape('c', {bS*iD*oH*oW, kH*kW}, block.getWorkspace());			
			// col2d->addiColumnVector(epsilon1d);		

			// NDArray<T>* tempEpsilon = new NDArray<T>('c', {iD, bS, iH, iW}, block.getWorkspace());
			// NDArray<T>* outEpsilon = tempEpsilon.permute({1, 0, 2, 3});
			T extraParams3[] = {sW, sH, pW, pH, iH, iW, dW, dH};   			// ??? zeros
			col6dPermuted->template applyTransform<simdOps::Col2Im<T>>(outEpsilon, extraParams3);
            outEpsilon->template applyScalar<simdOps::Divide<T>>((T) kH*kW, outEpsilon);

			STORE_RESULT(*outEpsilon);

			if(isEpsilonDup)
				delete epsilon;
			delete col6d;
			delete col6dPermuted;
			delete epsilon1d;
            // delete col2d;

			return ND4J_STATUS_OK;
        }

		//////////////////////////////////////////////////////////////////////////
		DECLARE_SHAPE_FN(avgpool2d_bp) {
			int* inShape = inputShape->at(0);            
            int bS = inShape[1];
            int iD = inShape[2];
            int iH = inShape[3];
            int iW = inShape[4];

			// calculate output Height/Width
            int* newShapeInfo = nullptr;
            ALLOCATE(newShapeInfo, block.getWorkspace(), 12, int);
			newShapeInfo[0] = 4;		// rank
			newShapeInfo[1] = iD;
			newShapeInfo[2] = bS;
			newShapeInfo[3] = iH;
			newShapeInfo[4] = iW;
            shape::updateStrides(newShapeInfo, 'c');
			int dimensions[] = {1, 0, 2, 3};
			shape::doPermuteShapeBuffer(4, newShapeInfo, dimensions);	
			return new ShapeList(newShapeInfo);
		}	

		//////////////////////////////////////////////////////////////////////////
		DECLARE_CUSTOM_OP(pnormpool2d_bp, 2, 1, false, 1, 10) {
			
            NDArray<T>* input = block.getVariables().at(0)->getNDArray();
			NDArray<T>* epsilon = block.getVariables().at(1)->getNDArray();
			NDArray<T>* outEpsilon = this->getZ(block);
			std::vector<int> argI = *(block.getIArguments());
			std::vector<T>   argT = *(block.getTArguments());
			
			int kH = argI[0];
			int kW = argI[1];
			int sH = argI[2];
			int sW = argI[3];
			int pH = argI[4];
			int pW = argI[5];
			int dH = argI[6];
			int dW = argI[7];			
			int isSameMode = argI[8];
			int pnorm = argI[9];
			T eps = argT[0];
		
			int bS = input->getShapeInfo()[1];
			int iD = input->getShapeInfo()[2];
			int iH = input->getShapeInfo()[3];
			int iW = input->getShapeInfo()[4];
			
			// calculate output Height/Width
			int oH, oW;
			nd4j::ops::calcOutHWpool2D(oH, oW, kH, kW, sH, sW, pH, pW, dH, dW, iH, iW, isSameMode);			

			bool cOrderStrides = false;
			bool isEpsilonDup = false;
			if (epsilon->ordering() != 'c') {
				epsilon = epsilon->dup('c');
				cOrderStrides = true;
				isEpsilonDup = true;
			}

			int strideToCompare[] = {oH*oW, iD*oH*oW, oW, 1};
			if (!cOrderStrides && shape::strideDescendingCAscendingF(epsilon->getShapeInfo())) {
				cOrderStrides = true;
			}
			else if (!shape::strideEquals(strideToCompare, 4, epsilon->stridesOf(), epsilon->rankOf())) {
				epsilon = epsilon->dup('c');
				cOrderStrides = true;
				isEpsilonDup = true;
			}

			NDArray<T>* col6d = nullptr;
			NDArray<T>* col6dPermuted = nullptr;
			NDArray<T>* epsilon1d = nullptr;

			if (cOrderStrides) {
				col6d = new NDArray<T>('c', {bS, iD, oH, oW, kH, kW}, block.getWorkspace());
				col6dPermuted = col6d->permute({0, 1, 4, 5, 2, 3});
				epsilon1d = epsilon->reshape('c', {(int) epsilon->lengthOf(), 1}); //zero copy reshape
			}
			else {
				col6d = new NDArray<T>('c', {iD, bS, oH, oW, kH, kW}, block.getWorkspace());
				col6dPermuted = col6d->permute({1, 0, 4, 5, 2, 3});
				NDArray<T>* epsilonTemp = epsilon->permute({1, 0, 2, 3});
				epsilon1d = epsilonTemp->reshape('c', {(int) epsilon->lengthOf(), 1}); //Should be a zero-copy reshape always
				delete epsilonTemp;
			}

			NDArray<T>* col2d = col6d->reshape('c', {bS*iD*oH*oW, kH*kW});

			T extraParams1[] = {kW, kH, sW, sH, pW, pH, dW, dH};
			input->template applyTransform<simdOps::Im2col<T>>(col6dPermuted, extraParams1);
						
			NDArray<T>* pNorm = new NDArray<T>(col2d->getShapeInfo(), block.getWorkspace()); 
            col2d->template applyTransform<simdOps::Abs<T>>(pNorm, nullptr); 
			
			T extraParams11[] = {(T)pnorm};
			pNorm->template applyTransform<simdOps::Pow<T>>(extraParams11);
			*pNorm = *(pNorm->sum({1}));
			T extraParams2[] = {1.f/pnorm};
			pNorm->template applyTransform<simdOps::Pow<T>>(extraParams2);
			
			NDArray<T>* numerator = new NDArray<T>(col2d->getShapeInfo(), block.getWorkspace());
            if (pnorm != 2) {
				NDArray<T>* absp2 = new NDArray<T>(col2d->getShapeInfo(), block.getWorkspace());
				col2d->template applyTransform<simdOps::Abs<T>>(absp2, nullptr);
				T extraParams3[] = {pnorm - 2};
				absp2->template applyTransform<simdOps::Pow<T>>(extraParams3);
				nd4j::NDArrayFactory::mmulHelper(col2d, absp2, numerator, (T)1.f, (T)0.f);                
				delete absp2;
            }
			NDArray<T>* denom = new NDArray<T>(pNorm->getShapeInfo(), block.getWorkspace()); 
			T extraParams4[] = {pnorm - 1};

			pNorm->template applyTransform<simdOps::Pow<T>>(denom, extraParams4);
            denom->template applyScalar<simdOps::Max<T>>(eps); // in case of 0
			denom->template applyPairwiseTransform<simdOps::Divide<T>>(epsilon1d, denom, nullptr);
			numerator->muliColumnVector(denom);

			// NDArray<T>* tempEpsilon = new NDArray<T>('c', {iD, bS, iH, iW});
			// NDArray<T>* outEpsilon = tempEpsilon.permute({1, 0, 2, 3});
			T extraParams5[] = {sW, sH, pW, pH, iH, iW, dW, dH};   			// ??? zeros
			col6dPermuted->template applyTransform<simdOps::Col2Im<T>>(outEpsilon, extraParams5);

			STORE_RESULT(*outEpsilon);

			if(isEpsilonDup)
				delete epsilon;
			delete col6d;
			delete col6dPermuted;
			delete epsilon1d;
			delete pNorm;	
			delete numerator;
			delete denom;
            delete col2d;

			return ND4J_STATUS_OK;
        }

		//////////////////////////////////////////////////////////////////////////
		DECLARE_SHAPE_FN(pnormpool2d_bp) {

			int* inShape = inputShape->at(0);            
            int bS = inShape[1];
            int iD = inShape[2];
            int iH = inShape[3];
            int iW = inShape[4];

			// calculate output Height/Width
            int* newShapeInfo = nullptr;
            ALLOCATE(newShapeInfo, block.getWorkspace(), 12, int);
			newShapeInfo[0] = 4;		// rank
			newShapeInfo[1] = iD;
			newShapeInfo[2] = bS;
			newShapeInfo[3] = iH;
			newShapeInfo[4] = iW;
            shape::updateStrides(newShapeInfo, 'c');
			int dimensions[] = {1, 0, 2, 3};
			shape::doPermuteShapeBuffer(4, newShapeInfo, dimensions);	
			return new ShapeList(newShapeInfo);
		}	
		
		

    }
}

#endif //LIBND4J_CONVO_OPS_H
