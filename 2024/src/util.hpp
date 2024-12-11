#ifndef UTIL_HPP
#define UTIL_HPP
#include <charconv>
#include <cmath>
#include <cstdint>
#include <optional>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>

namespace parse {
constexpr auto to_string(const std::string_view &sv)
    -> std::optional<std::string> {
  return std::string(sv);
}

constexpr auto to_int(const std::string_view &sv) -> std::optional<int> {
  int value;
  auto [ptr, err] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
  if (err == std::errc{}) {
    return value;
  }
  return std::nullopt;
}

constexpr auto to_int64(const std::string_view &sv) -> std::optional<int64_t> {
  int64_t value;
  auto [ptr, err] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
  if (err == std::errc{}) {
    return value;
  }
  return std::nullopt;
}

constexpr auto to_uint64(const std::string_view &sv)
    -> std::optional<uint64_t> {
  uint64_t value;
  auto [ptr, err] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
  if (err == std::errc{}) {
    return value;
  }
  return std::nullopt;
}
} // namespace parse

template <typename Parser>
auto split(const std::string &s, const char delimiter,
           Parser parser = parse::to_string)
    -> std::vector<
        typename std::invoke_result_t<Parser, std::string_view>::value_type> {
  using T = typename std::invoke_result_t<Parser, std::string_view>::value_type;
  std::vector<T> result;
  for (const auto &token : std::views::split(s, delimiter) |
                               std::views::transform([&](auto &&token) {
                                 return std::string_view(token.begin(),
                                                         token.end());
                               })) {
    if (auto parsed = parser(token); parsed) {
      result.push_back(*parsed);
    }
  }
  return result;
}

template <typename T>
std::string join(const std::vector<T> &vec, std::string delimiter) {
  std::ostringstream s("");

  auto it = vec.cbegin();
  for (; it < vec.cend() - 1; ++it) {
    print(s, "{}{}", *it, delimiter);
  }
  print(s, "{}", *it);
  return s.str();
}

inline uint64_t concatenate(uint64_t x, uint64_t y) {
  uint64_t pow10 = 1;
  while (pow10 <= y)
    pow10 *= 10;
  return x * pow10 + y;
}
#endif // UTIL_HPP
