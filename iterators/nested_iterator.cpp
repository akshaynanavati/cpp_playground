#include <iostream>
#include <vector>

constexpr auto msg = "Done iterating!";

// TODO Make this work with any iterator not just vector. Arrays are tricky
// because they need 2 template params (T, N), not just T.
template <class T> struct is_container { typedef void type; };
template <class T> struct is_container<std::vector<T>> { typedef T type; };

template <class C, class T> class NestedIterator_ {
  typedef NestedIterator_<T, typename is_container<T>::type> ni_t;

  C container_;
  ni_t ni_;
  size_t i_ = 0;

  void assign_ni() {
    const size_t n = container_.size();
    while (i_ < n) {
      try {
        ni_ = ni_t(std::move(container_[i_++]));
        return;
      } catch (...) {
      }
    }
    throw std::runtime_error(msg);
  }

public:
  NestedIterator_() = default;
  explicit NestedIterator_(C container) : container_(std::move(container)) {
    assign_ni();
  }

  decltype(ni_.next()) next() {
    try {
      return ni_.next();
    } catch (...) {
      assign_ni();
    }
    return ni_.next();
  }
};

template <class T> class NestedIterator_<T, void> {
  T data_;
  bool called = false;

public:
  NestedIterator_() = default;
  NestedIterator_(T data) : data_(std::move(data)) {}
  T next() {
    if (called) {
      throw std::runtime_error(msg);
    }
    called = true;
    return data_;
  }
};

template <class C>
using NestedIterator = NestedIterator_<C, typename is_container<C>::type>;

int main() {
  std::vector<std::vector<std::vector<std::vector<int>>>> v = {
      {{{1, 2}, {}, {3, 4, 5}, {}, {}, {6, 7, 8}, {9}, {}, {}},
       {},
       {{10, 11}},
       {{}},
       {{12}}},
      {}};
  auto ni = NestedIterator<decltype(v)>(std::move(v));
  size_t i = 1;
  try {
    while (1) {
      auto n = ni.next();
      if (i != n) {
        std::cout << "Expected n(" << n << ") to equal i(" << i << ")"
                  << std::endl;
        return 1;
      }
      ++i;
    }
  } catch (const std::exception &e) {
  }
  if (i != 13) {
    std::cout << "Expected total iterations = " << i << "to equal 13"
              << std::endl;
    return 1;
  }
  return 0;
}
