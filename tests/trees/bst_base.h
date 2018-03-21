#pragma once

#include <array>
#include <cstdint>
#include <random>

#include "gtest/gtest.h"

namespace falcon {
namespace tests {
constexpr size_t kMinSize = 1 << 5;
constexpr size_t kMaxSize = 1 << 15;
constexpr size_t kLookups = 1 << 10;

template <template <typename, typename> class BST> void simpleTest() {
  BST<int, int> t;
  std::array<std::pair<int, int>, 6> testCases = {
      std::make_pair(1, 1), std::make_pair(2, 5), std::make_pair(3, 4),
      std::make_pair(0, 1), std::make_pair(5, 5), std::make_pair(4, 4)};
  for (const auto [k, v] : testCases) {
    t.emplace(k, v);
  }

  for (const auto [k, v] : testCases) {
    ASSERT_EQ(*t.find(k), v);
  }

  t.emplace(1, 1000);
  ASSERT_EQ(*t.find(1), 1000);
}

template <template <typename, typename> class BST>
std::pair<BST<uint32_t, uint32_t>, std::map<uint32_t, uint32_t>>
testCase(size_t n) {
  static std::mt19937 rng(time(NULL));
  BST<uint32_t, uint32_t> tree;
  std::map<uint32_t, uint32_t> map;
  for (size_t i = 0; i < n; ++i) {
    auto key = rng();
    auto val = rng();
    tree.emplace(key, val);
    map.emplace(key, val);
  }
  return {std::move(tree), std::move(map)};
}

template <template <typename, typename> class BST> void testInsert() {
  static std::mt19937 rng(time(NULL) / 2);
  for (size_t n = kMinSize; n <= kMaxSize; n <<= 1) {
    auto [tree, map] = testCase<BST>(n);
    ASSERT_EQ(tree.size(), map.size());

    for (const auto &i : map) {
      ASSERT_EQ(*tree.find(i.first), i.second);
    }

    for (size_t i = 0; i < kLookups; ++i) {
      auto lookup = rng();
      auto expected = map.find(lookup);
      auto actual = tree.find(lookup);
      if (expected == map.end()) {
        ASSERT_EQ(actual, nullptr);
      } else {
        ASSERT_EQ(*actual, expected->second);
      }
    }
  }
}
} // namespace tests
} // namespace falcon
