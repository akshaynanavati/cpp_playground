#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

template <class T> struct TreeNode {
  std::unique_ptr<TreeNode<T>> left;
  std::unique_ptr<TreeNode<T>> right;
  T data;

  TreeNode(T &&data, std::unique_ptr<TreeNode<T>> left = nullptr,
           std::unique_ptr<TreeNode<T>> right = nullptr)
      : left(std::move(left)), right(std::move(right)),
        data(std::forward<T>(data)) {}

  bool isLeaf() const { return left == nullptr && right == nullptr; }

  /**
   * Traversal is one of InOrder, PostOrder, or PreOrder.
   *
   * Returns a class with a begin and end which return the beginning and end
   * respectively of an iterator through the nodes in Traversal order.
   *
   * For example:
   *
   * To traverse a tree in order you might do:
   * for (const auto& data : t.traverse<InOrder>()) {}
   *
   * To traverse a tree post order you might do:
   * for (const auto& data : t.traverse<PostOrder>()) {}
   *
   * To traverse a tree pre order you might do:
   * for (const auto& data : t.traverse<PreOrder>()) {}
   *
   * Note that the iterator does not keep the tree alive. If the tree is freed,
   * the iterator will be invalidated and accessing the iterator will cause
   * undefined behavior.
   */
  template <template <typename> class Traversal> Traversal<T> traverse() const {
    return Traversal<T>(*this);
  }
};

template <class T> class InOrderIt {
  std::vector<const TreeNode<T> *> stack_;
  const T *cur_ = nullptr;

public:
  typedef std::forward_iterator_tag iterator_category;
  typedef T *pointer;
  typedef T &reference;

  explicit InOrderIt() = default;
  explicit InOrderIt(const TreeNode<T> &node) {
    auto cur = node.left.get();
    stack_.push_back(&node);
    // Find the left most node
    while (cur) {
      stack_.push_back(cur);
      cur_ = &cur->data;
      cur = cur->left.get();
    }
    stack_.pop_back(); // cur_ has been pushed - pop it
  }

  InOrderIt &operator++() {
    if (stack_.empty()) {
      cur_ = nullptr;
      return *this;
    }
    auto node = stack_.back();
    stack_.pop_back();
    cur_ = &node->data;
    auto cur = node->right.get();
    while (cur) {
      stack_.push_back(cur);
      cur = cur->left.get();
    }
    return *this;
  }

  const T &operator*() const { return *cur_; }

  bool operator==(const InOrderIt &other) const {
    if (cur_ == nullptr && other.cur_ == nullptr) {
      return true;
    }

    if (cur_ == nullptr) {
      return false;
    }

    if (other.cur_ == nullptr) {
      return false;
    }

    return *cur_ == *other.cur_;
  }

  bool operator!=(const InOrderIt &other) const { return !operator==(other); }
};

template <class T> class PreOrderIt {
  std::vector<const TreeNode<T> *> stack_;
  const T *cur_;

public:
  typedef std::forward_iterator_tag iterator_category;
  typedef T *pointer;
  typedef T &reference;

  explicit PreOrderIt() = default;
  explicit PreOrderIt(const TreeNode<T> &node) {
    stack_.push_back(node.right.get());
    stack_.push_back(node.left.get());
    cur_ = &node.data;
  }

  PreOrderIt &operator++() {
    if (stack_.empty()) {
      cur_ = nullptr;
      return *this;
    }
    auto node = stack_.back();
    stack_.pop_back();
    if (node->right) {
      stack_.push_back(node->right.get());
    }
    if (node->left) {
      stack_.push_back(node->left.get());
    }
    cur_ = &node->data;
    return *this;
  }

  const T &operator*() const { return *cur_; }

  bool operator==(const PreOrderIt &other) const {
    if (cur_ == nullptr && other.cur_ == nullptr) {
      return true;
    }

    if (cur_ == nullptr) {
      return false;
    }

    if (other.cur_ == nullptr) {
      return false;
    }

    return *cur_ == *other.cur_;
  }

  bool operator!=(const PreOrderIt &other) const { return !operator==(other); }
};

/**
 * Ideally this would be "lazy" (upto the tree depth a la in order to pre order)
 * and not just pre-compute the entire result into a vector. But I'm not
 * entirely sure how to implement it as such.
 */
