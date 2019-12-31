#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <vector>

#include "lib/matrix2d.h"

using Matrix = Matrix_2d<double>;
using Weights = Matrix_2d<double>;

double CalculateError(const Matrix& lhs, const Matrix& rhs) {
  assert(lhs.Ninput() == rhs.Ninput());
  assert(lhs.Noutput() == rhs.Noutput());

  double error{};
  for (size_t i{}; i < lhs.Size(); ++i) {
    error += 0.5 * (lhs(i) - rhs(i)) * (lhs(i) - rhs(i));
  }
  return error;
}

template <typename T>
double CalculateError(const Matrix_2d<T>& lhs, const Matrix_2d<T>& rhs) {
  assert(lhs.Size() == rhs.Size());
  double error{};

  for (size_t i{}; i < lhs.Size(); ++i) {
    error += (lhs(i) - rhs(i)) * (lhs(i) - rhs(i));
  }

  error *= 0.5;
  return error;
}

class Layer {
 public:
  void SetSize(size_t new_size) {
    m_size = new_size;
    Resize();
  }

  constexpr size_t InputSize() const noexcept {
    return m_size;
  }

  constexpr size_t OutputSize() const noexcept {
    return m_size + m_has_bias_node;
  }

  void SetOutputData(Matrix data) {
    assert(m_size == data.Size());

    m_results = std::move(data);

    if (m_has_bias_node) {
      m_results.Resize(1u, m_size + 1);
      m_results(m_size) = 1.0;
    }
  }

  template <typename TF>
  void RunTF() {
    m_results = TF::TransferFunction(m_sums);

    if (m_has_bias_node) {
      m_results.Resize(1u, m_size + 1);
      m_results(m_size) = 1.0;
      // m_results.WTF().push_back(1.0);
    }
  }

  void SetInputData(Matrix data) {
    assert(m_size == data.Size());

    m_sums = std::move(data);
  }

  const Matrix& GetOutputData() const noexcept {
    return m_results;
  }

  double GetData(size_t idx) const noexcept {
    return m_results(idx);
  }

  const Matrix& GetInputData() const noexcept {
    return m_sums;
  }

  void SetBiasNode(bool new_value) {
    m_has_bias_node = new_value;
    Resize();
  }

 private:
  Matrix_2d<double> m_sums;  // m sums has no entry for the bias value!!
  Matrix_2d<double> m_results;

  bool m_has_bias_node{};
  size_t m_size{};

  void Resize() {
    m_sums.Resize(1u, InputSize());
    m_results.Resize(1u, OutputSize());

    if (m_has_bias_node) {
      m_results(1, OutputSize() - 1) = 1.0;
    }
  }
};

struct Linear {
  static constexpr double TransferFunction(double value) noexcept {
    return value;
  }

  static Matrix TransferFunction(const Matrix& data) noexcept {
    return data;
  }

  static constexpr double InverseTransferFunction(double) noexcept {
    return 1.0;
  }

  static Matrix InverseTransferFunction(Matrix data) noexcept {
    for (size_t i{}; i < data.Size(); ++i) {
      data(i) = InverseTransferFunction(data(i));
    }
    return data;
  }
};

struct Sigmoid {
  static constexpr double TransferFunction(double value) noexcept {
    return 1.0 / (1.0 + exp(-value));
  }

  static Matrix TransferFunction(Matrix data) noexcept {
    for (size_t i{}; i < data.Size(); ++i) {
      data(i) = TransferFunction(data(i));
    }
    return data;
  }

  static constexpr double InverseTransferFunction(double value) noexcept {
    const auto tf_value = TransferFunction(value);
    return tf_value * (1.0 - tf_value);
  }

  static Matrix InverseTransferFunction(Matrix data) noexcept {
    for (size_t i{}; i < data.Size(); ++i) {
      data(i) = InverseTransferFunction(data(i));
    }
    return data;
  }
};

struct Relu {
  static constexpr double kLeek{0.1};

  static constexpr double TransferFunction(double value) noexcept {
    return value >= 0.0 ? value : kLeek * value;
  }

  static Matrix TransferFunction(Matrix data) noexcept {
    for (size_t i{}; i < data.Size(); ++i) {
      data(i) = TransferFunction(data(i));
    }
    return data;
  }

  static constexpr double InverseTransferFunction(double value) noexcept {
    return value >= 0.0 ? 1.0 : kLeek;
  }

  static Matrix InverseTransferFunction(Matrix data) noexcept {
    for (size_t i{}; i < data.Size(); ++i) {
      data(i) = InverseTransferFunction(data(i));
    }
    return data;
  }
};

struct Tanh {
  static constexpr double TransferFunction(double value) noexcept {
    return 2.0 * Sigmoid::TransferFunction(value) - 1.0;
  }

  static Matrix TransferFunction(Matrix data) noexcept {
    for (size_t i{}; i < data.Size(); ++i) {
      data(i) = TransferFunction(data(i));
    }
    return data;
  }

  static constexpr double InverseTransferFunction(double value) noexcept {
    return 2.0 * Sigmoid::InverseTransferFunction(value);
  }

  static Matrix InverseTransferFunction(Matrix data) noexcept {
    for (size_t i{}; i < data.Size(); ++i) {
      data(i) = InverseTransferFunction(data(i));
    }
    return data;
  }
};

