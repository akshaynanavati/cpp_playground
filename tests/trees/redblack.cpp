#include "tests/trees/bst_base.h"
#include "falcon/trees/redblack.h"

#include "gtest/gtest.h"

using namespace falcon;
using namespace falcon::tests;

TEST(RedBlackTree, Simple) { simpleTest<RBTree>(); }

TEST(RedBlackTree, Test) { testInsert<RBTree>(); }
