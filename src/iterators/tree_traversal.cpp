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
};

// Below lie some nifty use cases of the above traversals that I can think of.

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
