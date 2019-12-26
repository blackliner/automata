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
  double& operator()(size_t x, size_t y) {
    return m_data[x + y * m_columns];
  }

  const double& operator()(size_t x, size_t y) const {
    return m_data[x + y * m_columns];
  }

  double& operator()(size_t idx) {
    return m_data[idx];
  }

  const double& operator()(size_t idx) const {
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

  size_t Ninput() const {
    return m_columns;
  }

  size_t Noutput() const {
    return m_rows;
  }

 private:
  size_t m_columns{};
  size_t m_rows{};
  std::vector<double> m_data;
};

struct Node {
  double in_value{};   // here are the weighted inputs summed up
  double out_value{};  // this is tf(in_value)
};

class Layer {
 public:
  void SetSize(size_t new_size) {
    m_size = new_size;
    Resize();
  }

  size_t InputSize() const {
    return m_nodes.size();
  }

  size_t OutputSize() const {
    return m_has_bias_node ? m_nodes.size() - 1 : m_nodes.size();
  }

  void SetOutputData(const Data& data) {
    size_t iterate_size{m_nodes.size()};
    if (m_has_bias_node) {
      --iterate_size;
    }

    assert(iterate_size == data.size());

    for (size_t i{}; i < iterate_size; ++i) {
      m_nodes[i].out_value = data[i];
    }

    if (m_has_bias_node) {
      m_nodes.back().out_value = 1.0;
    }
  }

  void SetInputData(const Data& data) {
    size_t iterate_size{m_nodes.size()};
    if (m_has_bias_node) {
      --iterate_size;
    }

    assert(iterate_size == data.size());

    for (size_t i{}; i < iterate_size; ++i) {
      m_nodes[i].in_value = data[i];
    }
  }

  Data GetData() const {
    Data data;
    data.reserve(m_nodes.size());
    for (const auto& node : m_nodes) {
      data.push_back(node.out_value);
    }
    return data;
  }

  double GetData(size_t idx) const {
    return m_nodes[idx].out_value;
  }

  Data GetInputData() const {
    Data data;
    data.reserve(m_nodes.size());
    for (const auto& node : m_nodes) {
      data.push_back(node.in_value);
    }
    return data;
  }

  void SetBiasNode(bool new_value) {
    m_has_bias_node = new_value;
    Resize();
  }

 private:
  std::vector<Node> m_nodes;
  bool m_has_bias_node{};
  size_t m_size{};

  void Resize() {
    if (m_has_bias_node) {
      m_nodes.resize(m_size + 1);
    } else {
      m_nodes.resize(m_size);
    }
  }
};

enum class TFtype { LINEAR, SIGMOID };

double TransferFunction(double value, TFtype tf_type = TFtype::LINEAR) {
  switch (tf_type) {
    case TFtype::LINEAR:
      return value;
    case TFtype::SIGMOID:
      return 1.0 / (1.0 + exp(-value));
    default:
      return value;
  }
}

Data TransferFunction(const Data& data, TFtype tf_type = TFtype::LINEAR) {
  Data result;
  result.reserve(data.size());
  for (const auto& value : data) {
    result.push_back(TransferFunction(value, tf_type));
  }
  return result;
}

double InverseTransferFunction(double value, TFtype tf_type = TFtype::LINEAR) {
  switch (tf_type) {
    case TFtype::LINEAR:
      return 1.0;
    case TFtype::SIGMOID:
      return TransferFunction(value, TFtype::SIGMOID) * (1.0 - TransferFunction(value, TFtype::SIGMOID));
    default:
      return 1.0;
  }
}

Data InverseTransferFunction(const Data& data, TFtype tf_type = TFtype::LINEAR) {
  Data result;
  result.reserve(data.size());
  for (const auto& value : data) {
    result.push_back(InverseTransferFunction(value, tf_type));
  }
  return result;
}

