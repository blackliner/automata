#include "gtest/gtest.h"

#include "colony.h"

TEST(ColonyTests, first) {
  colony unit;
  EXPECT_EQ(0, unit.size());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
