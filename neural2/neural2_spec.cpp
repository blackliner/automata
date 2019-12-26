#include "gtest/gtest.h"

#include "neural2.h"

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

// TEST(Weights, randomize)

TEST(Layer, set_size) {
  Layer layer;
  layer.SetSize(1);

  EXPECT_EQ(1, layer.InputSize());
}

TEST(Layer, set_data) {
  Layer layer;
  layer.SetSize(1);
  Data input{1.0};
  layer.SetOutputData(input);

  EXPECT_EQ(1, layer.GetOutputData().front());
}

TEST(Layer, feed_forward_0) {
  Layer layer;
  layer.SetSize(1);

  Data input{0.0};
  layer.SetOutputData(input);

  Weights weights;
  weights.Resize(1, 1);
  weights(0, 0) = 1.0;

  auto result = FeedForwardLayer(layer, weights);

  EXPECT_FLOAT_EQ(0.0, result.back());
}

TEST(Layer, feed_forward_1) {
  Layer layer;
  layer.SetSize(1);

  layer.SetOutputData({1.0});

  Weights weights;
  weights.Resize(1, 1);
  weights(0, 0) = 1.0;

  auto result = FeedForwardLayer(layer, weights);

  EXPECT_FLOAT_EQ(1.0, result.back());
}

TEST(Layer, feed_forward_size2_0) {
  Layer layer;
  layer.SetSize(2);
  layer.SetOutputData({0.0, 0.0});

  Weights weights;
  weights.Resize(2, 1);
  weights.Reset(1.0);

  auto result = FeedForwardLayer(layer, weights);

  EXPECT_FLOAT_EQ(0.0, result.back());
}

TEST(Layer, feed_forward_size2_1) {
  Layer layer;
  layer.SetSize(2);
  layer.SetOutputData({1.0, 1.0});

  Weights weights;
  weights.Resize(2, 1);
  weights.Reset(1.0);

  auto result = FeedForwardLayer(layer, weights);

  EXPECT_FLOAT_EQ(2.0, result.back());
}

TEST(Layer, layer_with_bias_node) {
  Layer layer;
  layer.SetSize(2);
  layer.SetBiasNode(true);
  layer.SetOutputData({1.0, 1.0});

  Weights weights;
  weights.Resize(layer.OutputSize(), 1);
  weights.Reset(1.0);

  auto result = FeedForwardLayer(layer, weights);

  EXPECT_FLOAT_EQ(3.0, result.back());
}

TEST(TransferFunction, linear) {
  const double input{0.5};
  const double output{Linear::TransferFunction(input)};

  EXPECT_FLOAT_EQ(input, output);
}

TEST(TransferFunction, sigmoud) {
  const double input{0.5};
  const double output{Sigmoid::TransferFunction(input)};

  EXPECT_FLOAT_EQ(1.0 / (1.0 + exp(-input)), output);
}

TEST(CalculateDeltaSum, sigmoid) {
  Layer layer;
  layer.SetSize(1);
  layer.SetInputData({1.235});
  layer.SetOutputData({0.77});

  Data target{0.0};

  auto result = CalculateDeltaSum<Sigmoid>(layer, target);

  EXPECT_FLOAT_EQ(-0.13439891, result.front());
}

TEST(CalculateDeltaWeights, sigmoid_layer_1x1) {
  Layer layer;
  layer.SetSize(1);
  layer.SetOutputData({0.5});

  Data delta_sum{-1.0};

  auto result = CalculateDeltaWeights(layer, delta_sum);

  EXPECT_FLOAT_EQ(-0.5, result(0, 0));
}

TEST(CalculateDeltaWeights, sigmoid_layer_5x1) {
  Layer layer;
  layer.SetSize(5);
  layer.SetOutputData({0.5, 0.5, 0.5, 0.5, 0.5});

  Data delta_sum{-1.0};

  auto result = CalculateDeltaWeights(layer, delta_sum);

  for (size_t i{}; i < layer.OutputSize(); ++i) {
    EXPECT_FLOAT_EQ(-0.5, result(i, 0));
  }
}

