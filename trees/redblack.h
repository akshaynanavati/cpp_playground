#include <memory>

#include "utils/utils.h"

namespace {
enum class Color { Red = 0, Black = 1 };

template <class Key, class Val> struct RBNode {
  Color color = Color::Red;
  RBNode<Key, Val> *parent = nullptr;
  std::unique_ptr<RBNode<Key, Val>> left = nullptr;
  std::unique_ptr<RBNode<Key, Val>> right = nullptr;
  Key key;
  Val val;

  RBNode(Key &&key, Val &&val)
      : key(std::forward<Key>(key)), val(std::forward<Val>(val)) {}

  bool isLeaf() const { return left == nullptr && right == nullptr; }

  bool isLeft() {
    if (parent->left.get() == this) {
      return true;
    }
    return false;
  }
  RBNode<Key, Val> *grandparent() { return parent->parent; }
  RBNode<Key, Val> *uncle() {
    auto g = grandparent();
    if (parent->isLeft()) {
      return g->right.get();
    } else {
      return g->left.get();
    }
  }
};
} // namespace

namespace falcon {
template <class Key, class Val> class RBTree {
  std::unique_ptr<RBNode<Key, Val>> root_ = nullptr;
  size_t size_ = 0;

  void rotateLeft(RBNode<Key, Val> *node) {
    auto parent = node->parent;
    auto nodeUnique = std::move(parent->right);
    parent->right = std::move(node->left);
    if (parent->right) {
      parent->right->parent = parent;
    }
    auto grandparent = parent->parent;
    if (grandparent == nullptr) {
      node->left = std::move(root_);
      node->left->parent = node;
      root_ = std::move(nodeUnique);
      root_->parent = nullptr;
    } else if (parent->isLeft()) {
      node->left = std::move(grandparent->left);
      node->left->parent = node;
      grandparent->left = std::move(nodeUnique);
      grandparent->left->parent = grandparent;
    } else {
      node->left = std::move(grandparent->right);
      node->left->parent = node;
      grandparent->right = std::move(nodeUnique);
      grandparent->right->parent = grandparent;
    }
  }

  void rotateRight(RBNode<Key, Val> *node) {
    auto parent = node->parent;
    auto nodeUnique = std::move(parent->left);
    parent->left = std::move(node->right);
    if (parent->left) {
      parent->left->parent = parent;
    }
    auto grandparent = parent->parent;
    if (grandparent == nullptr) {
      node->right = std::move(root_);
      node->right->parent = node;
      root_ = std::move(nodeUnique);
      node->parent = nullptr;
    } else if (parent->isLeft()) {
      node->right = std::move(grandparent->left);
      node->right->parent = node;
      grandparent->left = std::move(nodeUnique);
      grandparent->left->parent = grandparent;
    } else {
      node->right = std::move(grandparent->right);
      node->right->parent = node;
      grandparent->right = std::move(nodeUnique);
      grandparent->right->parent = grandparent;
    }
  }

  void rebalance(RBNode<Key, Val> *node) {
    auto parent = node->parent;
    if (parent == nullptr) {
      node->color = Color::Black;
      return;
    }

    if (parent->color == Color::Black) {
      return;
    }

    auto uncle = node->uncle();
    auto grandparent = node->grandparent();
    if (getColor(uncle) == Color::Red) {
      node->color = Color::Black;
      uncle->color = Color::Black;
      grandparent->color = Color::Red;
      return rebalance(grandparent);
    }

    if (grandparent->left && node == grandparent->left->right.get()) {
      rotateLeft(node);
      std::swap(node, parent);
    } else if (grandparent->right && node == grandparent->right->left.get()) {
      rotateRight(node);
      std::swap(node, parent);
    }

    if (node->isLeft()) {
      rotateRight(parent);
    } else {
      rotateLeft(parent);
    }
    parent->color = Color::Black;
    grandparent->color = Color::Red;
  }

  Color getColor(RBNode<Key, Val> *node) {
    if (node == nullptr) {
      return Color::Black;
    }
    return node->color;
  }

public:
  size_t size() const { return size_; }

  void insert(Key key, Val val) {
    auto cur = root_.get();
    if (UNLIKELY(cur == nullptr)) {
      root_ = std::make_unique<RBNode<Key, Val>>(std::forward<Key>(key),
                                                 std::forward<Val>(val));
      root_->color = Color::Black;
      ++size_;
      return;
    }
    while (true) {
      if (key == cur->key) {
        cur->val = std::forward<Val>(val);
        return;
      } else if (key < cur->key) {
        if (cur->left) {
          cur = cur->left.get();
        } else {
          cur->left = std::make_unique<RBNode<Key, Val>>(
              std::forward<Key>(key), std::forward<Val>(val));
          cur->left->parent = cur;
          rebalance(cur->left.get());
          ++size_;
          return;
        }
      } else {
        if (cur->right) {
          cur = cur->right.get();
        } else {
          cur->right = std::make_unique<RBNode<Key, Val>>(
              std::forward<Key>(key), std::forward<Val>(val));
          cur->right->parent = cur;
          rebalance(cur->right.get());
          ++size_;
          return;
        }
      }
    }
  }

  Val *lookup(const Key &key) const {
    auto cur = root_.get();
    while (cur) {
      if (key == cur->key) {
        return &cur->val;
      } else if (key < cur->key) {
        cur = cur->left.get();
      } else {
        cur = cur->right.get();
      }
    }
    return nullptr;
  }
};
} // namespace falcon
