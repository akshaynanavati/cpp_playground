#include <algorithm>
#include <array>
#include <cmath>

#include "falcon/utils/utils.h"

namespace falcon {
namespace detail {
/**
 * Computes (\sum_{i = 0}^{i = cont.size()}{2^(-cont[i])})^-1 for HyperLogLog.
 */
template <class Container> long double harmonicMean(const Container &cont) {
  long double z = 0;
  for (auto i : cont) {
    z += std::exp2(-i);
  }
  return 1 / z;
}
} // namespace detail

template <class Key, template <typename> class Hash, size_t LogM = 8>
class HyperLogLog {
  static_assert(LogM <= 16);
  static constexpr size_t M = ((size_t)1) << LogM;
  static constexpr size_t registerMask = M - 1;

  long double alpha_ = 1.0;
  Hash<Key> hasher_;
  std::array<uint8_t, M> registers_;

  uint8_t countTrailingZeroes(uint64_t w) {
    if (UNLIKELY(w == 0)) {
      return 64 - LogM;
    }
    return __builtin_ctzll(w);
  }

public:
  explicit HyperLogLog() { reset(); };
  explicit HyperLogLog(long double alpha) : alpha_(alpha) { reset(); }

  void insert(const Key &key) {
    uint64_t hash = hasher_(key);
    size_t registerIdx = hash & registerMask;
    uint64_t w = hash >> LogM;
    registers_[registerIdx] =
        std::max(registers_[registerIdx], (uint8_t)countTrailingZeroes(w));
  }

  size_t count() {
    long double z = detail::harmonicMean(registers_);
    return z * (M << LogM) * alpha_;
  }

  void reset() { std::fill(registers_.begin(), registers_.end(), 0); }
};
} // namespace falcon
