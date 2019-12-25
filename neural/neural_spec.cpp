#include "gtest/gtest.h"

#include "neural.h"

TEST(Weights, 1x1) {
  Weights w;
  w.Resize(1, 1);
  w(0, 0) = 1.0;
  EXPECT_FLOAT_EQ(1.0, w(0, 0));
}

TEST(Weights, 2x2) {
  Weights w;
  w.Resize(2, 2);
  w(0, 0) = 0.0;
  w(0, 1) = 1.0;
  w(1, 0) = 2.0;
  w(1, 1) = 3.0;
  EXPECT_FLOAT_EQ(0.0, w(0, 0));
  EXPECT_FLOAT_EQ(1.0, w(0, 1));
  EXPECT_FLOAT_EQ(2.0, w(1, 0));
  EXPECT_FLOAT_EQ(3.0, w(1, 1));
}

TEST(Weights, 2x2_with_reset) {
  Weights w;
  w.Resize(2, 2);
  w.Reset(3.5);
  EXPECT_FLOAT_EQ(3.5, w(0, 0));
  EXPECT_FLOAT_EQ(3.5, w(0, 1));
  EXPECT_FLOAT_EQ(3.5, w(1, 0));
  EXPECT_FLOAT_EQ(3.5, w(1, 1));
}

TEST(Neural, set_size) {
  Layer layer;
  layer.SetSize(1);

  EXPECT_EQ(1, layer.Size());
}

TEST(Neural, set_data) {
  Layer layer;
  layer.SetSize(1);
  Data input{1.0};
  layer.SetData(input);

  EXPECT_EQ(1, layer.GetData().front());
}

TEST(Neural, feed_forward_0) {
  Layer layer;
  layer.SetSize(1);

  Data input{0.0};
  layer.SetData(input);

  Weights weights;
  weights.Resize(1, 1);
  weights(0, 0) = 1.0;

  auto result = FeedForwardLayer(layer, weights);

  EXPECT_FLOAT_EQ(0.0, result.back());
}

TEST(Neural, feed_forward_1) {
  Layer layer;
  layer.SetSize(1);

  layer.SetData({1.0});

  Weights weights;
  weights.Resize(1, 1);
  weights(0, 0) = 1.0;

  auto result = FeedForwardLayer(layer, weights);

  EXPECT_FLOAT_EQ(1.0, result.back());
}

TEST(Neural, feed_forward_size2_0) {
  Layer layer;
  layer.SetSize(2);
  layer.SetData({0.0, 0.0});

  Weights weights;
  weights.Resize(2, 1);
  weights.Reset(1.0);

  auto result = FeedForwardLayer(layer, weights);

  EXPECT_FLOAT_EQ(0.0, result.back());
}

TEST(Neural, feed_forward_size2_1) {
  Layer layer;
  layer.SetSize(2);
  layer.SetData({1.0, 1.0});

  Weights weights;
  weights.Resize(2, 1);
  weights.Reset(1.0);

  auto result = FeedForwardLayer(layer, weights);

  EXPECT_FLOAT_EQ(2.0, result.back());
}

TEST(Network, set_layout) {
  Network network;
  const Layout layout = {1, 1};

  network.SetLayout(layout);

  const auto read_layout = network.GetLayout();

  EXPECT_EQ(layout, read_layout);
}

TEST(Network, feed_forward_0) {
  Network network;
  network.SetLayout({1, 1});
  network.ResetWeights(1.0);
  network.SetInput({0.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(0.0, output.front());
}

TEST(Network, feed_forward_1) {
  Network network;
  network.SetLayout({1, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(1.0, output.front());
}

TEST(Network, feed_forward_2x1_0) {
  Network network;
  network.SetLayout({2, 1});
  network.ResetWeights(1.0);
  network.SetInput({0.0, 0.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(0.0, output.front());
}

TEST(Network, feed_forward_2x1_1) {
  Network network;
  network.SetLayout({2, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0, 1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(2.0, output.front());
}

TEST(Network, feed_forward_2x1x1_1) {
  Network network;
  network.SetLayout({2, 1, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0, 1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(2.0, output.front());
}

TEST(Network, feed_forward_2x4x1_1) {
  Network network;
  network.SetLayout({2, 4, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0, 1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(8.0, output.front());
}

TEST(Network, feed_forward_2x4x8x16x32x1_1) {
  Network network;
  network.SetLayout({2, 4, 8, 16, 32, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0, 1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(32768.0, output.front());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}