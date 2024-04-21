#include <vector-cow.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <string>

// TEST(test_vector, initializer_constructor) {
//   vector_cow::Vector<std::string> v(2, "hello");
//   for (auto it = v.begin(); it != v.end(); ++it) {
//     std::cout << *it << ' ';
//   }
//   std::cout << '\n' << v.capacity() << ' ' << v.size() << '\n';
// }

TEST(test_vector, reserve) {
  vector_cow::Vector<std::string> v(2, "hello");
  v.reserve(5);
  v.reserve(2);
  ASSERT_EQ(v.capacity(), 5);
}

TEST(test_vector, push_back) {
  vector_cow::Vector<int> v(2, 5);
  v.push_back(2);
  vector_cow::Vector<int> expect{5, 5, 2};
  // ASSERT_EQ(v, expect);

  // for (auto it1 = v.begin(); it1 != v.end(); ++it1) {
  // std::cout << *it1 << '\n';
  // }
}

TEST(test_vector, resize) {
  vector_cow::Vector<int> v(2, 5);
  v.resize(5);

  // for (auto it1 = v.begin(); it1 != v.end(); ++it1) {
  // std::cout << *it1 << '\n';
  // }
}

TEST(test_vector, access_operator) {
  {
    vector_cow::Vector<int> v(4, 2);
    v[2] = 4;
    // for (auto it1 = v.begin(); it1 != v.end(); ++it1) {
    //   std::cout << *it1 << '\n';
    // }
  }
  {
    const vector_cow::Vector<int> v(4, 2);
    // std::cout << '\n' << v[2];
  }
}

TEST(test_vector, access_at) {
  {
    vector_cow::Vector<int> v(4, 2);
    v.at(2) = 4;
    // for (auto it1 = v.begin(); it1 != v.end(); ++it1) {
    //   std::cout << *it1 << '\n';
    // }
  }
  { const vector_cow::Vector<int> v(4, 2); }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}