TEST(CalculateDeltaWeights, sigmoid_layer_5x2) {
  Layer layer;
  layer.SetSize(5);
  layer.SetOutputData({0.5, 0.5, 0.5, 0.5, 0.5});

  Data delta_sum{-1.0, 1.0};

  auto result = CalculateDeltaWeights(layer, delta_sum);

  for (size_t i{}; i < layer.OutputSize(); ++i) {
    EXPECT_FLOAT_EQ(-0.5, result(i, 0));
    EXPECT_FLOAT_EQ(0.5, result(i, 1));
  }
}

TEST(BackPropagate, sigmoid_layer_1x1) {
  Layer layer;
  layer.SetSize(2);
  layer.SetOutputData({0.5, 1.0});
  Weights weights;
  weights.Resize(2, 1);
  weights.Reset(1.0);

  const auto next_layer_data = FeedForwardLayer(layer, weights);

  Layer out_layer;
  out_layer.SetSize(1);
  out_layer.SetInputData(next_layer_data);
  out_layer.SetOutputData(Sigmoid::TransferFunction(next_layer_data));

  Data target{0.0};
  const auto error = CalculateError(out_layer.GetOutputData(), target);

  const auto delta_sum = CalculateDeltaSum<Sigmoid>(out_layer, target);

  const auto delta_weights = CalculateDeltaWeights(layer, delta_sum);

  Weights weights_opt;
  weights_opt.Resize(2, 1);
  for (size_t i{}; i < weights_opt.Ninput() * weights_opt.Noutput(); ++i) {
    weights_opt(i) = weights(i) + 0.5 * delta_weights(i);
  }

  const auto next_layer_data_opt = FeedForwardLayer(layer, weights_opt);
  Layer out_layer_opt;
  out_layer_opt.SetSize(1);
  out_layer_opt.SetInputData(next_layer_data_opt);
  out_layer_opt.SetOutputData(Sigmoid::TransferFunction(next_layer_data_opt));
  const auto error_opt = CalculateError(out_layer_opt.GetOutputData(), target);

  EXPECT_LT(error_opt, error);
}

TEST(Network, set_layout) {
  Network<Linear> network;
  const Layout layout = {1, 1};

  network.SetLayout(layout);

  const auto read_layout = network.GetLayout();

  EXPECT_EQ(layout, read_layout);
}

