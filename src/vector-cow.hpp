#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

template <typename S> struct Storage {
  S *begin_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;

  Storage(size_t size, size_t capa) : size_(size), capacity_(capa) {
    begin_ = static_cast<S *>(operator new(sizeof(S) * capacity_));
  }
  ~Storage() {
    for (std::size_t i = 0; i < size_; ++i) {
      begin_[i].~S();
    }
    operator delete(begin_);
  }
};

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

  Vector() { data_ = std::make_shared<Storage<value_type>>(0, 0); }
  Vector(const Vector &other) : data_(other.data_) {}
  Vector(Vector &&other) : data_(other.data_) { other.data_.reset(); }

  Vector &operator=(const Vector &other) {
    if (this != &other) {
      other.data_ = this->data_;
    }
    return *this;
  }

  Vector &operator=(Vector &&other) {
    Vector tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  };

  ~Vector() = default;

  // Iterator
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
  using const_iterator = const Iterator<value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() noexcept {
    if (data_.use_count() > 1) {
      detach();
    }
    return iterator(data_->begin_);
  }
  iterator end() noexcept {
    if (data_.use_count() > 1) {
      detach();
    }
    return iterator(data_->begin_ + size());
  }
  const_iterator begin() const noexcept {
    return const_iterator(data_->begin_);
  }
  const_iterator end() const noexcept {
    return const_iterator(data_->begin_ + size());
  }
  const_iterator cbegin() const noexcept {
    return const_iterator(data_->begin_);
  }
  const_iterator cend() const noexcept {
    return const_iterator(data_->begin_ + size());
  }
  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  explicit Vector(size_type size)
      : data_(std::make_shared<Storage<value_type>>(size, size)) {
    size_t current = 0;
    try {
      for (; current < size; ++current) {
        new (data_->begin_ + current) T(0);
      }
    } catch (const std::exception &e) {
      for (size_t i = 0; i < current; ++i) {
        data_->begin_[i].~T();
        operator delete(data_->begin_);
      }
      throw;
    }
  }

  Vector(size_type size, const_reference value)
      : data_(std::make_shared<Storage<value_type>>(size, size)) {
    size_t current = 0;
    try {
      for (; current < size; ++current) {
        new (data_->begin_ + current) T(value);
      }
    } catch (const std::exception &e) {
      for (size_t i = 0; i < current; ++i) {
        data_->begin_[i].~T();
        operator delete(data_->begin_);
      }
      throw;
    }
  }

  explicit Vector(std::initializer_list<T> items)
      : data_(
            std::make_shared<Storage<value_type>>(items.size(), items.size())) {
    std::uninitialized_move(items.begin(), items.end(), data_->begin_);
  }

  Vector(iterator first, iterator end) {
    size_t size = end - first;
    data_ = std::make_shared<Storage<value_type>>(size, size);
    size_t current = 0;
    try {
      for (; current < size; ++current, ++first) {
        new (data_->begin_ + current) T(*first);
      }
    } catch (const std::exception &e) {
      for (size_t i = 0; i < current; ++i) {
        data_->begin_[i].~T();
        operator delete(data_->begin_);
      }
      throw;
    }
  }

  void reserve(size_type new_capa) {
    if (new_capa > capacity()) {
      auto tmp = std::make_shared<Storage<value_type>>(size(), new_capa);
      size_t current = 0;
      try {
        for (; current < size(); ++current) {
          new (tmp->begin_ + current) T(*(data_->begin_ + current));
        }
      } catch (const std::exception &e) {
        for (size_t i = 0; i < current; ++i) {
          tmp->begin_[i].~T();
        }
        operator delete(tmp->begin_);
        throw;
      }
      data_.swap(tmp);
    }
  }

  void push_back(const_reference value) {
    if (data_.use_count() > 1 && size() < capacity()) {
      detach();
    }
    if (size() == 0) {
      reserve(1);
    } else if (size() == capacity()) {
      reserve(size() * 2);
    }
    try {
      new (data_->begin_ + size()) T(value);
    } catch (const std::exception &e) {
      data_->begin_[size()].~T();
      throw;
    }
    ++data_->size_;
  }

  void resize(size_type count) {
    if (count > size()) {
      if (count > capacity()) {
        reserve(count);
      }
      for (size_t i = size(); i < count; ++i) {
        push_back(0);
      }
    } else if (count < size()) {
      for (size_t i = size(); i > count; --i) {
        data_->begin_[i].~T();
      }
    } else {
      return;
    }
    data_->size_ = count;
  }

  reference operator[](size_type pos) {
    if (data_.use_count() > 1) {
      detach();
    }
    return data_->begin_[pos];
  }

  const_reference operator[](size_type pos) const { return data_->begin_[pos]; }

  reference at(size_type pos) {
    if (pos >= size()) {
      throw std::out_of_range("Out of range!");
    }
    if (data_.use_count() > 1) {
      detach();
    }
    return data_->begin_[pos];
  }

  const_reference at(size_type pos) const {
    if (pos >= size()) {
      throw std::out_of_range("Out of range!");
    }
    return data_->begin_[pos];
  }

  size_type size() const noexcept { return data_->size_; }
  size_type capacity() const noexcept { return data_->capacity_; }

  bool empty() const noexcept { return (size() == 0); }

  void shrink_to_fit() {
    if (capacity() > size()) {
      auto tmp = std::make_shared<Storage<value_type>>(size(), size());
      size_t current = 0;
      try {
        for (; current < size(); ++current) {
          new (tmp->begin_ + current) T(*(data_->begin_ + current));
        }
      } catch (const std::exception &e) {
        for (size_t i = 0; i < current; ++i) {
          tmp->begin_[i].~T();
        }
        operator delete(tmp->begin_);
        throw;
      }
      data_.swap(tmp);
    }
  }

  iterator insert(const_iterator pos, const T &value) {
    size_t idx = pos - cbegin();

    if (pos == cend()) {
      push_back(value);
    } else {
      push_back(0);
      for (auto i = size(); i != idx; --i) {
        data_->begin_[i] = data_->begin_[i - 1];
      }
      data_->begin_[idx] = value;
    }

    return begin() + idx;
  }

  // iterator insert(const_iterator pos, T&& value ); ??

  iterator insert(const_iterator pos, size_type count, const T &value) {
    if (count == 0) {
      return pos;
    }
    size_t idx = pos - cbegin();

    if (pos == cend()) {
      for (size_t i = 0; i < count; ++i) {
        push_back(value);
      }
    } else {
      for (size_t i = 0; i < count; ++i) {
        push_back(0);
      }
      int i = -1;
      for (size_t j = size(); j != idx; --j, ++i) {
        data_->begin_[j] = data_->begin_[j - count + i];
      }
      for (size_t i = idx; i != idx + count; ++i) {
        data_->begin_[i] = value;
      }
    }

    return begin() + idx;
  }

  iterator insert(const_iterator pos, iterator first, iterator last) {
    if (first == last) {
      return pos;
    }
    size_t idx = pos - cbegin();
    size_t count = last - first;

    if (pos == cend()) {
      auto cur = first;
      for (size_t i = 0; i < count; ++i, ++cur) {
        push_back(*cur);
      }
    } else {
      for (size_t i = 0; i < count; ++i) {
        push_back(0);
      }
      int i = -1;
      for (size_t j = size(); j != idx; --j, ++i) {
        data_->begin_[j] = data_->begin_[j - count + i];
      }
      auto cur = first;
      for (size_t i = idx; i != idx + count; ++i, ++cur) {
        data_->begin_[i] = *cur;
      }
    }
    return begin() + idx;
  }

  iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
    if (ilist.size() == 0) {
      return pos;
    }
    size_t idx = pos - cbegin();
    size_t count = ilist.size();

    if (pos == cend()) {
      auto cur = ilist.begin();
      for (size_t i = 0; i < count; ++i, ++cur) {
        push_back(*cur);
      }
    } else {
      for (size_t i = 0; i < count; ++i) {
        push_back(0);
      }
      int i = -1;
      for (size_t j = size(); j != idx; --j, ++i) {
        data_->begin_[j] = data_->begin_[j - count + i];
      }
      auto cur = ilist.begin();
      for (size_t i = idx; i != idx + count; ++i, ++cur) {
        data_->begin_[i] = *cur;
      }
    }
    return begin() + idx;
  }

  iterator erase(const_iterator pos) {
    if (pos == cend()) {
      return pos;
    }
    size_t idx = pos - cbegin();
    size_t last = cend() - cbegin();
    for (auto i = idx; i != size(); ++i) {
      data_->begin_[i] = data_->begin_[i + 1];
    }
    data_->begin_[last].~T();
    --data_->size_;
    return cbegin() + idx;
  }

  iterator erase(const_iterator first, const_iterator last) {
    if (first == last) {
      return last;
    }
    size_t idx = first - cbegin();
    size_t count = last - first;
    int j = 0;
    for (auto i = idx; i != size(); ++i, ++j) {
      data_->begin_[i] = data_->begin_[i + count + j];
    }

    for (auto i = idx; i < count; ++i) {
      data_->begin_[i].~T();
    }
    data_->size_ -= count;
    return cbegin() + idx;
  }

  size_t count() const { return data_.use_count(); }

private:
  void detach() {
    auto tmp = std::make_shared<Storage<value_type>>(size(), capacity());
    size_t current = 0;
    try {
      for (; current < size(); ++current) {
        new (tmp->begin_ + current) T(*(data_->begin_ + current));
      }
    } catch (const std::exception &e) {
      for (size_t i = 0; i < current; ++i) {
        tmp->begin_[i].~T();
      }
      operator delete(tmp->begin_);
      throw;
    }
    data_.swap(tmp);
  }

  void swap(Vector &obj) noexcept { obj.data_ = this->data_; }

  std::shared_ptr<Storage<value_type>> data_;
};

template <typename T>
inline bool operator==(const Vector<T> &lhs, const Vector<T> &rhs) {
  return (
      lhs.size() == rhs.size() &&
      std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

} // namespace vector_cow