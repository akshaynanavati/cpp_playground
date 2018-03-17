#include <memory>

namespace falcon {
template <class T> struct LLNode {
  LLNode *next;
  T data;

  LLNode(T &&data, std::unique_ptr<LLNode<T>> next = nullptr)
      : next(next.release()), data(std::forward<T>(data)) {}

  LLNode(LLNode &&other) {
    next = other.next;
    other.next = nullptr;
    data = std::move(other.data);
  }
  LLNode(LLNode &other) = delete;

  LLNode &operator=(LLNode &&other) {
    delete next;
    next = other.next;
    other.next = nullptr;
    data = std::move(other.data);
    return *this;
  }
  LLNode &operator=(LLNode &other) = delete;

  ~LLNode() {
    if (next == nullptr) {
      return;
    }

    auto cur = next;
    size_t i = 0;
    while (cur) {
      ++i;
      auto tmp = cur->next;
      cur->next = nullptr;
      delete cur;
      cur = tmp;
    }
  }

private:
  template <class Container> void freeBlock(Container block, size_t n) {
    for (size_t i = 0; i < n; ++i) {
      delete block[i];
    }
  }
  template <class Container> void freeBlock(Container block) {
    freeBlock(block, block.size());
  }
};
} // namespace falcon
