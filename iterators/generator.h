#pragma once

#include <cstdlib>
#include <iterator>

template <class T, class F> class RandomAccessGenerator {
public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef size_t value_type;
  typedef size_t difference_type;
  typedef T *pointer;
  typedef T reference;

  explicit RandomAccessGenerator() = default;
  explicit RandomAccessGenerator(F f) : RandomAccessGenerator(f, 0) {}
  explicit RandomAccessGenerator(F f, size_t i) : i_(i), f_(f) {}
  explicit RandomAccessGenerator(size_t n) : i_(n) {}

  bool operator==(const RandomAccessGenerator &other) const {
    return i_ == other.i_;
  }

  bool operator!=(const RandomAccessGenerator &other) const {
    return !operator==(other);
  }

  RandomAccessGenerator &operator++() {
    ++i_;
    return *this;
  }

  RandomAccessGenerator &operator--() {
    --i_;
    return *this;
  }

  RandomAccessGenerator &operator+=(size_t n) {
    i_ += n;
    return *this;
  }

  RandomAccessGenerator &operator-=(size_t n) {
    i_ -= n;
    return *this;
  }

  RandomAccessGenerator operator+(size_t n) const {
    return RandomAccessGenerator(f_, i_ + n);
  }

  RandomAccessGenerator operator-(size_t n) const {
    return RandomAccessGenerator(f_, i_ - n);
  }

  size_t operator-(const RandomAccessGenerator &other) const {
    return i_ - other.i_;
  }

  T operator*() const { return f_(i_); }

  T operator[](size_t n) const { return f_(i_ + n); }

private:
  size_t i_ = 0;
  F f_;
};
