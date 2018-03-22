#include <array>

#include "falcon/iterators/bit_vector.h"

namespace falcon {
namespace detail {
constexpr size_t kDefaultNBits = 1 << 20;
constexpr uint64_t kDefaultK = 8;
} // namespace detail

template <class Key, class Hash, size_t nBits = detail::kDefaultNBits,
          uint64_t k = detail::kDefaultK>
class BloomFilter {
  BitVector<nBits> bitVector_;
  std::array<Hash, k> hashFunctions_;

public:
  explicit BloomFilter() {
    for (int i = 0; i < k; ++i) {
      hashFunctions_[i] = Hash(i);
    }
  }

  void insert(const Key &key) {
    for (uint64_t i = 0; i < k; ++i) {
      uint64_t bitIdx = hashFunctions_[i](key) % nBits;
      bitVector_.setBit(bitIdx);
    }
  }

  bool find(const Key &key) {
    for (uint64_t i = 0; i < k; ++i) {
      uint64_t bitIdx = hashFunctions_[i](key) % nBits;
      if (!bitVector_[bitIdx]) {
        return false;
      }
    }
    return true;
  }
};
} // namespace falcon
