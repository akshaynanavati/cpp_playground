#include <ctime>
#include <random>
#include <unordered_set>

#include "falcon/iterators/dense_set.h"

#include "gtest/gtest.h"

using namespace falcon;

constexpr size_t kMinSize = 1 << 5;
constexpr size_t kMaxSize = 1 << 15;
constexpr size_t kLookups = 1 << 10;

std::pair<DenseSet<uint32_t>, std::unordered_set<uint32_t>> testCase(size_t n) {
  static std::mt19937 rng(time(NULL));
  DenseSet<uint32_t> ds;
  std::unordered_set<uint32_t> us;
  for (size_t i = 0; i < n; ++i) {
    auto val = rng();
    ds.insert(val);
    us.insert(val);
  }
  return {std::move(ds), std::move(us)};
}

TEST(DenseSet, Test) {
  static std::mt19937 rng(time(NULL) / 2);
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

    // Do some random deletions
    for (size_t i = 0; i < dset.size(); ++i) {
      if (i & 1) {
        auto val = uset.begin();
        if (val != uset.end()) {
          auto toDelete = *val;
          ASSERT_EQ(uset.erase(toDelete), 1);
          ASSERT_EQ(dset.erase(toDelete), 1);
        }
      } else {
        auto val = rng();
        ASSERT_EQ(dset.erase(val), uset.erase(val));
      }
    }
    ASSERT_EQ(dset.size(), uset.size());

    for (size_t i = 0; i < kLookups; ++i) {
      auto lookup = rng();
      bool expected = uset.find(lookup) != uset.end();
      bool actual = dset.find(lookup);
      ASSERT_EQ(actual, expected);
    }
  }
}