TEST(Network, feed_forward_0) {
  Network<Linear> network;
  network.SetLayout({1, 1});
  network.ResetWeights(1.0);
  network.SetInput({0.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(0.0, output.front());
}

TEST(Network, feed_forward_1) {
  Network<Linear> network;
  network.SetLayout({1, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(1.0, output.front());
}

TEST(Network, feed_forward_2x1_0) {
  Network<Linear> network;
  network.SetLayout({2, 1});
  network.ResetWeights(1.0);
  network.SetInput({0.0, 0.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(0.0, output.front());
}

TEST(Network, feed_forward_2x1_1) {
  Network<Linear> network;
  network.SetLayout({2, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0, 1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(2.0, output.front());
}

TEST(Network, feed_forward_2x1x1_1) {
  Network<Linear> network;
  network.SetLayout({2, 1, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0, 1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(2.0, output.front());
}

TEST(Network, feed_forward_2x4x1_1) {
  Network<Linear> network;
  network.SetLayout({2, 4, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0, 1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(8.0, output.front());
}

TEST(Network, feed_forward_2x4x8x16x32x1_1) {
  Network<Linear> network;
  network.SetLayout({2, 4, 8, 16, 32, 1});
  network.ResetWeights(1.0);
  network.SetInput({1.0, 1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(32768.0, output.front());
}

TEST(Network, feed_forward_2x2x1_1_with_bias) {
  Network<Linear> network;
  network.SetLayout({2, 2, 1}, true);
  network.ResetWeights(1.0);
  network.SetInput({1.0, 1.0});

  network.FeedForward();

  const auto output = network.GetOutput();

  EXPECT_FLOAT_EQ(7.0, output.front());
}

TEST(Network, back_propagate_2x1) {
  Network<Linear> network;
  network.SetLayout({2, 1});
  network.ResetWeights(1.0);
  network.SetInput({0.5, 1.0});

  Data target{0.0};

  network.FeedForward();
  const double error = network.GetError(target);

  network.BackPropagate(target);

  network.FeedForward();
  const double error_opt = network.GetError(target);

  EXPECT_LT(error_opt, error);
}

TEST(Network, back_propagate_2x2) {
  Network<Linear> network;
  network.SetLayout({2, 2});
  network.ResetWeights(1.0);
  network.SetInput({0.5, 1.0});

  Data target{0.0, 0.0};

  network.FeedForward();
  const double error = network.GetError(target);

  network.BackPropagate(target);

  network.FeedForward();
  const double error_opt = network.GetError(target);

  EXPECT_LT(error_opt, error);
}

TEST(Network, back_propagate_1x1x1) {
  Network<Linear> network;
  network.SetLayout({1, 1, 1});
  network.ResetWeights(1.0);
  network.SetInput({0.5});

  Data target{0.0};

  network.FeedForward();
  const double error = network.GetError(target);

  network.BackPropagate(target);

  network.FeedForward();
  const double error_opt = network.GetError(target);

  EXPECT_LT(error_opt, error);
}

TEST(Network, back_propagate_2x2x1) {
  Network<Linear> network;
  network.SetLayout({2, 2, 1});
  network.ResetWeights(1.0);
  network.SetInput({0.5, 1.0});

  Data target{0.0};

  network.FeedForward();
  const double error = network.GetError(target);

  network.BackPropagate(target);

  network.FeedForward();
  const double error_opt = network.GetError(target);

  EXPECT_LT(error_opt, error);
}

// TEST(Network, set_weights){}
// TEST(Network, set_weights_with wrong_dimension){}
// TEST(Network, randomize_weights)

TEST(Network, back_propagate_2x3x1_with_weights) {
  std::vector<Weights> weights;
  weights.resize(2);
  weights[0].Resize(2, 3);
  weights[0].SetWeights({0.8, 0.2, 0.4, 0.9, 0.3, 0.5});
  weights[1].Resize(3, 1);
  weights[1].SetWeights({0.3, 0.5, 0.9});

  Network<Sigmoid> network;
  network.SetLearnFactor(1.0);
  network.SetLayout({2, 3, 1});
  network.ResetWeights(1.0);
  network.SetWeights(std::move(weights));
  network.SetInput({1.0, 1.0});

  Data target{0.0};

  network.FeedForward();
  const double error = network.GetError(target);

  network.BackPropagate(target);

  network.FeedForward();
  const double error_opt = network.GetError(target);

  std::vector<Weights> weights_target;
  weights_target.resize(2);
  weights_target[0].Resize(2, 3);
  weights_target[0].Resize(2, 3);
  weights_target[0].SetWeights({0.712, 0.112, 0.355, 0.855, 0.268, 0.468});
  weights_target[1].Resize(3, 1);
  weights_target[1].SetWeights({0.166, 0.329, 0.708});

  for (size_t i{}; i < weights_target.size(); ++i) {
    for (size_t j{}; j < weights_target[i].Size(); ++j) {
      // skip for now, since i do not have any ground truth data
      // EXPECT_FLOAT_EQ(weights_target[i](j), network.GetWeights()[i](j));
    }
  }

  EXPECT_LT(error_opt, error);
}

TEST(Network, back_propagate_2x1_with_bias) {
  Network<Linear> network;
  network.SetLayout({2, 1}, true);
  network.ResetWeights(1.0);
  network.SetInput({0.5, 1.0});

  Data target{0.0};

  network.FeedForward();
  const double error = network.GetError(target);

  network.BackPropagate(target);

  network.FeedForward();
  const double error_opt = network.GetError(target);

  EXPECT_LT(error_opt, error);
}

TEST(Network, back_propagate_2x1_with_bias_sigmoid) {
  Network<Sigmoid> network;
  network.SetLayout({2, 1}, true);
  network.ResetWeights(1.0);
  network.SetInput({0.5, 1.0});

  Data target{0.0};

  network.FeedForward();
  const double error = network.GetError(target);

  network.BackPropagate(target);

  network.FeedForward();
  const double error_opt = network.GetError(target);

  EXPECT_LT(error_opt, error);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}