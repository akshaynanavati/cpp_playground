#pragma once

#include <memory>
#include <utility>

namespace falcon {
template <template <typename, typename> class Node, class Key, class Val>
struct BSTNode {
  typedef std::pair<Key, Val> data_t;

  Node<Key, Val> *parent = nullptr;
  std::unique_ptr<Node<Key, Val>> left = nullptr;
  std::unique_ptr<Node<Key, Val>> right = nullptr;
  std::pair<Key, Val> data;

  template <class Key_, class Val_>
  BSTNode(Key_ &&key, Val_ &&val)
      : data(std::make_pair<Key, Val>(std::forward<Key_>(key),
                                      std::forward<Val_>(val))) {}

  bool isLeft() const {
    if (parent->left.get() == this) {
      return true;
    }
    return false;
  }
  Node<Key, Val> *grandparent() { return parent->parent; }
  Node<Key, Val> *uncle() {
    auto g = grandparent();
    if (parent->isLeft()) {
      return g->right.get();
    } else {
      return g->left.get();
    }
  }
};

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

  template <class Key_, class Val_>
  Node<Key, Val> *emplace(Key_ &&key, Val_ &&val) {
    auto cur = root_.get();
    while (true) {
      if (key == cur->data.first) {
        cur->data.second = std::forward<Val_>(val);
        return nullptr;
      } else if (key < cur->data.first) {
        if (cur->left) {
          cur = cur->left.get();
        } else {
          cur->left = std::make_unique<Node<Key, Val>>(std::forward<Key_>(key),
                                                       std::forward<Val_>(val));
          cur->left->parent = cur;
          ++size_;
          return cur->left.get();
        }
      } else {
        if (cur->right) {
          cur = cur->right.get();
        } else {
          cur->right = std::make_unique<Node<Key, Val>>(
              std::forward<Key_>(key), std::forward<Val_>(val));
          cur->right->parent = cur;
          ++size_;
          return cur->right.get();
        }
      }
    }
  }

  Node<Key, Val> *findNode(const Key &key) const {
    auto cur = root_.get();
    while (cur) {
      if (key == cur->data.first) {
        return cur;
      } else if (key < cur->data.first) {
        cur = cur->left.get();
      } else {
        cur = cur->right.get();
      }
    }
    return nullptr;
  }

public:
  typedef Node<Key, Val> node_t;
  typedef Key key_t;

  size_t size() const { return size_; }

  Val *find(const Key &key) const {
    auto node = findNode(key);
    if (node == nullptr) {
      return nullptr;
    }
    return &node->data.second;
  }

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
  template <template <typename> class Traversal>
  Traversal<Node<Key, Val>> traverse() const {
    return Traversal<Node<Key, Val>>(*root_);
  }
};
} // namespace falcon
