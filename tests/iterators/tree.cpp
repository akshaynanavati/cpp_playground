#include <array>

#include "falcon/iterators/tree.h"
#include "falcon/trees/splay.h"

#include "gtest/gtest.h"

using namespace falcon;

TEST(TreeIterator, Basic) {
  std::array<uint8_t, 10> vals = {5, 1, 7, 8, 2, 3, 0, 4, 6, 9};
  SplayTree<uint8_t, uint8_t> t;
  for (auto i : vals) {
    t.emplace(i, 0);
  }

  std::array<uint8_t, 10> inOrder = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  {
    size_t i = 0;
    for (auto elem : t.traverse<InOrder>()) {
      ASSERT_EQ(elem.first, inOrder[i++]);
    }
  }
  /* TODO fix bidirectional
  {
    std::reverse(inOrder.begin(), inOrder.end());
    size_t i = 0;
    auto traversal = t.traverse<InOrder>();
    for (auto it = --traversal.end(); it != traversal.begin(); --it) {
      ASSERT_EQ(it->first, inOrder[i++]);
    }
  }
  */

  std::array<uint8_t, 10> postOrder = {0, 4, 3, 2, 1, 6, 9, 8, 7, 5};
  {
    size_t i = 0;
    for (auto elem : t.traverse<PostOrder>()) {
      ASSERT_EQ(elem.first, postOrder[i++]);
    }
  }
  /* TODO fix bidirectional
  {
    std::reverse(postOrder.begin(), postOrder.end());
    size_t i = 0;
    auto traversal = t.traverse<PostOrder>();
    for (auto it = --traversal.end(); it != traversal.begin(); --it) {
      ASSERT_EQ(it->first, postOrder[i++]);
    }
  }
  */

  std::array<uint8_t, 10> preOrder = {5, 1, 0, 2, 3, 4, 7, 6, 8, 9};
  {
    size_t i = 0;
    for (auto elem : t.traverse<PreOrder>()) {
      ASSERT_EQ(elem.first, preOrder[i++]);
    }
  }
  /* TODO fix bidirectional
  {
    std::reverse(preOrder.begin(), preOrder.end());
    size_t i = 0;
    auto traversal = t.traverse<PreOrder>();
    for (auto it = --traversal.end(); it != traversal.begin(); --it) {
      ASSERT_EQ(it->first, preOrder[i++]);
    }
  }
  */
}
