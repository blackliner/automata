

template <typename T>
class Matrix_2d {
  // x = columns = n_input, y = rows = n_output
 public:
  Matrix_2d() = default;
  Matrix_2d(std::initializer_list<T> data) : m_data(data), m_columns(1), m_rows(data.size()) {
  }

  Matrix_2d(const Matrix_2d& other) = default;
  Matrix_2d& operator=(const Matrix_2d& other) = default;

  Matrix_2d(Matrix_2d&& other) = default;
  Matrix_2d& operator=(Matrix_2d&& other) = default;

  ~Matrix_2d() = default;
  T& operator()(size_t x, size_t y) noexcept {
    return m_data[x + y * m_columns];
  }

  const T& operator()(size_t x, size_t y) const noexcept {
    return m_data[x + y * m_columns];
  }

  T& operator()(size_t idx) noexcept {
    return m_data[idx];
  }

  const T& operator()(size_t idx) const noexcept {
    return m_data[idx];
  }

  //   Matrix_2d& operator=(Matrix_2d other) noexcept {
  //     m_data = std::move(other.m_data);
  //     m_columns = other.m_columns;
  //     m_rows = other.m_rows;

  //     return *this;
  //   }

  Matrix_2d& operator+=(const Matrix_2d& rhs) {
    assert(m_columns == rhs.m_columns);
    assert(m_rows == rhs.m_rows);

    for (size_t i{}; i < m_data.size(); ++i) {
      m_data[i] += rhs.m_data[i];
    }
    return *this;
  }

  Matrix_2d operator+(const Matrix_2d& rhs) {
    assert(m_columns == rhs.m_columns);
    assert(m_rows == rhs.m_rows);

    Matrix_2d result;
    result.Resize(m_columns, m_rows);

    for (size_t i{}; i < m_data.size(); ++i) {
      result.m_data[i] = m_data[i] + rhs.m_data[i];
    }
    return result;
  }

  Matrix_2d& operator-=(const Matrix_2d& rhs) {
    assert(m_columns == rhs.m_columns);
    assert(m_rows == rhs.m_rows);

    for (size_t i{}; i < m_data.size(); ++i) {
      m_data[i] -= rhs.m_data[i];
    }
    return *this;
  }

  Matrix_2d operator-(const Matrix_2d& rhs) const {
    assert(m_columns == rhs.m_columns);
    assert(m_rows == rhs.m_rows);

    Matrix_2d result;
    result.Resize(m_columns, m_rows);

    for (size_t i{}; i < m_data.size(); ++i) {
      result.m_data[i] = m_data[i] - rhs.m_data[i];
    }
    return result;
  }

  Matrix_2d Dot(const Matrix_2d& rhs) const {
    assert(m_columns == rhs.m_columns);
    assert(m_rows == rhs.m_rows);

    Matrix_2d result;
    result.Resize(m_columns, m_rows);

    for (size_t i{}; i < Size(); ++i) {
      result(i) = m_data[i] * rhs(i);
    }

    return result;
  }

  void Resize(size_t n_columns, size_t n_rows) {
    assert(n_rows * n_columns != 0);
    m_rows = n_rows;
    m_columns = n_columns;
    m_data.resize(n_rows * n_columns);
  }

  void Reset(T new_value = T{}) {
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

  void SetWeights(std::initializer_list<T>&& new_data) {
    m_data = std::move(new_data);
  }

  void Randomize() {
    for (auto& value : m_data) {
      value = static_cast<T>(rand()) / RAND_MAX;
      value -= 0.5;
      value *= 2.0;
    }
  }

 private:
  std::vector<T> m_data;

  size_t m_columns{};
  size_t m_rows{};
};
