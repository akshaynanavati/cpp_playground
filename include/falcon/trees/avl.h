#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>

#include "falcon/trees/bst.h"

namespace falcon {
namespace detail {
template <class Key, class Val>
struct AVLNode : public BSTNode<AVLNode, Key, Val> {
  size_t height = 1;

  template <class Key_, class Val_>
  AVLNode(Key_ &&key, Val_ &&val)
      : BSTNode<AVLNode, Key, Val>(std::forward<Key_>(key),
                                   std::forward<Val_>(val)) {}
};
} // namespace detail

template <class Key, class Val>
class AVLTree : public BST<detail::AVLNode, Key, Val> {
  size_t height(detail::AVLNode<Key, Val> *node) {
    if (node == nullptr) {
      return 0;
    }
    return node->height;
  }

  ssize_t deltaH(detail::AVLNode<Key, Val> *node) {
    return height(node->left.get()) - height(node->right.get());
  }

  void rebalance(detail::AVLNode<Key, Val> *node) {
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
            BST<detail::AVLNode, Key, Val>::rotateRight(parent);
            grandparent->height -= 2;
            node = parent;
            continue;
          }
        } else {
          if (deltaH(grandparent) == -2) {
            BST<detail::AVLNode, Key, Val>::rotateRight(node);
            ++node->height;
            --parent->height;
            BST<detail::AVLNode, Key, Val>::rotateLeft(node);
            grandparent->height -= 2;
            continue;
          }
        }
      } else {
        if (grandparent->right.get() == parent) {
          if (deltaH(grandparent) == -2) {
            BST<detail::AVLNode, Key, Val>::rotateLeft(parent);
            grandparent->height -= 2;
            node = parent;
            continue;
          }
        } else {
          if (deltaH(grandparent) == 2) {
            BST<detail::AVLNode, Key, Val>::rotateLeft(node);
            ++node->height;
            --parent->height;
            BST<detail::AVLNode, Key, Val>::rotateRight(node);
            grandparent->height -= 2;
            continue;
          }
        }
      }
      node = grandparent;
    }
  }

public:
  void emplace(Key key, Val val) {
    if (BST<detail::AVLNode, Key, Val>::root_ == nullptr) {
      BST<detail::AVLNode, Key, Val>::root_ =
          std::make_unique<detail::AVLNode<Key, Val>>(std::forward<Key>(key),
                                                      std::forward<Val>(val));
      ++BST<detail::AVLNode, Key, Val>::size_;
      return;
    }

    rebalance(BST<detail::AVLNode, Key, Val>::emplace(std::move(key),
                                                      std::move(val)));
  }
};
} // namespace falcon
