#include <cmath>
#include <vector>

namespace {
enum class Tag { Empty = 0, Tombstone = 1, Value = 2 };

template <class Key> struct SetElem {
  Tag tag = Tag::Empty;
  Key k;
  uint64_t hash;
};
} // namespace

namespace falcon {
template <class Key, size_t probe = 1> class DenseSet {
  std::vector<SetElem<Key>> buf_{1 << 10};
  size_t nElems_ = 0;
  std::hash<Key> hasher_;
  size_t maxChain_ = 0;

  Key *insert(SetElem<Key> &&setElem) {
    // Assumes setElem is a Value
    const size_t n = buf_.size();

    size_t i = setElem.hash % n;
    size_t j = 1;
    while (true) {
      switch (buf_[i % n].tag) {
      case Tag::Empty:
      case Tag::Tombstone:
        buf_[i % n] = std::move(setElem);
        maxChain_ = std::max(j, maxChain_);
        ++nElems_;
        return &buf_[i % n].k;
      case Tag::Value:
        if (buf_[i % n].k == setElem.k) {
          return &buf_[i % n].k;
        }
      }
      i += (size_t)std::pow(j++, probe);
    }
  }

  void resize() {
    const size_t n = buf_.size();

    auto buf = std::move(buf_);
    buf_ = std::vector<SetElem<Key>>(n * 2);
    nElems_ = 0;
    maxChain_ = 0;

    for (size_t i = 0; i < n; ++i) {
      auto setElem = std::move(buf[i]);
      switch (setElem.tag) {
      case Tag::Value:
        insert(std::move(buf[i]));
      }
    }
  }

public:
  Key *insert(const Key &k) {
    if (nElems_ >= ((buf_.size() * 7) >> 3)) {
      resize();
    }
    auto hash = hasher_(k);
    return insert({Tag::Value, k, hash});
  }

  bool find(const Key &k) const {
    const size_t n = buf_.size();

    auto hash = hasher_(k);
    size_t i = hash % n;
    size_t j = 1;
    while (j <= maxChain_ && buf_[i % n].tag != Tag::Empty) {
      const auto &val = buf_[i % n];
      if (val.tag == Tag::Value && val.k == k) {
        return true;
      }
      i += (size_t)std::pow(j++, probe);
    }
    return false;
  }

  size_t size() { return nElems_; }
};
} // namespace falcon
