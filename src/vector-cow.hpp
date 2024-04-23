#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

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
  Vector(const Vector &other)
      : begin_(other.begin_), size_(other.size_), capacity_(other.capacity_) {}

  Vector(Vector &&other)
      : begin_(other.begin_), size_(other.size_), capacity_(other.capacity_) {
    other.begin_ = nullptr;
    other.size_ = other.capacity_ = 0;
  }

  Vector &operator=(const Vector &other) {
    if (this != &other) {
      other.begin_ = this->begin_;
      other.size_ = this->size_;
      other.capacity_ = this->capacity_;
    }
    return *this;
  }

  Vector &operator=(Vector &&other) {
    Vector tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  };

  ~Vector() = default;

  explicit Vector(size_type sizes)
      : begin_(static_cast<T *>(operator new(sizeof(T) * sizes))), size_(sizes),
        capacity_(sizes) {
    create_objects(0, size_, 0);
  }

  Vector(size_type size, const_reference value)
      : begin_(static_cast<T *>(operator new(sizeof(T) * size))), size_(size),
        capacity_(size) {
    create_objects(0, size_, value);
  }

  explicit Vector(std::initializer_list<T> items)
      : begin_(static_cast<T *>(operator new(sizeof(T) * items.size()))),
        size_(items.size()), capacity_(size_) {
    std::uninitialized_move(items.begin(), items.end(), begin_.get());
  }

  // Vector(iterator first, iterator end);

  void reserve(size_type n) {
    if (n <= capacity_) {
      return;
    }
    std::shared_ptr<T> tmp(static_cast<T *>(operator new(sizeof(T) * n)));
    try {
      for (size_t i = 0; i < size_; ++i) {
        new (tmp.get() + i) T(*(begin_.get() + i));
      }
    } catch (const std::exception &e) {
      throw;
    }
    begin_.swap(tmp);
    capacity_ = n;
  }

  void push_back(const_reference value) {
    if (begin_.use_count() > 1 && size_ < capacity_) {
      detach();
    }
    if (size_ == 0) {
      reserve(1);
    } else if (size_ == capacity_) {
      reserve(size_ * 2);
    }
    new (begin_.get() + size_) T(value);
    ++size_;
  }

  void resize(size_type count) {
    if (count > size_) {
      if (count > capacity_) {
        reserve(count);
      }
      create_objects(size_, count, 0);
    } else if (count < size_) {
      for (size_t i = size_; i > count; --i) {
        begin_.get()[i].~T();
      }
    } else {
      return;
    }
    size_ = count;
  }

  reference operator[](size_type pos) {
    if (begin_.use_count() > 1) {
      detach();
    }
    return begin_.get()[pos];
  }

  const_reference operator[](size_type pos) const { return begin_.get()[pos]; }

  reference at(size_type pos) {
    if (pos >= size_) {
      throw std::out_of_range("Out of range!");
    }
    if (begin_.use_count() > 1) {
      detach();
    }
    return begin_.get()[pos];
  }

  const_reference at(size_type pos) const {
    if (pos >= size_) {
      throw std::out_of_range("Out of range!");
    }
    return begin_.get()[pos];
  }

  size_type size() const noexcept { return size_; }
  size_type capacity() const noexcept { return capacity_; }

  bool empty() const noexcept { return (size_ == 0); }

  void shrink_to_fit() {
    std::shared_ptr<T> v_new(static_cast<T *>(operator new(sizeof(T) * size_)));
    size_t current = 0;
    try {
      for (; current < size_; ++current) {
        new (v_new.get() + current) T(*(begin_.get() + current));
      }
    } catch (const std::exception &e) {
      for (size_t i = 0; i < current; ++i) {
        begin_.get()[i].~T();
      }
      throw;
    }
    begin_.swap(v_new);
    capacity_ = size_;
  }

  template <typename It> class Iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = It;
    using reference = value_type &;
    using pointer = value_type *;
    using iterator_category = std::random_access_iterator_tag;

    explicit Iterator(Vector::pointer arr) : arr_(arr) {}
    auto &operator++() noexcept {
      ++arr_;
      return *this;
    }
    auto operator++(int) noexcept {
      auto temp = *this;
      ++(*this);
      return temp;
    }
    auto &operator--() noexcept {
      --arr_;
      return *this;
    }
    auto operator--(int) noexcept {
      auto temp = *this;
      --(*this);
      return temp;
    }

    reference operator*() const noexcept { return *arr_; }
    pointer operator->() const noexcept { return arr_; }

    bool operator==(const Iterator &other) const noexcept {
      return arr_ == other.arr_;
    }
    bool operator!=(const Iterator &other) const noexcept {
      return !(arr_ == other.arr_);
    }
    friend bool operator<(const Iterator &lhs, const Iterator &rhs) noexcept {
      return (rhs - lhs > 0);
    }
    friend bool operator>(const Iterator &lhs, const Iterator &rhs) noexcept {
      return rhs < lhs;
    }
    friend bool operator<=(const Iterator &lhs, const Iterator &rhs) noexcept {
      return !(rhs < lhs);
    }
    friend bool operator>=(const Iterator &lhs, const Iterator &rhs) noexcept {
      return !(lhs < rhs);
    }

    reference operator[](difference_type n) const { return *(arr_ + n); }
    Iterator &operator+=(difference_type n) {
      if (n >= 0)
        while (n--)
          ++arr_;
      else
        while (n++)
          --arr_;
      return *this;
    }
    Iterator &operator-=(difference_type n) { return *this += -n; }

    friend Iterator operator+(Iterator it, difference_type n) {
      return it += n;
    }
    friend Iterator operator+(difference_type n, Iterator it) { return it + n; }
    friend Iterator operator-(Iterator it, difference_type n) {
      it -= n;
      return it;
    }
    friend difference_type operator-(const Iterator &lhs, const Iterator &rhs) {
      return lhs.arr_ - rhs.arr_;
    }

  private:
    Vector::pointer arr_;
  };

  using iterator = Iterator<value_type>;
  using const_iterator = Iterator<const value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() noexcept {
    if (begin_.use_count() > 1) {
      detach();
    }
    return iterator(begin_.get());
  }
  iterator end() noexcept {
    if (begin_.use_count() > 1) {
      detach();
    }
    return iterator(begin_.get() + size_);
  }
  const_iterator begin() const noexcept { return const_iterator(begin_.get()); }
  const_iterator end() const noexcept {
    return const_iterator(begin_.get() + size_);
  }
  const_iterator cbegin() const noexcept {
    return const_iterator(begin_.get());
  }
  const_iterator cend() const noexcept {
    return const_iterator(begin_.get() + size_);
  }
  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

