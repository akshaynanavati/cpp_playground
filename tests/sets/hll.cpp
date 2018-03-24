#include <ctime>
#include <iostream>
#include <limits>
#include <random>
#include <string>

#include "falcon/io/csv.h"
#include "falcon/sets/hll.h"

using namespace falcon;

constexpr int64_t lb = 1 << 20;
constexpr int64_t ub = 1ll << 34;

// Generate random numbers and feed them to the HLL algorithm. Each
// call should generate a unique "hash" and we can see how close the
// count is to actual number of calls.
template <class Key> class Hash {
  std::mt19937_64 rng_;

public:
  Hash() { rng_.seed(std::time(NULL)); }

  uint64_t operator()(const Key & /* unused */) { return rng_(); }
};

int64_t testHll(size_t n) {
  HyperLogLog<std::string, Hash> hll;
  for (size_t i = 0; i < n; ++i) {
    hll.insert("hello");
  }
  return hll.count();
}

int main() {
  std::mt19937 rng;
  Csv writer(std::cout);
  writer.writeRow("HLL Count", "Actual Count", "Delta", "%");
  HyperLogLog<std::string, Hash, 16> hll(0.3l / 0.7l + 1.0l);
  for (size_t i = 0; i < ub; ++i) {
    hll.insert("hello");
    int64_t n = i + 1;
    if (n % 1000000 == 0) {
      int64_t count = hll.count();
      writer.writeRow(count, n, count - n, 100.0l * (count - n) / n);
    }
  }
  return 0;
}
