#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <sstream>

#include "falcon/utils/utils.h"

namespace falcon {
namespace detail {
/**
 * Computes (\sum_{i = 0}^{i = cont.size()}{2^(-cont[i])})^-1 for HyperLogLog.
 */
template <class Container> long double harmonicMean(const Container &cont) {
  long double z = 0;
  for (auto i : cont) {
    z += std::exp2((long double)-i);
  }
  return 1.0l / z;
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

  uint8_t countTrailingZeroes(uint64_t w) const {
    if (UNLIKELY(w == 0)) {
      return 64 - LogM;
    }
    return __builtin_ctzll(w);
  }

public:
  explicit HyperLogLog() { reset(); };
  explicit HyperLogLog(long double alpha) : alpha_(alpha) { reset(); }
  static HyperLogLog<Key, Hash, LogM> readFromDisk(const std::string &fname) {
    std::ifstream infile(fname, std::ios::binary);
    char logm;
    infile.read(&logm, 1);
    if (UNLIKELY(logm != LogM)) {
      std::stringstream ss;
      ss << "LogM from file " << logm << " does not match LogM " << LogM;
      throw std::runtime_error(ss.str());
    }
    long double alpha;
    infile.read(reinterpret_cast<char *>(&alpha), sizeof(long double));
    HyperLogLog<Key, Hash, LogM> hll(alpha);
    infile.read(hll.registers_.data(), hll.registers_.size());
    return hll;
  }

  void insert(const Key &key) {
    uint64_t hash = hasher_(key);
    size_t registerIdx = hash & registerMask;
    uint64_t w = hash >> LogM;
    registers_[registerIdx] =
        std::max(registers_[registerIdx], (uint8_t)countTrailingZeroes(w));
  }

  void setAlpha(long double alpha) { alpha_ = alpha; }

  size_t count() const {
    long double z = detail::harmonicMean(registers_);
    return z * (M << LogM) * alpha_;
  }

  void saveToDisk(const std::string &fname) const {
    std::ofstream of(fname, std::ios::binary);
    of.put(static_cast<char>(LogM));
    of.write(reinterpret_cast<const char *>(&alpha_), sizeof(long double));
    of.write(reinterpret_cast<const char *>(registers_.data()),
             registers_.size());
    of.flush();
  }

  void reset() { std::fill(registers_.begin(), registers_.end(), 0); }
};
} // namespace falcon
