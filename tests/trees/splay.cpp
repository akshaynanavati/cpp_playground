#include "falcon/trees/splay.h"
#include "tests/trees/bst_base.h"

#include "gtest/gtest.h"

using namespace falcon;
using namespace falcon::tests;

TEST(SplayTree, Simple) { simpleTest<SplayTree>(); }

TEST(SplayTree, Test) { testInsert<SplayTree>(); }
