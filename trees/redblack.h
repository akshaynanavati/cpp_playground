#pragma once

#include <memory>

#include "trees/bst.h"
#include "utils/utils.h"

namespace detail {
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
} // namespace detail

namespace falcon {
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
  void insert(Key key, Val val) {
    if (BST<detail::RBNode, Key, Val>::root_ == nullptr) {
      BST<detail::RBNode, Key, Val>::root_ =
          std::make_unique<detail::RBNode<Key, Val>>(std::forward<Key>(key),
                                                     std::forward<Val>(val));
      BST<detail::RBNode, Key, Val>::root_->color = detail::Color::Black;
      ++BST<detail::RBNode, Key, Val>::size_;
      return;
    }

    rebalance(
        BST<detail::RBNode, Key, Val>::insert(std::move(key), std::move(val)));
  }
};
} // namespace falcon
