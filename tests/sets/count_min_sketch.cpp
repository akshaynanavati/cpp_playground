#include <ctime>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>

#include "falcon/io/csv.h"
#include "falcon/sets/count_min_sketch.h"

#include "city.h"

using namespace falcon;

constexpr int64_t kInserts = 1 << 25;
constexpr int64_t kMaxCount = 1 << 15;
constexpr int64_t kSample = (1 << 15) - 1;

union Buf64 {
  uint64_t x;
  char buf[sizeof(uint64_t)];

  template <class Buf64_> bool operator==(Buf64_ other) const {
    return x == other.x;
  }
};

class CityHash {
  uint64_t seed_ = 0;

public:
  CityHash() = default;
  CityHash(uint64_t seed) : seed_(seed) {}

  CityHash &operator=(CityHash &&) = default;

  uint64_t operator()(const Buf64 &key) const {
    return CityHash64WithSeed(key.buf, sizeof(uint64_t), seed_);
  }
};

/**
 * Generates "random" buffers by generating a random 64 bit number and casting
 * it to a char* of length 8. Then passes this through to city to be further
 * hashed.
 */
int main() {
  Csv writer(std::cout);
  writer.writeRow("Count Min Sketch Count", "Actual Count", "Delta", "%");
  auto cm = std::make_unique<CountMinSketch<Buf64, CityHash, 16, 8>>();

  std::mt19937_64 rng_;
  rng_.seed(std::time(NULL));

  std::unordered_map<Buf64, size_t, CityHash> counts;
  for (size_t i = 0; i < kInserts; ++i) {
    Buf64 buf = {rng_()};
    counts.emplace(buf, rng_() % kMaxCount);
  }

  for (const auto &[buf, count] : counts) {
    cm->insert(buf, count);
  }

  size_t i = 0;
  for (const auto &[buf, expected] : counts) {
    if (((++i) & kSample) == 0) {
      auto actual = cm->count(buf);
      writer.writeRow(actual, expected, actual - expected,
                      100.0l * (actual - expected) / expected);
    }
  }

  return 0;
}
