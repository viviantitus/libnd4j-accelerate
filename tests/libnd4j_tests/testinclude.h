//
// Created by agibsonccc on 1/15/17.
//

#ifndef LIBND4J_TESTINCLUDE_H
#define LIBND4J_TESTINCLUDE_H
#include <reduce.h>
#include <reduce3.h>
#include <shape.h>
#include <data_gen.h>
#include <cnpy.h>
#include <gtest/gtest.h>
#include <NativeOps.h>
//http://stackoverflow.com/questions/228005/alternative-to-itoa-for-converting-integer-to-string-c
std::string int_array_to_string(int int_array[], int size_of_array) {
    std::string returnstring = "[";
    for (int temp = 0; temp < size_of_array; temp++) {
        returnstring += std::to_string(int_array[temp]);
        if(temp < size_of_array - 1)
            returnstring += ",";
    }
    returnstring += "]";
    return returnstring;
}

::testing::AssertionResult arrsEquals(int n,int *assertion,int *other) {
    for(int i = 0; i < n; i++) {
        if(assertion[i] != other[i]) {
            std::string message = std::string("Failure at index  ") + std::to_string(i)  + std::string(" assertion: ") +  int_array_to_string(assertion,n) + std::string(" and test array ") + int_array_to_string(other,n) + std::string("  is not equal");
            return ::testing::AssertionFailure() << message;
        }

    }
    return ::testing::AssertionSuccess();

}


#endif //LIBND4J_TESTINCLUDE_H