Data FeedForwardLayer(const Layer& layer, const Weights& weights) {
  assert(weights.Ninput() == layer.InputSize());

  Data return_data;
  return_data.reserve(weights.Noutput());

  for (size_t n_output{}; n_output < weights.Noutput(); ++n_output) {
    double sum{};
    for (size_t n_input{}; n_input < weights.Ninput(); ++n_input) {
      sum += layer.GetData()[n_input] * weights(n_input, n_output);
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

Data CalculateDeltaSum(const Layer& layer, const Data& target, TFtype tf_type = TFtype::LINEAR) {
  const auto delta = Subtract(target, layer.GetData());
  const auto derivative = InverseTransferFunction(layer.GetInputData(), tf_type);
  const auto delta_sum = Dot(derivative, delta);

  return delta_sum;
}

Weights CalculateDeltaWeights(const Layer& layer, const Data& delta_sum) {
  Weights result;
  result.Resize(layer.OutputSize(), delta_sum.size());

  for (size_t layer_n{}; layer_n < layer.OutputSize(); ++layer_n) {
    for (size_t output_n{}; output_n < delta_sum.size(); ++output_n) {
      result(layer_n, output_n) = delta_sum[output_n] * layer.GetData(layer_n);
    }
  }

  return result;
}

using Layout = std::vector<size_t>;

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

    const auto required_weights = layout.size() - 1;
    m_weights.resize(required_weights);

    for (size_t n_weight{}; n_weight < required_weights; ++n_weight) {
      m_weights[n_weight].Resize(m_layers[n_weight].InputSize(), m_layers[n_weight + 1].OutputSize());
    }
  }

  void SetTFtype(TFtype tf_type) {
    m_tf_type = tf_type;
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

  Data GetOutput() const {
    assert(!m_layers.empty());
    return m_layers.back().GetData();
  }

  void ResetWeights(double new_value = 0.0) {
    for (auto& weight : m_weights) {
      weight.Reset(new_value);
    }
  }

  void FeedForward() {
    for (size_t layer_n{}; layer_n < m_layers.size() - 1; ++layer_n) {
      const auto data = FeedForwardLayer(m_layers[layer_n], m_weights[layer_n]);
      m_layers[layer_n + 1].SetInputData(data);
      m_layers[layer_n + 1].SetOutputData(TransferFunction(data, m_tf_type));
    }
  }

  Data CalcHiddenDeltaSum(const Weights& weight, const Data& delta_sum, TFtype tf_type = TFtype::LINEAR) {
    Data result;
    result.resize(weight.Ninput());

    for (size_t input_n{}; input_n < result.size(); ++input_n) {
      for (size_t output_n{}; output_n < weight.Noutput(); ++output_n) {
        result[input_n] += weight(input_n, output_n) * delta_sum[output_n];
      }

      result[input_n] *= InverseTransferFunction(result[input_n], tf_type);
    }

    return result;
  }

  void BackPropagate(const Data& target, TFtype tf_type = TFtype::LINEAR) {
    auto delta_weights{m_weights};

    Data delta_sum;
    for (size_t layer_n = m_layers.size() - 1; layer_n > 0; --layer_n) {
      if (layer_n == m_layers.size() - 1)  // output layer!
      {
        delta_sum = CalculateDeltaSum(m_layers[layer_n], target, tf_type);
      } else {
        delta_sum = CalcHiddenDeltaSum(m_weights[layer_n - 1], delta_sum, tf_type);
      }

      delta_weights[layer_n - 1] = CalculateDeltaWeights(m_layers[layer_n - 1], delta_sum);

      // Weights weights_opt;
      // weights_opt.Resize(m_weights.back().Ninput(), m_weights.back().Noutput());
    }

    for (size_t weight_n{}; weight_n < m_weights.size(); ++weight_n) {
      for (size_t i{}; i < m_weights[weight_n].Ninput() * m_weights[weight_n].Noutput(); ++i) {
        // weights_opt(i) = m_weights.back()(i) + 0.5 * delta_weights(i);
        m_weights[weight_n](i) = m_weights[weight_n](i) + m_learn_factor * delta_weights[weight_n](i);
      }
    }
  }

  double GetError(const Data& target) {
    return CalculateError(GetOutput(), target);
  }

  const std::vector<Weights>& GetWeights() const {
    return m_weights;
  }

 private:
  double m_learn_factor{0.5};
  Layout m_layout;
  TFtype m_tf_type{TFtype::LINEAR};
  std::vector<Weights> m_weights;
  std::vector<Layer> m_layers;
};
