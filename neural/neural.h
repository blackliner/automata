#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

using Data = std::vector<double>;

// using Weights = std::vector<std::vector<double>>;
class Weights {
  // x = columns = n_input, y = rows = n_output
 public:
  double& operator()(size_t x, size_t y) {
    return m_data[x + y * m_columns];
  }

  const double& operator()(size_t x, size_t y) const {
    return m_data[x + y * m_columns];
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
  double value{};
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

  void SetData(const Data& data) {
    size_t iterate_size{m_nodes.size()};
    if (m_has_bias_node) {
      --iterate_size;
    }

    assert(iterate_size == data.size());

    for (size_t i{}; i < iterate_size; ++i) {
      m_nodes[i].value = data[i];
    }

    if (m_has_bias_node) {
      m_nodes.back().value = 1.0;
    }
  }

  Data GetData() const {
    Data data;
    data.reserve(m_nodes.size());
    for (const auto& node : m_nodes) {
      data.push_back(node.value);
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

Data FeedForwardLayer(const Layer& first_layer, const Weights& weights, TFtype tf_type = TFtype::LINEAR) {
  assert(weights.Ninput() == first_layer.InputSize());

  Data return_data;
  return_data.reserve(weights.Noutput());

  for (size_t n_output{}; n_output < weights.Noutput(); ++n_output) {
    double sum{};
    for (size_t n_input{}; n_input < weights.Ninput(); ++n_input) {
      sum += first_layer.GetData()[n_input] * weights(n_input, n_output);
    }
    sum = TransferFunction(sum, tf_type);
    return_data.push_back(sum);
  }

  return return_data;
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

  Layout GetLayout() const {
    return m_layout;
  }

  void SetInput(const Data& data) {
    assert(!m_layers.empty());
    m_layers.front().SetData(data);
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
      m_layers[layer_n + 1].SetData(data);
    }
  }

 private:
  Layout m_layout;
  std::vector<Weights> m_weights;
  std::vector<Layer> m_layers;
};
