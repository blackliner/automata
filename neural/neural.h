#include <algorithm>
#include <array>
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
  size_t m_columns;
  size_t m_rows;
  std::vector<double> m_data;
};

struct Node {
  double value;
};

class Layer {
 public:
  void SetSize(size_t new_size) {
    m_nodes.resize(new_size);
  }

  size_t Size() const {
    return m_nodes.size();
  }

  void SetData(const Data& data) {
    for (size_t i{}; i < m_nodes.size(); ++i) {
      m_nodes[i].value = data[i];
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

 private:
  std::vector<Node> m_nodes;
};

Data FeedForwardLayer(const Layer& first_layer, const Weights& weights) {
  assert(weights.Ninput() == first_layer.Size());

  Data return_data;
  return_data.reserve(weights.Noutput());

  for (size_t n_output{}; n_output < weights.Noutput(); ++n_output) {
    double sum{};
    for (size_t n_input{}; n_input < weights.Ninput(); ++n_input) {
      sum += first_layer.GetData()[n_input] * weights(n_input, n_output);
    }
    return_data.push_back(sum);
  }

  return return_data;
}

using Layout = std::vector<size_t>;

class Network {
 public:
  void SetLayout(const Layout& layout) {
    assert(layout.size() > 1);
    assert(std::all_of(layout.begin(), layout.end(), [](const auto& value) { return value > 0; }));

    m_layout = layout;
    const auto required_weights = layout.size() - 1;
    m_weights.resize(required_weights);
    for (size_t n_weight{}; n_weight < required_weights; ++n_weight) {
      m_weights[n_weight].Resize(layout[n_weight], layout[n_weight + 1]);
    }

    if (layout.size() != m_layers.size()) {
      m_layers.resize(layout.size());
    }

    for (size_t i{}; i < m_layers.size(); ++i) {
      if (layout[i] != m_layers[i].Size()) {
        m_layers[i].SetSize(layout[i]);
      }
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
