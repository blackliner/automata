#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <vector>

using Data = std::vector<double>;

double CalculateError(const Data& lhs, const Data& rhs) {
  assert(lhs.size() == rhs.size());
  double error{};
  for (size_t i{}; i < lhs.size(); ++i) {
    error += 0.5 * (lhs[i] - rhs[i]) * (lhs[i] - rhs[i]);
  }
  return error;
}

class Weights {
  // x = columns = n_input, y = rows = n_output
 public:
  double& operator()(size_t x, size_t y) noexcept {
    return m_data[x + y * m_columns];
  }

  const double& operator()(size_t x, size_t y) const noexcept {
    return m_data[x + y * m_columns];
  }

  double& operator()(size_t idx) noexcept {
    return m_data[idx];
  }

  const double& operator()(size_t idx) const noexcept {
    return m_data[idx];
  }

  void Resize(size_t n_columns, size_t n_rows) {
    assert(n_rows * n_columns != 0);
    m_rows = n_rows;
    m_columns = n_columns;
    m_data.resize(n_rows * n_columns);
  }

  void Reset(double new_value = 0.0) {
    for (double& value : m_data) {
      value = new_value;
    }
  }

  constexpr size_t Ninput() const noexcept {
    return m_columns;
  }

  constexpr size_t Noutput() const noexcept {
    return m_rows;
  }

  constexpr size_t Size() const noexcept {
    return m_columns * m_rows;
  }

  void SetWeights(std::initializer_list<double>&& new_data) {
    m_data = std::move(new_data);
  }

  void Randomize() {
    for (auto& value : m_data) {
      value = static_cast<double>(rand()) / RAND_MAX;
      value -= 0.5;
      value *= 2.0;
    }
  }

 private:
  size_t m_columns{};
  size_t m_rows{};
  Data m_data;
};

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

  void SetOutputData(Data data) {
    assert(m_size == data.size());

    m_results = std::move(data);

    if (m_has_bias_node) {
      m_results.push_back(1.0);
    }
  }

  void SetInputData(Data data) {
    assert(m_size == data.size());

    m_sums = std::move(data);
  }

  const Data& GetOutputData() const noexcept {
    return m_results;
  }

  double GetData(size_t idx) const noexcept {
    return m_results[idx];
  }

  const Data& GetInputData() const noexcept {
    return m_sums;
  }

  void SetBiasNode(bool new_value) {
    m_has_bias_node = new_value;
    Resize();
  }

 private:
  Data m_sums;  // m sums has no entry for the bias value!!
  Data m_results;

  bool m_has_bias_node{};
  size_t m_size{};

  void Resize() {
    m_sums.resize(InputSize());
    m_results.resize(OutputSize());
  }
};

struct Linear {
  static constexpr double TransferFunction(double value) noexcept {
    return value;
  }

  static Data TransferFunction(const Data& data) noexcept {
    return data;
  }

  static constexpr double InverseTransferFunction(double) noexcept {
    return 1.0;
  }

  static Data InverseTransferFunction(Data data) noexcept {
    for (size_t i{}; i < data.size(); ++i) {
      data[i] = InverseTransferFunction(data[i]);
    }
    return data;
  }
};

struct Sigmoid {
  static constexpr double TransferFunction(double value) noexcept {
    return 1.0 / (1.0 + exp(-value));
  }

  static Data TransferFunction(Data data) noexcept {
    for (size_t i{}; i < data.size(); ++i) {
      data[i] = TransferFunction(data[i]);
    }
    return data;
  }

  static constexpr double InverseTransferFunction(double value) noexcept {
    const auto tf_value = TransferFunction(value);
    return tf_value * (1.0 - tf_value);
  }

  static Data InverseTransferFunction(Data data) noexcept {
    for (size_t i{}; i < data.size(); ++i) {
      data[i] = InverseTransferFunction(data[i]);
    }
    return data;
  }
};

