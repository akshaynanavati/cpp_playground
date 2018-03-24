#include <cstdint>
#include <cstdlib>

namespace falcon {
template <size_t nBits> class BitVector {
  static constexpr size_t nBytes = nBits << 3;
  uint8_t *buf_;

public:
  BitVector() { buf_ = (uint8_t *)std::malloc(nBytes); }
  ~BitVector() { std::free(buf_); }

  bool operator[](size_t i) const {
    size_t bitIdx = i & 7;
    size_t bufIdx = i >> 3;
    return (buf_[bufIdx] >> (7 - bitIdx)) & 1;
  }

  void setBit(size_t i) {
    size_t bitIdx = i & 7;
    size_t bufIdx = i >> 3;
    auto block = buf_[bufIdx];
    buf_[bufIdx] = block | (1 << (7 - bitIdx));
  }
};
} // namespace falcon
