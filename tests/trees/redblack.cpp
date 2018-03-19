#include <array>
#include <cstdint>
#include <map>
#include <random>

#include "trees/redblack.h"

#include "gtest/gtest.h"

using namespace falcon;

constexpr size_t kMinSize = 1 << 5;
constexpr size_t kMaxSize = 1 << 15;
constexpr size_t kLookups = 1 << 10;

TEST(RedBlackTree, Simple) {
  RBTree<int, int> t;
  std::array<std::pair<int, int>, 6> testCases = {
      std::make_pair(1, 1), std::make_pair(2, 5), std::make_pair(3, 4),
      std::make_pair(0, 1), std::make_pair(5, 5), std::make_pair(4, 4)};
  for (const auto [k, v] : testCases) {
    t.insert(k, v);
  }

  for (const auto [k, v] : testCases) {
    ASSERT_EQ(*t.lookup(k), v);
  }

  t.insert(1, 1000);
  ASSERT_EQ(*t.lookup(1), 1000);
}

std::pair<RBTree<uint32_t, uint32_t>, std::map<uint32_t, uint32_t>>
testCase(size_t n) {
  static std::mt19937 rng(time(NULL));
  RBTree<uint32_t, uint32_t> tree;
  std::map<uint32_t, uint32_t> map;
  for (size_t i = 0; i < n; ++i) {
    auto key = rng();
    auto val = rng();
    tree.insert(key, val);
    map.emplace(key, val);
  }
  return {std::move(tree), std::move(map)};
}

TEST(RedBlackTree, Test) {
  static std::mt19937 rng(time(NULL) / 2);
  for (size_t n = kMinSize; n <= kMaxSize; n <<= 1) {
    auto [tree, map] = testCase(n);
    ASSERT_EQ(tree.size(), map.size());

    for (const auto &i : map) {
      ASSERT_EQ(*tree.lookup(i.first), i.second);
    }

    for (size_t i = 0; i < kLookups; ++i) {
      auto lookup = rng();
      auto expected = map.find(lookup);
      auto actual = tree.lookup(lookup);
      if (expected == map.end()) {
        ASSERT_EQ(actual, nullptr);
      } else {
        ASSERT_EQ(*actual, expected->second);
      }
    }
  }
}
