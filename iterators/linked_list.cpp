#include <memory>

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

    constexpr size_t n = 1024;
    std::array<LLNode *, n> toFree;
    auto cur = next;
    size_t i = 0;
    while (cur) {
      toFree[i % n] = cur;
      ++i;
      auto tmp = cur->next;
      cur->next = nullptr;
      cur = tmp;
      if (i % n == 0) {
        freeBlock(toFree);
      }
    }
    if (i % n > 0) {
      freeBlock(toFree, i);
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
