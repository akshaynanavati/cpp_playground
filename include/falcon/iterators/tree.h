#pragma once

#include <iterator>

namespace falcon {
template <class TreeNode> class InOrderIt {
  TreeNode *next_ = nullptr;

public:
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef typename TreeNode::data_t *pointer;
  typedef typename TreeNode::data_t &reference;

  explicit InOrderIt() = default;
  explicit InOrderIt(const TreeNode &node) {
    auto next_ = &node;

    while (next_->left) {
      next_ = next_->left.get();
    }
  }

  InOrderIt &operator++() {
    if (next_->parent == nullptr || next_->isLeft()) {
      if (next_->right) {
        next_ = next_->right.get();
        while (next_->left) {
          next_ = next_->left.get();
        }
      } else {
        next_ = next_->parent;
      }
    } else {
      do {
        next_ = next_->parent;
      } while (next_->parent && !next_->isLeft());
      next_ = next_->parent;
    }
    return *this;
  }

  InOrderIt &operator--() {
    if (next_->left) {
      next_ = next_->left;
      while (next_->right) {
        next_ = next_->right.get();
      }
    } else {
      next_ = next_->parent;
    }
    return *this;
  }

  const typename TreeNode::data_t &operator*() const { return next_->data; }

  bool operator==(const InOrderIt &other) const {
    if (next_ == nullptr && other.next_ == nullptr) {
      return true;
    }

    if (next_ == nullptr) {
      return false;
    }

    if (other.next_ == nullptr) {
      return false;
    }

    return next_ == other.next_;
  }

  bool operator!=(const InOrderIt &other) const { return !operator==(other); }
};

template <class TreeNode> class PreOrderIt {
  const TreeNode *next_;

public:
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef typename TreeNode::data_t *pointer;
  typedef typename TreeNode::data_t &reference;

  explicit PreOrderIt() = default;
  explicit PreOrderIt(const TreeNode &node) { next_ = &node; }

  PreOrderIt &operator++() {
    if (next_->left) {
      next_ = next_->left.get();
    } else if (next_->right) {
      next_ = next_->right.get();
    } else {
      next_ = nullptr;
    }

    return *this;
  }

  PreOrderIt &operator--() {
    if (next_->isLeft()) {
      next_ = next_->parent;
    } else {
      next_ = next_->parent;
      while (1) {
        if (next_->right) {
          next_ = next_->right.get();
        } else if (next_->left) {
          next_ = next_->left.get();
        } else {
          break;
        }
      }
    }
    return *this;
  }

  const typename TreeNode::data_t &operator*() const { return next_->data; }

  bool operator==(const PreOrderIt &other) const {
    if (next_ == nullptr && other.next_ == nullptr) {
      return true;
    }

    if (next_ == nullptr) {
      return false;
    }

    if (other.next_ == nullptr) {
      return false;
    }

    return next_ == other.next_;
  }

  bool operator!=(const PreOrderIt &other) const { return !operator==(other); }
};

/**
 * Ideally this would be "lazy" (upto the tree depth a la in order to pre order)
 * and not just pre-compute the entire result into a vector. But I'm not
 * entirely sure how to implement it as such.
 */
template <class TreeNode> class PostOrderIt {
  TreeNode *next_;

public:
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef typename TreeNode::data_t *pointer;
  typedef typename TreeNode::data_t &reference;

  explicit PostOrderIt() = default;
  explicit PostOrderIt(const TreeNode &node) {
    auto next_ = &node;

    while (1) {
      if (next_->left) {
        next_ = next_->left.get();
      } else if (next_->right) {
        next_ = next_->right.get();
      } else {
        break;
      }
    }
  }

  PostOrderIt &operator++() {
    if (next_->isLeft()) {
      next_ = next_->parent;
      if (next_->right) {
        next_ = next_->right.get();
        while (1) {
          if (next_->left) {
            next_ = next_->left.get();
          } else if (next_->right) {
            next_ = next_->right.get();
          } else {
            break;
          }
        }
      }
    } else {
      next_ = next_->parent;
    }

    return *this;
  }

  PostOrderIt &operator--() {
    if (next_->right) {
      next_ = next_->right.get();
    } else {
      next_ = next_->left.get();
    }

    return *this;
  }

  const typename TreeNode::data_t &operator*() const { return next_->data; }

  bool operator==(const PostOrderIt &other) const {
    if (next_ == nullptr && other.next_ == nullptr) {
      return true;
    }

    if (next_ == nullptr) {
      return false;
    }

    if (other.next_ == nullptr) {
      return false;
    }

    return next_ == other.next_;
  }

  bool operator!=(const PostOrderIt &other) const { return !operator==(other); }
};

namespace detail {
template <template <typename> class TraversalIt, class TreeNode>
class Traversal_ {
  const TreeNode *const node_;

public:
  explicit Traversal_(const TreeNode &node) : node_(&node) {}

  TraversalIt<TreeNode> begin() { return TraversalIt<TreeNode>(*node_); }
  TraversalIt<TreeNode> end() { return TraversalIt<TreeNode>(); }
};
} // namespace detail

// These are the traversal types
template <class TreeNode>
using InOrder = detail::Traversal_<InOrderIt, TreeNode>;
template <class TreeNode>
using PostOrder = detail::Traversal_<PostOrderIt, TreeNode>;
template <class TreeNode>
using PreOrder = detail::Traversal_<PreOrderIt, TreeNode>;
} // namespace falcon
