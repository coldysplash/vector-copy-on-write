#include <vector-cow.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <string>

TEST(test_vector, initializer_constructor) {
  vector_cow::Vector<std::string> v{"hello", "hello"};
  vector_cow::Vector<std::string> v_2(2, "hello");
  ASSERT_EQ(v, v_2);
  ASSERT_EQ(v.size(), 2);
  ASSERT_EQ(v.capacity(), 2);
}

TEST(test_vector, size_constructor) {
  vector_cow::Vector<int> v(5);
  vector_cow::Vector<int> v_2{0, 0, 0, 0, 0};
  ASSERT_EQ(v, v_2);
}

TEST(test_vector, constructor_from_range) {
  vector_cow::Vector<int> v{5, 6, 7, 8};
  vector_cow::Vector<int> v_2(v.begin(), v.end());
  ASSERT_EQ(v, v_2);
}

TEST(test_vector, reserve) {
  vector_cow::Vector<std::string> v;
  v.reserve(5);
  ASSERT_EQ(v.capacity(), 5);
  ASSERT_EQ(v.size(), 0);
}

TEST(test_vector, push_back) {
  vector_cow::Vector<int> v;
  v.push_back(2);
  ASSERT_EQ(*v.begin(), 2);
}

TEST(test_vector, resize) {
  vector_cow::Vector<int> v(2, 5);
  v.resize(5);
  vector_cow::Vector<int> expect{5, 5, 0, 0, 0};
  ASSERT_EQ(v, expect);
  v.resize(0);
  ASSERT_EQ(v.size(), 0);
  ASSERT_EQ(v.capacity(), 5);
}

TEST(test_vector, access_operator) {
  {
    vector_cow::Vector<int> v(3, 2);
    v[2] = 4;
    vector_cow::Vector<int> expect{2, 2, 4};
    ASSERT_EQ(v, expect);
  }
  {
    const vector_cow::Vector<int> v(4, 2);
    // std::cout << '\n' << v[2];
  }
}

TEST(test_vector, access_at) {
  {
    vector_cow::Vector<int> v(3, 2);
    v.at(2) = 4;
    vector_cow::Vector<int> expect{2, 2, 4};
    ASSERT_EQ(v, expect);
  }
  { const vector_cow::Vector<int> v(4, 2); }
}

TEST(test_vector, empty) {
  vector_cow::Vector<int> v;
  ASSERT_EQ(v.empty(), true);
  v.push_back(2);
  ASSERT_EQ(v.empty(), false);
}

TEST(test_vector, shrink_to_fit) {
  vector_cow::Vector<int> v;
  v.resize(100);
  v.resize(50);
  v.shrink_to_fit();
  ASSERT_EQ(v.capacity(), 50);
}

TEST(test_vector, iterator) {
  using int_iterator = vector_cow::Vector<int>::iterator;

  // +=, +
  {
    vector_cow::Vector<int> v{1, 2, 3, 4, 5, 6};
    int_iterator it = v.begin() + 2;
    it += 1;
    ASSERT_EQ(*it, 4);
  }

  // -=, -
  {
    vector_cow::Vector<int> v{1, 2, 3, 4, 5, 6};
    int_iterator it = v.end() - 2;
    it -= 3;
    ASSERT_EQ(*it, 2);

    int_iterator it_2 = v.end();
    std::ptrdiff_t res = it_2 - it;
    ASSERT_EQ(res, 5);
  }

  //[]
  {
    vector_cow::Vector<int> v{1, 2, 3, 4, 5, 6};
    int_iterator it = v.begin();
    ASSERT_EQ(it[2], 3);
    it[2] = 4;
    ASSERT_EQ(it[2], 4);
  }

  // compare
  {
    vector_cow::Vector<int> v{1, 2, 3, 4, 5, 6};
    int_iterator it = v.begin();
    ASSERT_EQ((it == v.begin()), true);
    ASSERT_EQ((it == v.end() - 1), false);
    ASSERT_EQ((it < v.begin() + 2), true);
    ASSERT_EQ((it + 4 > v.begin() + 2), true);
  }
}

TEST(test_vector, insert_1) {
  {
    vector_cow::Vector<int> v{1, 2, 3, 4};
    vector_cow::Vector<int>::iterator it = v.insert(v.cend(), 7);
    ASSERT_EQ(*(it), 7);
  }
  {
    vector_cow::Vector<int> v(2, 100);
    auto it = v.begin();
    v.insert(it + 1, 200);
    ASSERT_EQ(v, (vector_cow::Vector<int>{100, 200, 100}));
  }
}

TEST(test_vector, insert_2) {
  {
    vector_cow::Vector<int> v(2, 100);
    auto it = v.end();
    v.insert(it, 2, 200);
    ASSERT_EQ(v, (vector_cow::Vector<int>{100, 100, 200, 200}));
  }
  {
    vector_cow::Vector<int> v(2, 100);
    auto it = v.begin();
    it = v.insert(it, 2, 200);
    ASSERT_EQ(v, (vector_cow::Vector<int>{200, 200, 100, 100}));
  }
}

TEST(test_vector, insert_3) {
  {
    vector_cow::Vector<int> v(2, 100);
    vector_cow::Vector<int> v_1(2, 200);
    auto it = v.insert(v.end(), v_1.begin(), v_1.end());
    ASSERT_EQ(*it, 200);
    ASSERT_EQ(v, (vector_cow::Vector<int>{100, 100, 200, 200}));
  }
  {
    vector_cow::Vector<int> v(2, 100);
    vector_cow::Vector<int> v_1(2, 200);
    v.insert(v.begin(), v_1.begin(), v_1.end());
    ASSERT_EQ(v, (vector_cow::Vector<int>{200, 200, 100, 100}));
  }
}

TEST(test_vector, insert_4) {
  {
    vector_cow::Vector<int> v(2, 100);
    auto it = v.insert(v.end(), {200, 200});
    ASSERT_EQ(*it, 200);
    ASSERT_EQ(v, (vector_cow::Vector<int>{100, 100, 200, 200}));
  }
  {
    vector_cow::Vector<int> v(2, 100);
    v.insert(v.begin(), {200, 200});
    ASSERT_EQ(v, (vector_cow::Vector<int>{200, 200, 100, 100}));
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}