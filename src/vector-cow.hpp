#pragma once

#include <cstddef>
#include <initializer_list>

namespace vector_cow {

template <typename T> class Vector {
public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = const value_type *;

  Vector() = default;
  Vector(const Vector &other);
  Vector(Vector &&other);
  Vector &operator=(const Vector &other);
  Vector &operator=(Vector &&other);
  ~Vector();

  Vector(size_type size);
  Vector(size_type size, const_reference value);
  Vector(std::initializer_list<T> other);
  //   Vector(iterator first, iterator end);

private:
  pointer begin_ = nullptr;
  size_type size_ = 0;
  size_type capacity_ = 0;
};

} // namespace vector_cow