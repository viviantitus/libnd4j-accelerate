//
// Created by agibsonccc on 1/17/17.
//
#include "testinclude.h"
#include <reduce3.h>

class EqualsTest : public testing::Test {
public:
    int firstShapeBuffer[8] = {2,1,2,1,1,0,1,102};
    float data[2] = {1.0,7.0};
    int secondShapeBuffer[8] = {2,2,1,6,1,0,6,99};
    float dataSecond[12] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0};
    int opNum = 4;
    float extraArgs[1] = {1e-6};
    int dimension[1] = {2147483647};
    int dimensionLength = 1;
};


TEST_F(EqualsTest,Eps) {
    float val = functions::reduce3::Reduce3<float>::execScalar(opNum,
                                                               data,
                                                               firstShapeBuffer,
                                                               extraArgs,
                                                               dataSecond,
                                                               secondShapeBuffer);
    ASSERT_TRUE(val < 0.5);
}
