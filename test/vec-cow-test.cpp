#include <vector-cow.hpp>

#include <gtest/gtest.h>

#include <iostream>

TEST(test_vector, initializer_constructor) {
  vector_cow::Vector<int> v{1, 2, 3};
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}