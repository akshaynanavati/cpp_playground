#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>

#include "trees/bst.h"

namespace {
template <class Key, class Val> struct AVLNode {
  AVLNode<Key, Val> *parent = nullptr;
  std::unique_ptr<AVLNode<Key, Val>> left = nullptr;
  std::unique_ptr<AVLNode<Key, Val>> right = nullptr;
  Key key;
  Val val;
  size_t height = 1;

  AVLNode(Key &&key, Val &&val)
      : key(std::forward<Key>(key)), val(std::forward<Val>(val)) {}

  bool isLeaf() const { return left == nullptr && right == nullptr; }

  bool isLeft() {
    if (parent->left.get() == this) {
      return true;
    }
    return false;
  }
  AVLNode<Key, Val> *grandparent() { return parent->parent; }
  AVLNode<Key, Val> *uncle() {
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
template <class Key, class Val> class AVLTree : public BST<AVLNode, Key, Val> {
  size_t height(AVLNode<Key, Val> *node) {
    if (node == nullptr) {
      return 0;
    }
    return node->height;
  }

  ssize_t deltaH(AVLNode<Key, Val> *node) {
    return height(node->left.get()) - height(node->right.get());
  }

  void rebalance(AVLNode<Key, Val> *node) {
    while (1) {
      if (node == nullptr) {
        return;
      }
      auto parent = node->parent;
      if (parent == nullptr) {
        return;
      }
      parent->height =
          std::max(height(parent->left.get()), height(parent->right.get())) + 1;
      auto grandparent = parent->parent;
      if (grandparent == nullptr) {
        return;
      }
      grandparent->height = std::max(height(grandparent->left.get()),
                                     height(grandparent->right.get())) +
                            1;
      if (parent->left.get() == node) {
        if (grandparent->left.get() == parent) {
          if (deltaH(grandparent) == 2) {
            BST<AVLNode, Key, Val>::rotateRight(parent);
            grandparent->height -= 2;
            node = parent;
            continue;
          }
        } else {
          if (deltaH(grandparent) == -2) {
            BST<AVLNode, Key, Val>::rotateRight(node);
            ++node->height;
            --parent->height;
            BST<AVLNode, Key, Val>::rotateLeft(node);
            grandparent->height -= 2;
            continue;
          }
        }
      } else {
        if (grandparent->right.get() == parent) {
          if (deltaH(grandparent) == -2) {
            BST<AVLNode, Key, Val>::rotateLeft(parent);
            grandparent->height -= 2;
            node = parent;
            continue;
          }
        } else {
          if (deltaH(grandparent) == 2) {
            BST<AVLNode, Key, Val>::rotateLeft(node);
            ++node->height;
            --parent->height;
            BST<AVLNode, Key, Val>::rotateRight(node);
            grandparent->height -= 2;
            continue;
          }
        }
      }
      node = grandparent;
    }
  }

public:
  void insert(Key key, Val val) {
    if (BST<AVLNode, Key, Val>::root_ == nullptr) {
      BST<AVLNode, Key, Val>::root_ = std::make_unique<AVLNode<Key, Val>>(
          std::forward<Key>(key), std::forward<Val>(val));
      ++BST<AVLNode, Key, Val>::size_;
      return;
    }

    rebalance(BST<AVLNode, Key, Val>::insert(std::move(key), std::move(val)));
  }
};
} // namespace falcon
