#include "gtest/gtest.h"

#include "quad_tree.h"

TEST(QuadTree, instantiation) {
  EXPECT_NO_THROW(QuadTree<int>());
}

TEST(QuadTree, add_node) {
  QuadTree<int> quad_tree{};

  EXPECT_NO_THROW(quad_tree.AddNode(1, {0, 0}););
}

TEST(QuadTree, size) {
  QuadTree<int> quad_tree{};

  quad_tree.AddNode(1, {0, 0});
  quad_tree.AddNode(1, {0, 0});

  EXPECT_EQ(quad_tree.Size(), 2);
}

TEST(QuadTree, search) {
  QuadTree<int> quad_tree{};
  quad_tree.SetPoints({-50, 50}, {50, -50});

  // quad_tree.AddNode(0, {0, 0});
  quad_tree.AddNode(1, {-10, -10});
  quad_tree.AddNode(2, {-10, 10});
  quad_tree.AddNode(3, {10, 10});
  quad_tree.AddNode(4, {10, -10});

  Vector2D<int> left_top{5, 15};
  Vector2D<int> right_bot{15, 5};

  auto neighbours = quad_tree.Search(left_top, right_bot);

  ASSERT_EQ(neighbours.size(), 1);
  EXPECT_EQ(neighbours.front(), 3);
}

TEST(QuadTree, huge_search) {
  QuadTree<int> quad_tree{};
  quad_tree.SetPoints({-50, 50}, {50, -50});

  int n_iter{500000};
  for (int i{}; i < n_iter; ++i) {
    int rand_x = static_cast<int>(50.0 * rand() / RAND_MAX);
    int rand_y = static_cast<int>(50.0 * rand() / RAND_MAX);
    quad_tree.AddNode(i, {rand_x, rand_y});
  }

  Vector2D<int> left_top{-5, 5};
  Vector2D<int> right_bot{5, -5};

  auto neighbours = quad_tree.Search(left_top, right_bot);

  // ASSERT_EQ(neighbours.size(), 1);
  // EXPECT_EQ(neighbours.front(), 3);
  EXPECT_EQ(quad_tree.Size(), n_iter);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}