template <class T> class PostOrderIt {
  std::vector<const T *> iter_;
  size_t i_ = -1;

public:
  typedef std::forward_iterator_tag iterator_category;
  typedef T *pointer;
  typedef T &reference;

  explicit PostOrderIt() = default;
  explicit PostOrderIt(const TreeNode<T> &node) {
    std::vector<const TreeNode<T> *> toVisit;
    toVisit.push_back(&node);
    while (!toVisit.empty()) {
      ++i_;
      const auto cur = toVisit.back();
      toVisit.pop_back();
      iter_.push_back(&cur->data);

      if (cur->left) {
        toVisit.push_back(cur->left.get());
      }

      if (cur->right) {
        toVisit.push_back(cur->right.get());
      }
    }
  }

  PostOrderIt &operator++() { --i_; }

  const T &operator*() const { return *iter_[i_]; }

  bool operator==(const PostOrderIt &other) const { return i_ == other.i_; }

  bool operator!=(const PostOrderIt &other) const { return !operator==(other); }
};

namespace {
template <template <typename> class TraversalIt, class T> class Traversal_ {
  const TreeNode<T> *const node_;

public:
  explicit Traversal_(const TreeNode<T> &node) : node_(&node) {}

  TraversalIt<T> begin() { return TraversalIt<T>(*node_); }
  TraversalIt<T> end() { return TraversalIt<T>(); }
};
} // namespace

// These are the traversal types
template <class T> using InOrder = Traversal_<InOrderIt, T>;
template <class T> using PostOrder = Traversal_<PostOrderIt, T>;
template <class T> using PreOrder = Traversal_<PreOrderIt, T>;

// Below lie some nifty use cases of the above traversals that I can think of.

/**
 * Input: Binary tree
 * Output: true if BST and false otherwise
 */
bool isBst(const TreeNode<uint64_t> &t) {
  auto it = t.traverse<InOrder>();
  return std::is_sorted(it.begin(), it.end());
}

/**
 * Input: Binary search tree
 * Output: Sorted vector of elements in BST.
 */
std::vector<uint64_t> sortBst(const TreeNode<uint64_t> &t) {
  std::vector<uint64_t> vec;
  for (const auto &i : t.traverse<InOrder>()) {
    vec.push_back(i);
  }
  return vec;
}

// Test code
template <template <typename> class Traversal>
bool validate(const TreeNode<uint64_t> &t) {
  uint64_t expected = 1;
  for (const auto &i : t.traverse<Traversal>()) {
    if (i != expected) {
      std::cout << "Expected " << i << " to equal " << expected << std::endl;
      return false;
    }
    ++expected;
  }
  return true;
}

typedef TreeNode<uint64_t> Tree;
bool basicTest() {
  {
    Tree t =
        Tree(6,
             std::make_unique<Tree>(
                 3, std::make_unique<Tree>(2, std::make_unique<Tree>(1)),
                 std::make_unique<Tree>(5, std::make_unique<Tree>(4))),
             std::make_unique<Tree>(
                 11, std::make_unique<Tree>(
                         7, nullptr,
                         std::make_unique<Tree>(9, std::make_unique<Tree>(8),
                                                std::make_unique<Tree>(10)))));

    std::cout << "Validating in order traversal...";
    if (!validate<InOrder>(t)) {
      return false;
    }
    std::cout << "passed!" << std::endl;
  }

  {
    Tree t =
        Tree(1,
             std::make_unique<Tree>(
                 2, std::make_unique<Tree>(3, std::make_unique<Tree>(4)),
                 std::make_unique<Tree>(5, std::make_unique<Tree>(6))),
             std::make_unique<Tree>(
                 7, std::make_unique<Tree>(
                        8, nullptr,
                        std::make_unique<Tree>(9, std::make_unique<Tree>(10),
                                               std::make_unique<Tree>(11)))));

    std::cout << "Validating pre order traversal...";
    if (!validate<PreOrder>(t)) {
      return false;
    }
    std::cout << "passed!" << std::endl;
  }

  {
    Tree t =
        Tree(12,
             std::make_unique<Tree>(
                 6,
                 std::make_unique<Tree>(
                     3, std::make_unique<Tree>(2, std::make_unique<Tree>(1))),
                 std::make_unique<Tree>(5, nullptr, std::make_unique<Tree>(4))),
             std::make_unique<Tree>(
                 11, std::make_unique<Tree>(
                         10, nullptr,
                         std::make_unique<Tree>(9, std::make_unique<Tree>(7),
                                                std::make_unique<Tree>(8)))));

    std::cout << "Validating post order traversal...";
    if (!validate<PostOrder>(t)) {
      return false;
    }
    std::cout << "passed!" << std::endl;
  }
  return true;
}

int main() {
  if (!basicTest()) {
    return 1;
  }

  return 0;
}
