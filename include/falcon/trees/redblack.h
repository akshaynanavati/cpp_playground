#pragma once

#include <memory>

#include "falcon/trees/bst.h"

namespace falcon {
namespace detail {
enum class Color { Red = 0, Black = 1 };

template <class Key, class Val>
struct RBNode : public BSTNode<RBNode, Key, Val> {
  Color color = Color::Red;

  template <class Key_, class Val_>
  RBNode(Key_ &&key, Val_ &&val)
      : BSTNode<RBNode, Key, Val>(std::forward<Key_>(key),
                                  std::forward<Val_>(val)) {}
};
} // namespace detail

template <class Key, class Val>
class RBTree : public BST<detail::RBNode, Key, Val> {
  void rebalance(detail::RBNode<Key, Val> *node) {
    if (node == nullptr) {
      return;
    }

    auto parent = node->parent;
    if (parent == nullptr) {
      node->color = detail::Color::Black;
      return;
    }

    if (parent->color == detail::Color::Black) {
      return;
    }

    auto uncle = node->uncle();
    auto grandparent = node->grandparent();
    if (getColor(uncle) == detail::Color::Red) {
      node->color = detail::Color::Black;
      uncle->color = detail::Color::Black;
      grandparent->color = detail::Color::Red;
      return rebalance(grandparent);
    }

    if (grandparent->left && node == grandparent->left->right.get()) {
      BST<detail::RBNode, Key, Val>::rotateLeft(node);
      std::swap(node, parent);
    } else if (grandparent->right && node == grandparent->right->left.get()) {
      BST<detail::RBNode, Key, Val>::rotateRight(node);
      std::swap(node, parent);
    }

    if (node->isLeft()) {
      BST<detail::RBNode, Key, Val>::rotateRight(parent);
    } else {
      BST<detail::RBNode, Key, Val>::rotateLeft(parent);
    }
    parent->color = detail::Color::Black;
    grandparent->color = detail::Color::Red;
  }

  detail::Color getColor(detail::RBNode<Key, Val> *node) {
    if (node == nullptr) {
      return detail::Color::Black;
    }
    return node->color;
  }

public:
  void emplace(Key key, Val val) {
    if (BST<detail::RBNode, Key, Val>::root_ == nullptr) {
      BST<detail::RBNode, Key, Val>::root_ =
          std::make_unique<detail::RBNode<Key, Val>>(std::forward<Key>(key),
                                                     std::forward<Val>(val));
      BST<detail::RBNode, Key, Val>::root_->color = detail::Color::Black;
      ++BST<detail::RBNode, Key, Val>::size_;
      return;
    }

    rebalance(
        BST<detail::RBNode, Key, Val>::emplace(std::move(key), std::move(val)));
  }
};
} // namespace falcon
