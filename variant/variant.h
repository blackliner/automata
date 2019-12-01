#include <cstddef>

template <std::size_t index, typename... Types>
union variadic_union {};

template <std::size_t index, typename Tfirst, typename... Trest>
union variadic_union<index, Tfirst, Trest...> {
  Tfirst m_first;
  static constexpr std::size_t m_index = index;

  template <std::size_t get_index>
  constexpr auto& get() {
    if constexpr (get_index == m_index) return m_first;
    // else
    //  return m_rest.get<get_index>();
  }

  variadic_union<index + 1, Trest...> m_rest;
};

template <typename... T>
using my_variadic_union = variadic_union<0, T...>;

template <typename T>
class variant {
 public:
  T& operator=(const T& rhs) {
    return m_value = rhs;
  }

  bool operator==(const T& rhs) const {
    return m_value == rhs;
  }

 private:
  T m_value;
};

template <typename T>
bool operator==(const T& lhs, const variant<T>& rhs) {
  return rhs == lhs;
}