#include <ctime>
#include <random>
#include <unordered_set>

#include "iterators/dense_set.h"

#include "gtest/gtest.h"

using namespace falcon;

constexpr size_t kMinSize = 1 << 5;
constexpr size_t kMaxSize = 1 << 15;
constexpr size_t kLookups = 1 << 10;

std::pair<DenseSet<uint64_t>, std::unordered_set<uint64_t>> testCase(size_t n) {
  static std::mt19937_64 rng(time(NULL));
  DenseSet<uint64_t> ds;
  std::unordered_set<uint64_t> us;
  for (size_t i = 0; i < n; ++i) {
    auto val = rng();
    ds.insert(val);
    us.insert(val);
  }
  return {ds, us};
}

TEST(DenseSet, Lookup) {
  static std::mt19937_64 rng(time(NULL) / 2);
  for (size_t n = kMinSize; n <= kMaxSize; n <<= 1) {
    auto [dset, uset] = testCase(n);
    ASSERT_EQ(dset.size(), uset.size());

    for (auto i : uset) {
      ASSERT_TRUE(dset.find(i));
    }

    for (size_t i = 0; i < kLookups; ++i) {
      auto lookup = rng();
      bool expected = uset.find(lookup) != uset.end();
      bool actual = dset.find(lookup);
      ASSERT_EQ(actual, expected);
    }
  }
}
