#include <array>
#include <functional>
#include <limits>

namespace falcon {
template <typename Key, class Hash, size_t LogB = 11, size_t l = 8>
class CountMinSketch {
  static_assert(4 <= LogB && LogB <= 30);

  static constexpr size_t b = 1 << LogB;
  static constexpr size_t numShifts = 64 / LogB;
  static constexpr size_t numHashF =
      l % numShifts ? 1 + l / numShifts : l / numShifts;

  std::array<Hash, numHashF> hashFunctions_;
  std::array<std::array<size_t, l>, b> arr;

  std::array<size_t, l> hashes(const Key &key) {
    std::array<size_t, l> hashes;

    size_t i = 0;
    for (size_t h = 0; h < numHashF; ++h) {
      auto hash = hashFunctions_[h](key);
      size_t j = 0;
      for (; j < numShifts && i < l; ++j) {
        hashes[i++] = hash & (b - 1);
        hash >>= LogB;
      }
    }

    return hashes;
  }

public:
  CountMinSketch() {
    for (size_t i = 0; i < numHashF; ++i) {
      hashFunctions_[i] = Hash(i);
    }
  }

  void insert(const Key &key, size_t count = 1) {
    size_t i = 0;
    for (auto h : hashes(key)) {
      arr[i++][h] += count;
    }
  }

  size_t count(const Key &key) {
    size_t minCount = std::numeric_limits<uint64_t>::max();
    size_t i = 0;
    for (auto h : hashes(key)) {
      minCount = std::min(minCount, arr[i++][h]);
    }
    return minCount;
  }
};
} // namespace falcon
