#include <vector-cow.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <string>

TEST(test_vector, initializer_constructor) {
  vector_cow::Vector<std::string> v(2, "hello");
  for (auto it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << '\n' << v.capacity() << ' ' << v.size() << '\n';
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}