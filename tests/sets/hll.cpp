#include <ctime>
#include <iostream>
#include <limits>
#include <random>
#include <string>

#include "falcon/io/csv.h"
#include "falcon/sets/hll.h"

#include "city.h"

using namespace falcon;

constexpr int64_t lb = 1 << 20;
constexpr int64_t ub = 1ll << 34;

// Generate random numbers and feed them to the HLL algorithm. Each
// call should generate a unique "hash" and we can see how close the
// count is to actual number of calls.
template <class Key> class RngHash {
  std::mt19937_64 rng_;

public:
  RngHash() { rng_.seed(std::time(NULL)); }

  uint64_t operator()(const Key & /* unused */) { return rng_(); }
};

int main_rng() {
  std::mt19937 rng;
  Csv writer(std::cout);
  writer.writeRow("HLL Count", "Actual Count", "Delta", "%");
  HyperLogLog<std::string, RngHash, 16> hll(0.3l / 0.68l + 1.0l);
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

union Buf64 {
  uint64_t x;
  char buf[sizeof(uint64_t)];
};

template <class Key> class CityHash {
public:
  uint64_t operator()(const Buf64 &key) {
    return CityHash64(key.buf, sizeof(uint64_t));
  }
};

/**
 * Generates "random" buffers by generating a random 64 bit number and casting
 * it to a char* of length 8. Then passes this through city hash to be further
 * hashed.
 */
int main_city() {
  Csv writer(std::cout);
  writer.writeRow("HLL Count", "Actual Count", "Delta", "%");
  HyperLogLog<Buf64, CityHash, 16> hll(0.3l / 0.68l + 1.0l);

  std::mt19937_64 rng_;
  rng_.seed(std::time(NULL));

  for (size_t i = 0; i < ub; ++i) {
    Buf64 buf = {rng_()};
    hll.insert(buf);
    int64_t n = i + 1;
    if (n % 1000000 == 0) {
      int64_t count = hll.count();
      writer.writeRow(count, n, count - n, 100.0l * (count - n) / n);
    }
  }
  return 0;
}

int main() { return main_city(); }
