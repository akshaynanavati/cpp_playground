#pragma once

#include <memory>

namespace falcon {
template <template <typename, typename> class Node, class Key, class Val>
class BST {
protected:
  std::unique_ptr<Node<Key, Val>> root_ = nullptr;
  size_t size_ = 0;

  void rotateLeft(Node<Key, Val> *node) {
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

  void rotateRight(Node<Key, Val> *node) {
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

  Node<Key, Val> *insert(Key key, Val val) {
    auto cur = root_.get();
    while (true) {
      if (key == cur->key) {
        cur->val = std::forward<Val>(val);
        return nullptr;
      } else if (key < cur->key) {
        if (cur->left) {
          cur = cur->left.get();
        } else {
          cur->left =
              std::make_unique<Node<Key, Val>>(std::move(key), std::move(val));
          cur->left->parent = cur;
          ++size_;
          return cur->left.get();
        }
      } else {
        if (cur->right) {
          cur = cur->right.get();
        } else {
          cur->right = std::make_unique<Node<Key, Val>>(std::forward<Key>(key),
                                                        std::forward<Val>(val));
          cur->right->parent = cur;
          ++size_;
          return cur->right.get();
        }
      }
    }
  }

public:
  size_t size() const { return size_; }

  Val *find(const Key &key) const {
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