Matrix FeedForwardLayer(const Layer& layer, const Weights& weights) {
  assert(weights.Ninput() == layer.OutputSize());

  Matrix return_data;
  return_data.Resize(1u, weights.Noutput());

  for (size_t n_output{}; n_output < weights.Noutput(); ++n_output) {
    for (size_t n_input{}; n_input < weights.Ninput(); ++n_input) {
      return_data(n_output) += layer.GetOutputData()(n_input) * weights(n_input, n_output);
    }
  }

  return return_data;
}

// This fcn only serves for the calculation of the output layer
template <typename TF>
Matrix CalculateDeltaSum(const Layer& layer, const Matrix& target) {
  const auto delta = target - layer.GetInputData();
  const auto derivative = TF::InverseTransferFunction(layer.GetInputData());
  const auto delta_sum = derivative.Dot(delta);

  return delta_sum;
}

template <typename TF>
Matrix CalcHiddenDeltaSum(const Layer& layer, const Weights& weight, const Matrix& delta_sum) {
  Matrix result;
  result.Resize(1u, layer.InputSize());

  for (size_t input_n{}; input_n < result.Size(); ++input_n) {
    for (size_t output_n{}; output_n < weight.Noutput(); ++output_n) {
      result(input_n) += weight(input_n, output_n) * delta_sum(output_n);
    }
  }

  const auto derivative = TF::InverseTransferFunction(layer.GetInputData());

  // result = Dot(derivative, result);
  result = derivative.Dot(result);

  return result;
}

Weights CalculateDeltaWeights(const Layer& layer, const Matrix& delta_sum) {
  Weights result;
  result.Resize(layer.OutputSize(), delta_sum.Size());

  for (size_t node_n{}; node_n < layer.OutputSize(); ++node_n) {
    for (size_t output_n{}; output_n < delta_sum.Size(); ++output_n) {
      result(node_n, output_n) = delta_sum(output_n) * layer.GetData(node_n);
    }
  }
  return result;
}

using Layout = std::vector<size_t>;

template <typename TF>
class Network {
 public:
  void SetLayout(const Layout& layout, bool wiht_bias = false) {
    assert(layout.size() > 1);
    assert(std::all_of(layout.begin(), layout.end(), [](const auto& value) { return value > 0; }));

    m_layout = layout;

    m_layers.resize(layout.size());

    for (size_t i{}; i < m_layers.size(); ++i) {
      m_layers[i].SetSize(layout[i]);
      m_layers[i].SetBiasNode(wiht_bias);
    }

    // last layer does not need a bias node
    m_layers.back().SetBiasNode(false);

    const auto required_weights = layout.size() - 1;
    m_weights.resize(required_weights);

    for (size_t n_weight{}; n_weight < required_weights; ++n_weight) {
      const auto weight_in_size = m_layers[n_weight].OutputSize();
      const auto weight_out_size = m_layers[n_weight + 1].InputSize();
      m_weights[n_weight].Resize(weight_in_size, weight_out_size);
    }
  }

  void SetLearnFactor(double new_factor) {
    m_learn_factor = new_factor;
  }

  Layout GetLayout() const {
    return m_layout;
  }

  void SetInput(const Matrix& data) {
    assert(!m_layers.empty());
    m_layers.front().SetOutputData(data);
  }

  const Matrix& GetOutput() const {
    assert(!m_layers.empty());
    return m_layers.back().GetInputData();
  }

  void ResetWeights(double new_value = 0.0) {
    for (auto& weight : m_weights) {
      weight.Reset(new_value);
    }
  }

  void RandomizeWeights() {
    for (auto& weight : m_weights) {
      weight.Randomize();
    }
  }

  void FeedForward() {
    for (size_t layer_n{}; layer_n < m_layers.size() - 1; ++layer_n) {
      const auto data = FeedForwardLayer(m_layers[layer_n], m_weights[layer_n]);
      m_layers[layer_n + 1].SetInputData(data);

      m_layers[layer_n + 1].RunTF<TF>();
      // TODO not eally faster...
      // const auto tf_data = TF::TransferFunction(data);
      // m_layers[layer_n + 1].SetOutputData(std::move(tf_data));
    }
  }

  std::vector<Weights> BackPropagate(const Matrix& target) {
    std::vector<Weights> delta_weights;
    delta_weights.resize(m_weights.size());

    Matrix delta_sum;
    for (size_t layer_n = m_layers.size() - 1; layer_n > 0; --layer_n) {
      if (layer_n == m_layers.size() - 1)  // output layer!
      {
        delta_sum = CalculateDeltaSum<TF>(m_layers[layer_n], target);
      } else {
        delta_sum = CalcHiddenDeltaSum<TF>(m_layers[layer_n], m_weights[layer_n], delta_sum);
      }

      delta_weights[layer_n - 1] = CalculateDeltaWeights(m_layers[layer_n - 1], delta_sum);
    }

    return delta_weights;
  }

  void AddWeights(const std::vector<Weights>& weights) {
    assert(m_weights.size() == weights.size());
    for (size_t i{}; i < m_weights.size(); ++i) {
      assert(m_weights[i].Size() == weights[i].Size());
    }

    for (size_t i{}; i < m_weights.size(); ++i) {
      for (size_t j{}; j < m_weights[i].Size(); ++j) {
        m_weights[i](j) += m_learn_factor * weights[i](j);
      }
    }
  }

  constexpr double GetError(const Matrix& target) const {
    return CalculateError(GetOutput(), target);
  }

  const std::vector<Weights>& GetWeights() const {
    return m_weights;
  }

  void SetWeights(std::vector<Weights> weights) {
    m_weights = std::move(weights);
  }

 private:
  double m_learn_factor{0.1};
  Layout m_layout;

  std::vector<Weights> m_weights;
  std::vector<Layer> m_layers;
};
