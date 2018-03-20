#include "tests/trees/bst_base.h"
#include "trees/avl.h"

#include "gtest/gtest.h"

using namespace falcon;
using namespace falcon::tests;

TEST(AVLTree, Simple) { simpleTest<AVLTree>(); }

TEST(AVLTree, Test) { testInsert<AVLTree>(); }
