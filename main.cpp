#include <cstddef>
#include <type_traits>
#include <array>
#include <initializer_list>

template <typename T>
struct blocked_range {
  blocked_range() = delete; // 2-3. default prohibits our nice API idea
  blocked_range(T begin, T end, std::size_t grain_size = 1) {};
};

// the problem is N has no effect on std::initializer_list<<>> b_ranges
template <typename T, std::size_t N>
struct blocked_range_nd {
  // 2. template<typename TT>
  // 1. explicit blocked_range_nd(BlockedRanges... b_ranges) : arr{b_ranges...} {
  // 2. blocked_range_nd(std::initializer_list<TT> b_ranges) {// : arr{b_ranges...} {
  // 3. blocked_range_nd(std::initializer_list<blocked_range<T>> b_ranges) {
  // 4. explicit blocked_range_nd(const std::array<blocked_range<T>, N>& b_ranges) : arr{b_ranges} {
  explicit blocked_range_nd(const blocked_range<T> (&b_ranges) [N]) : arr{make_array_from_c_array(b_ranges)} {
    // 1. static_assert(sizeof...(BlockedRanges) == N);
  }

 private:

  // Start using index sequences
  template <std::size_t... Is>
  static std::array<blocked_range<T>, N> make_array_from_c_array_impl(const blocked_range<T> (&b_ranges) [N], std::index_sequence<Is...>) {
    return std::array<blocked_range<T>, N> {b_ranges[Is]...}; //read 'Is'-number of indices
  }

  static std::array<blocked_range<T>, N> make_array_from_c_array(const blocked_range<T> (&b_ranges) [N]) {
    return make_array_from_c_array_impl(b_ranges, std::make_index_sequence<N>{});
  }

  std::array<blocked_range<T>, N> arr;
};

int main() {
  // 1. blocked_range_nd<int, 2> t{blocked_range<int>{0, 2}, blocked_range<int>{0, 2}};
  // 2. We want blocked_range_nd<int, 2> {{0, 2}, {0, 2}}; the idea is initializer_list
  // These brackets can match to std::array
  // 3. blocked_range_nd<int, 2> {{0,2}, {0, 3}};
  // 4. blocked_range_nd<int, 2> {{{{0, 2}, {0, 3}}}}; // Can't believe this works. So many brackets... Explanation: I{} <-- blocked_range_nd, II{} <-- std::array, III{} <-- C-array in std::array, IV{} <-- blocked_range
  blocked_range_nd<int, 2> {{{0, 2}, {0, 3}}};
  return 0;
}