Data FeedForwardLayer(const Layer& layer, const Weights& weights) {
  assert(weights.Ninput() == layer.OutputSize());

  Data return_data;
  return_data.reserve(weights.Noutput());

  for (size_t n_output{}; n_output < weights.Noutput(); ++n_output) {
    double sum{};
    for (size_t n_input{}; n_input < weights.Ninput(); ++n_input) {
      sum += layer.GetOutputData()[n_input] * weights(n_input, n_output);
    }
    return_data.push_back(sum);
  }

  return return_data;
}

Data Subtract(const Data& lhs, const Data& rhs) {
  assert(lhs.size() == rhs.size());
  Data result(lhs.size());
  for (size_t i{}; i < lhs.size(); ++i) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}

Data Dot(const Data& lhs, const Data& rhs) {
  assert(lhs.size() == rhs.size());
  Data result(lhs.size());
  for (size_t i{}; i < lhs.size(); ++i) {
    result[i] = lhs[i] * rhs[i];
  }
  return result;
}

template <typename TF>
Data CalculateDeltaSum(const Layer& layer, const Data& target) {
  const auto delta = Subtract(target, layer.GetOutputData());
  const auto derivative = TF::InverseTransferFunction(layer.GetInputData());
  const auto delta_sum = Dot(derivative, delta);

  return delta_sum;
}

template <typename TF>
Data CalcHiddenDeltaSum(const Layer& layer, const Weights& weight, const Data& delta_sum) {
  Data result;
  result.resize(layer.InputSize());

  for (size_t input_n{}; input_n < result.size(); ++input_n) {
    for (size_t output_n{}; output_n < weight.Noutput(); ++output_n) {
      result[input_n] += weight(input_n, output_n) * delta_sum[output_n];
    }
  }

  const auto derivative = TF::InverseTransferFunction(layer.GetInputData());

  result = Dot(derivative, result);

  return result;
}

Weights CalculateDeltaWeights(const Layer& layer, const Data& delta_sum) {
  Weights result;
  result.Resize(layer.OutputSize(), delta_sum.size());

  for (size_t node_n{}; node_n < layer.OutputSize(); ++node_n) {
    for (size_t output_n{}; output_n < delta_sum.size(); ++output_n) {
      result(node_n, output_n) = delta_sum[output_n] * layer.GetData(node_n);
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
      m_layers[i].SetBiasNode(wiht_bias);
      m_layers[i].SetSize(layout[i]);
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

  void SetInput(const Data& data) {
    assert(!m_layers.empty());
    m_layers.front().SetOutputData(data);
  }

  const Data& GetOutput() const {
    assert(!m_layers.empty());
    return m_layers.back().GetOutputData();
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

      const auto tf_data = TF::TransferFunction(data);
      m_layers[layer_n + 1].SetOutputData(tf_data);
    }
  }

  void BackPropagate(const Data& target) {
    auto delta_weights{m_weights};

    Data delta_sum;
    for (size_t layer_n = m_layers.size() - 1; layer_n > 0; --layer_n) {
      if (layer_n == m_layers.size() - 1)  // output layer!
      {
        delta_sum = CalculateDeltaSum<TF>(m_layers[layer_n], target);
      } else {
        delta_sum = CalcHiddenDeltaSum<TF>(m_layers[layer_n], m_weights[layer_n], delta_sum);
      }

      delta_weights[layer_n - 1] = CalculateDeltaWeights(m_layers[layer_n - 1], delta_sum);
    }

    for (size_t weight_n{}; weight_n < m_weights.size(); ++weight_n) {
      for (size_t i{}; i < m_weights[weight_n].Ninput() * m_weights[weight_n].Noutput(); ++i) {
        m_weights[weight_n](i) = m_weights[weight_n](i) + m_learn_factor * delta_weights[weight_n](i);
      }
    }
  }

  constexpr double GetError(const Data& target) const {
    return CalculateError(GetOutput(), target);
  }

  const std::vector<Weights>& GetWeights() const {
    return m_weights;
  }

  void SetWeights(std::vector<Weights> weights) {
    m_weights = std::move(weights);
  }

 private:
  double m_learn_factor{0.5};
  Layout m_layout;

  std::vector<Weights> m_weights;
  std::vector<Layer> m_layers;
};