private:
  void detach() {
    std::shared_ptr<T> v_new(
        static_cast<T *>(operator new(sizeof(T) * capacity_)));
    size_t current = 0;
    try {
      for (; current < size_; ++current) {
        new (v_new.get() + current) T(*(begin_.get() + current));
      }
    } catch (const std::exception &e) {
      for (size_t i = 0; i < current; ++i) {
        begin_.get()[i].~T();
      }
      throw;
    }
    begin_.swap(v_new);
  }

  void swap(Vector &obj) noexcept {
    obj.begin_ = this->begin_;
    obj.size_ = this->size_;
    obj.capacity_ = this->capacity_;
  }

  void create_objects(size_t start, size_t end, value_type value) {
    size_t current = start;
    try {
      for (; current < end; ++current) {
        new (begin_.get() + current) T(value);
      }
    } catch (const std::exception &e) {
      for (size_t i = 0; i < current; ++i) {
        begin_.get()[i].~T();
      }
      throw;
    }
  }

  std::shared_ptr<T> begin_ = nullptr;
  size_type size_ = 0;
  size_type capacity_ = 0;
};

template <typename T>
inline bool operator==(const Vector<T> &lhs, const Vector<T> &rhs) {
  return (
      lhs.size() == rhs.size() &&
      std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

} // namespace vector_cow