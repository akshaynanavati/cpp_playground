#pragma once

#include <cstdint>
#include <memory>

#include "falcon/trees/bst.h"

namespace detail {
template <class Key, class Val> struct SplayNode {
  SplayNode<Key, Val> *parent = nullptr;
  std::unique_ptr<SplayNode<Key, Val>> left = nullptr;
  std::unique_ptr<SplayNode<Key, Val>> right = nullptr;
  Key key;
  Val val;

  SplayNode(Key &&key, Val &&val)
      : key(std::forward<Key>(key)), val(std::forward<Val>(val)) {}

  bool isLeaf() const { return left == nullptr && right == nullptr; }

  bool isLeft() {
    if (parent->left.get() == this) {
      return true;
    }
    return false;
  }
  SplayNode<Key, Val> *grandparent() { return parent->parent; }
  SplayNode<Key, Val> *uncle() {
    auto g = grandparent();
    if (parent->isLeft()) {
      return g->right.get();
    } else {
      return g->left.get();
    }
  }
};
} // namespace detail

namespace falcon {
template <class Key, class Val>
class SplayTree : public BST<detail::SplayNode, Key, Val> {
  void rebalance(detail::SplayNode<Key, Val> *node) {
    while (1) {
      auto parent = node->parent;
      if (parent == nullptr) {
        return;
      }
      auto grandparent = parent->parent;
      if (grandparent == nullptr) {
        if (parent->left.get() == node) {
          BST<detail::SplayNode, Key, Val>::rotateRight(node);
        } else {
          BST<detail::SplayNode, Key, Val>::rotateLeft(node);
        }
        return;
      }

      if (grandparent->left.get() == parent) {
        if (parent->left.get() == node) {
          BST<detail::SplayNode, Key, Val>::rotateRight(parent);
          BST<detail::SplayNode, Key, Val>::rotateRight(node);
        } else {
          BST<detail::SplayNode, Key, Val>::rotateLeft(node);
          BST<detail::SplayNode, Key, Val>::rotateRight(node);
        }
      } else {
        if (parent->right.get() == node) {
          BST<detail::SplayNode, Key, Val>::rotateLeft(parent);
          BST<detail::SplayNode, Key, Val>::rotateLeft(node);
        } else {
          BST<detail::SplayNode, Key, Val>::rotateRight(node);
          BST<detail::SplayNode, Key, Val>::rotateLeft(node);
        }
      }
    }
  }

public:
  void emplace(Key key, Val val) {
    if (BST<detail::SplayNode, Key, Val>::root_ == nullptr) {
      BST<detail::SplayNode, Key, Val>::root_ =
          std::make_unique<detail::SplayNode<Key, Val>>(std::forward<Key>(key),
                                                        std::forward<Val>(val));
      ++BST<detail::SplayNode, Key, Val>::size_;
      return;
    }
    BST<detail::SplayNode, Key, Val>::emplace(std::move(key), std::move(val));
  }

  Val *find(const Key &key) {
    auto node = BST<detail::SplayNode, Key, Val>::findNode(key);
    if (node == nullptr) {
      return nullptr;
    }
    rebalance(node);
    return &node->val;
  }
};
} // namespace falcon
