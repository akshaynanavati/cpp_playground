#include <algorithm>
#include <array>
#include <cmath>

#include "falcon/utils/utils.h"

namespace falcon {
namespace detail {
/**
 * Computes (\sum_{i = 0}^{i = cont.size()}{2^(-cont[i])})^-1 for HyperLogLog.
 */
long double harmonicMean(const std::array<uint8_t, 256> &cont) {
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

  size_t countTrailingZeroes(size_t w) {
    if (UNLIKELY(w == 0)) {
      return sizeof(size_t) * 8 - M;
    }
    return __builtin_ctzll(w);
  }

public:
  explicit HyperLogLog() = default;
  explicit HyperLogLog(long double alpha) : alpha_(alpha) {}

  void insert(const Key &key) {
    size_t hash = hasher_(key);
    size_t registerIdx = hash & registerMask;
    size_t w = hash >> LogM;
    registers_[registerIdx] =
        std::max(registers_[registerIdx], (uint8_t)countTrailingZeroes(w));
  }

  size_t count() {
    long double z = detail::harmonicMean(registers_);
    return z * (M << LogM) * alpha_;
  }
};
} // namespace falcon
