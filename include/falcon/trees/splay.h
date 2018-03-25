#pragma once

#include <cstdint>
#include <memory>

#include "falcon/trees/bst.h"

namespace falcon {
namespace detail {
template <class Key, class Val>
struct SplayNode : public BSTNode<SplayNode, Key, Val> {
  template <class Key_, class Val_>
  SplayNode(Key_ &&key, Val_ &&val)
      : BSTNode<SplayNode, Key, Val>(std::forward<Key_>(key),
                                     std::forward<Val_>(val)) {}
};
} // namespace detail

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
    return &node->data.second;
  }
};
} // namespace falcon
