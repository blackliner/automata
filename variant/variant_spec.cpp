#include "gtest/gtest.h"

TEST(Variant, first_test) {
  EXPECT_EQ(true, false);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}