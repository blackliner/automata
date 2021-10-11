#include <string>

#include "gtest/gtest.h"

#include "variant.h"

TEST(Variant, copy_assign_and_both_equals_operators) {
  variant<int> unit;
  int test_value{5};

  unit = test_value;

  EXPECT_EQ(unit, test_value);
  EXPECT_EQ(test_value, unit);
}

TEST(Variant, same_with_string) {
  variant<std::string> unit;
  std::string test_value = "Hello World!";

  unit = test_value;

  EXPECT_EQ(unit, test_value);
  EXPECT_EQ(test_value, unit);
}

TEST(VariadicUnion, simple_union) {
  my_variadic_union<int> unit;
  int test_value{5};

  unit.get<0>() = test_value;

  EXPECT_EQ(unit.get<0>(), test_value);
  EXPECT_EQ(test_value, unit.get<0>());
}

TEST(VariadicUnion, dual_union) {
  my_variadic_union<int, double> unit;
  int test_value{5};

  unit.get<0>() = test_value;

  EXPECT_EQ(unit.get<0>(), test_value);
  EXPECT_EQ(test_value, unit.get<0>());
}

// TEST(Variant, multiple_types) {
//   variant<int, std::string> unit;
//   int test_value{5};

//   unit = test_value;

//   EXPECT_EQ(unit, test_value);
//   EXPECT_EQ(test_value, unit);
// }

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
